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
#include <kernel/mm_types.hpp>
#include <kernel/kernel.hpp>
#include <kernel/panic.hpp>
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
    void *pages = alloc_pages(GFP::KERNEL, SLAB_PAGES_ORDER)->addr();

    if (!pages)
        panic("%s\n", "error to allocate pages for slabs structs");

    slabs.head = reinterpret_cast<slab_t*>(pages);
    slabs.size = ((1 << SLAB_PAGES_ORDER) << PAGE_SHIFT) / sizeof(slab_t);

    // allocating pages for each slab objects
    void *page_ptr = nullptr;
    page_t *page   = nullptr;

    for (size_t i = 0; i < slabs.size; i++) {
        page     = get_zeroed_page(GFP::KERNEL);
        page_ptr = page->addr();

        if (!page_ptr)
            panic("%s\n", "error to allocate pages for objects");

        // set page as used in slab
        page->flags |= PG::SLAB;

        slabs.head[i].s_mem   = page_ptr;
        slabs.head[i].free    = page_ptr;
        slabs.head[i].next    = nullptr;
        slabs.head[i].prev    = nullptr;
        slabs.head[i].is_free = true;
        slabs.head[i].inuse   = 0;
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

void cache_t::create(const char *name, size_t size, u8 flags) noexcept
{
    list          = {nullptr, nullptr, 0};
    freelist      = {nullptr, nullptr, 0};
    objsize       = static_cast<u32>(roundup_pow_of_two(size));
    gfporder      = static_cast<u32>(kstd::ceil(kstd::log2(objsize)));
    objnum        = PAGE_SIZE >> gfporder;
    flags         = flags;
    kstd::strncpy(this->name, name, CACHE_NAMELEN);
}

void *cache_t::alloc(uint8_t flags) noexcept
{
    (void)flags;    // TODO: handle SLAB_KERNEL

    if (list.size == 0)
        alloc_slab();

    slab_t *slab = list.next_free;
    void *ptr    = slab->free;

    // handle case if all objects in the last slab are used
    if (slab->inuse == objnum) {
        alloc_slab();
        slab = list.next_free;
        ptr  = slab->free;
    }

    // update slab info
    slab->inuse++;
    slab->free = reinterpret_cast<uint8_t*>(slab->free) + objsize;

    return ptr;
}

void cache_t::alloc_slab(void) noexcept
{
    bool is_allocated = false;
    slab_t *slab;

    // first looking into the freelist for free slabs
    if (freelist.head) {
        list.next_free->next            = freelist.next_free;
        freelist.next_free->next        = nullptr;
        freelist.next_free->prev->next  = nullptr;

        if (freelist.size)
            freelist.next_free = freelist.next_free->prev;

        list.next_free->next->prev = list.next_free;
        list.next_free             = list.next_free->next;

        list.size++;
        freelist.size--;
        is_allocated = true;
    }
    else {
        // if there is no free slabs in freelist - then search them
        // in the external slabs array (m_slabs)
        while (slab_pos < slabs.size) {
            slab = &slabs.head[slab_pos];

            if (slab->is_free) {
                slab->is_free = false;

                if (list.head) {
                    slab->prev           = list.next_free;
                    slab->next           = nullptr;
                    list.next_free->next = slab;
                    list.next_free       = slab;
                }
                else {
                    list.head      = slab;
                    list.next_free = slab;
                }

                page_t *page = get_page(phys_addr_t(slab->s_mem));
                page->cache  = this;
                page->slab   = slab;

                list.size++;
                is_allocated = true;
                break;
            }

            slab_pos++;
        }
    }

    if (!is_allocated)
        panic("%s\n", "error to allocate new slab for cache");
}

void cache_t::free_slab(slab_t *slab) noexcept
{
    slab->free = reinterpret_cast<uint8_t*>(slab->free) - objsize;

    if (slab->inuse > 0)
        slab->inuse--;

    // handle slab with all objects free
    if (slab->inuse == 0 && list.next_free != slab) {
        kstd::memset(slab->s_mem, 0, objnum << gfporder);

        if (slab->prev)
            slab->prev->next = slab->next;

        slab->next->prev = slab->prev;

        // append free slab into freelist
        if (freelist.head) {
            slab->prev               = freelist.next_free;
            slab->next               = nullptr;
            freelist.next_free->next = slab;
            freelist.next_free       = slab;
        }
        else {
            slab->prev         = nullptr;
            slab->next         = nullptr;
            freelist.head      = slab;
            freelist.next_free = slab;
        }
        freelist.size++;
        list.size--;
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
    slab_t *slab = list.next_free;

    for (size_t i = list.size; i > 0; i--) {
        if (phys_addr_t(slab->s_mem) == page_addr) {
            free_slab(slab);
            is_free = true;
            break;
        }

        slab = slab->prev;
    }

    if (!is_free)
        panic("%s\n", "error to free slab object");
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
        panic(PANIC_ERR "kmalloc: %s\n", "large size for allocation");
        return nullptr;
    }

    // TODO: edit after switching to user space
    if (!(flags & GFP::KERNEL))
        return nullptr;

    auto index = get_cache_index(size);

    // handle incorrect index
    if (index >= kmem::CACHES_SIZE) {
        panic(PANIC_ERR "kmalloc: %s\n", "large size for allocation");
        return nullptr;
    }

    return kmem::caches[index].alloc(flags);
}

void kfree(const void *objp) noexcept
{
    // handle nullptr
    if (!objp)
        return;

    page_t *page = get_page(phys_addr_t(objp));
    page->cache->free_slab(page->slab);
}

size_t ksize(const void *objp) noexcept
{
    // handle nullptr
    if (!objp)
        return 0;

    page_t *page = get_page(phys_addr_t(objp));
    return page->cache->objsize;
}

} // namespace kernel