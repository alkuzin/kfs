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

/**
 * @file  slab.hpp
 * @brief Contain SLAB allocator declaration.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   06.10.2024
 */

#ifndef _KERNEL_SLAB_HPP_
#define _KERNEL_SLAB_HPP_

#include <kernel/types.hpp>
#include <kernel/gfp.hpp>


namespace kernel {
namespace kmem {

// TODO: GET_PAGE_CACHE() and GET_PAGE_SLAB()
inline const uint32_t CACHE_NAMELEN {16};

struct slab_t
{
    slab_t   *m_next;    // next slab
    slab_t   *m_prev;    // previous slab
    void     *m_s_mem;   // starting address of the first object
    void     *m_free;    // starting address of the first free object
    uint32_t  m_inuse;   // number of active objects in the slab
    bool      m_is_free; // checks if this slab is not used by cache
};

struct slab_list_t
{
    slab_t *m_next_free; // starting address of the next free slab
    slab_t *m_head;      // list head pointer
    size_t  m_size;      // number of elements
};

struct cache_t
{
    slab_list_t m_list;                // list of partial and full slabs
    slab_list_t m_freelist;            // list of free slabs
    uint32_t    m_gfporder;            // size of slab in pages (2^gfporder)
    uint32_t    m_objsize;             // object size
    uint32_t    m_objnum;              // number of objects in each slab
    uint8_t     m_flags;               // cache flags
    char        m_name[CACHE_NAMELEN]; // cache name

private:
    /** @brief Allocate a single slab.*/
    void alloc_slab(void) noexcept;

    /**
     * @brief Free a single slab.
     *
     * @param [in] slab - given slab to free.
     */
    void free_slab(slab_t *slab) noexcept;

public:
    /**
     * @brief Create a cache.
     *
     * @param [in] name - given cache name.
     * @param [in] size - given size of cache objects.
     * @param [in] flags - given allocation flags.
     */
    void create(const char *name, size_t size, uint32_t flags) noexcept;

    /**
     * @brief Allocate a single object from the cache.
     *
     * @param [in] flags - given allocation flags.
     * @return allocated object pointer.
     */
    void *alloc(uint8_t flags) noexcept;

    /**
     * @brief Free cache object.
     *
     * @param [in] objp - given object to free.
     */
    void free(void *objp) noexcept;
};

/** @brief Initialize SLAB allocator.*/
void init(void) noexcept;

} // namespace kmem

/**
 * @brief Allocate memory in the kernel.
 *
 * @param [in] size - given size of memory block to allocate.
 * @param [in] flags - given allocation flags.
 * @return pointer to the allocated memory in case of success.
 * @return nullptr in case of failure.
 */
void *kmalloc(size_t size, gfp_t flags) noexcept;

} // namespace kernel

#endif // _KERNEL_SLAB_HPP_