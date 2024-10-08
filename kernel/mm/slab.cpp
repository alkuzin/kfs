/**
 * Monolithic Unix-like kernel from scratch.
 * Copyright (C) 2024 Alexander (@alkuzin).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cmath.hpp>
#include <kernel/printk.hpp>
#include <kernel/kernel.hpp>
#include <kernel/debug.hpp>
#include <kernel/core.hpp>
#include <kernel/slab.hpp>
#include <kernel/pmm.hpp>

using namespace kernel::core::memory;


namespace kernel {
namespace kmem {

inline const uint8_t SLAB_PAGES_ORDER {0};  // 2^order pages to allocate
inline const uint8_t CACHES_SIZE      {9};

static cache_t     caches[CACHES_SIZE]; // array of predefined caches
inline size_t      slab_pos = 0;        // current free slab position
inline slab_list_t slabs;               // allocated slabs list


void init(void) noexcept
{
    void *pages = pmm.alloc_pages(GFP::KERNEL, SLAB_PAGES_ORDER)->addr();

    // TODO: replace with panic():
    if (!pages) {
        printk(KERN_ERR "%s\n", "error to allocate pages for slabs structs");
        core::khalt();
    }

    slabs.m_head = reinterpret_cast<slab_t*>(pages);
    slabs.m_size = ((1 << SLAB_PAGES_ORDER) << PAGE_SHIFT) / sizeof(slab_t);

    // allocating pages for each slab objects
    void *page_ptr = nullptr;

    for (size_t i = 0; i < slabs.m_size; i++) {
        page_ptr = pmm.get_zeroed_page(GFP::KERNEL | GFP::ZERO)->addr();

        // TODO: replace with panic():
        if (!page_ptr) {
            printk(KERN_ERR "%s\n", "error to allocate pages for objects");
            core::khalt();
        }

        slabs.m_head[i].m_s_mem   = page_ptr;
        slabs.m_head[i].m_free    = page_ptr;
        slabs.m_head[i].m_next    = nullptr;
        slabs.m_head[i].m_prev    = nullptr;
        slabs.m_head[i].m_is_free = true;
        slabs.m_head[i].m_inuse   = 0;
    }

    caches[8].create("kmalloc-2k", 2_KB, 0);
    caches[7].create("kmalloc-1k", 1_KB, 0);
    caches[6].create("kmalloc-512", 512, 0);
    caches[5].create("kmalloc-256", 256, 0);
    caches[4].create("kmalloc-128", 128, 0);
    caches[3].create("kmalloc-64", 64, 0);
    caches[2].create("kmalloc-32", 32, 0);
    caches[1].create("kmalloc-16", 16, 0);
    caches[0].create("kmalloc-8", 8, 0);
}

void cache_t::create(const char *name, size_t size, uint32_t flags) noexcept
{
    // initializing cache structure
    m_list          = {nullptr, nullptr, 0};
    m_freelist      = {nullptr, nullptr, 0};
    m_objsize       = roundup_pow_of_two(size);
    m_gfporder      = kstd::ceil(kstd::log2(m_objsize));
    m_objnum        = PAGE_SIZE >> m_gfporder;
    m_flags         = flags;
    kstd::strncpy(m_name, name, CACHE_NAMELEN);
}

void *cache_t::alloc(uint8_t flags) noexcept
{
    (void)flags;    // TODO: handle SLAB_KERNEL

    if (m_list.m_size == 0)
        alloc_slab();

    slab_t *slab = m_list.m_next_free;
    void *ptr    = slab->m_free;

    // handle case if all objects in the last slab are used
    if (slab->m_inuse == m_objnum) {
        alloc_slab();
        slab = m_list.m_next_free;
        ptr  = slab->m_free;
    }

    // update slab info
    slab->m_inuse++;
    slab->m_free = reinterpret_cast<uint8_t*>(slab->m_free) + m_objsize;

    return ptr;
}

void cache_t::alloc_slab(void) noexcept
{
    bool is_allocated = false;
    slab_t *slab;

    // first looking into the freelist for free slabs
    if (m_freelist.m_head) {
        m_list.m_next_free->m_next             = m_freelist.m_next_free;
        m_freelist.m_next_free->m_next         = nullptr;
        m_freelist.m_next_free->m_prev->m_next = nullptr;

        if (m_freelist.m_size)
            m_freelist.m_next_free = m_freelist.m_next_free->m_prev;

        m_list.m_next_free->m_next->m_prev = m_list.m_next_free;
        m_list.m_next_free = m_list.m_next_free->m_next;

        m_list.m_size++;
        m_freelist.m_size--;
        is_allocated = true;
    }
    else {
        // if there is no free slabs in freelist - then search them
        // in the external slabs array (m_slabs)
        while (slab_pos < slabs.m_size) {
            slab = &slabs.m_head[slab_pos];

            if (slab->m_is_free) {
                slab->m_is_free = false;

                if (m_list.m_head) {
                    slab->m_prev               = m_list.m_next_free;
                    slab->m_next               = nullptr;
                    m_list.m_next_free->m_next = slab;
                    m_list.m_next_free         = slab;
                }
                else {
                    m_list.m_head      = slab;
                    m_list.m_next_free = slab;
                }

                m_list.m_size++;
                is_allocated = true;
                break;
            }

            slab_pos++;
        }
    }

    // TODO: replace with panic():
    if (!is_allocated) {
        printk(KERN_ERR "%s\n", "error to allocate new slab for cache");
        core::khalt();
    }
}

void cache_t::free_slab(slab_t *slab) noexcept
{
    slab->m_free = reinterpret_cast<uint8_t*>(slab->m_free) - m_objsize;

    if (slab->m_inuse > 0)
        slab->m_inuse--;

    // handle slab with all objects free
    if (slab->m_inuse == 0 && m_list.m_next_free != slab) {
        kstd::memset(slab->m_s_mem, 0, m_objnum << m_gfporder);

        if (slab->m_prev)
            slab->m_prev->m_next = slab->m_next;

        slab->m_next->m_prev = slab->m_prev;

        // append free slab into freelist
        if (m_freelist.m_head) {
            slab->m_prev                   = m_freelist.m_next_free;
            slab->m_next                   = nullptr;
            m_freelist.m_next_free->m_next = slab;
            m_freelist.m_next_free         = slab;
        }
        else {
            slab->m_prev           = nullptr;
            slab->m_next           = nullptr;
            m_freelist.m_head      = slab;
            m_freelist.m_next_free = slab;
        }
        m_freelist.m_size++;
        m_list.m_size--;
    }
}

void cache_t::free(void *objp) noexcept
{
    auto page_number = PHYS_PFN(phys_addr_t(objp));
    auto page_addr   = PFN_PHYS(page_number);
    bool is_free     = false;

    // traverse through the list to find a suitable slab
    // traversing from the end of the list because it is more likely that
    // slab object to be freed is one of the closest allocated ones
    slab_t *slab = m_list.m_next_free;

    for (size_t i = m_list.m_size; i > 0; i--) {
        if (phys_addr_t(slab->m_s_mem) == page_addr) {
            free_slab(slab);
            is_free = true;
            break;
        }

        slab = slab->m_prev;
    }

    // TODO: replace with panic():
    if (!is_free) {
        printk(KERN_ERR "%s\n", "error to free slab object");
        core::khalt();
    }
}

} // namespace kmem

/**
 * @brief Get the specific cache index in cache array.
 *
 * @param [in] size - given size of memory block to allocate.
 */
constexpr inline int32_t get_cache_index(size_t size) noexcept
{
    auto rounded = roundup_pow_of_two(size);
    auto index   = 0;

    while (rounded > 8) {
        rounded >>= 1;
        index++;
    }

    return index;
}

void *kmalloc(size_t size, gfp_t flags) noexcept
{
    // handle incorrect size
    if (size > 2_KB) {
        // TODO: replace with panic():
        printk(KERN_ERR "kmalloc: %s\n", "large size for allocation");
        return nullptr;
    }

    // TODO: edit after switching to user space
    if (!(flags & GFP::KERNEL))
        return nullptr;

    auto index = get_cache_index(size);

    // handle incorrect index
    if (index >= kmem::CACHES_SIZE) {
        // TODO: replace with panic():
        printk(KERN_ERR "kmalloc: %s\n", "large size for allocation");
        return nullptr;
    }

    return kmem::caches[index].alloc(flags);
}

} // namespace kernel