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

#include <kernel/arch/i686/gdt.hpp>
#include <kernel/linkage.hpp>


namespace kernel {
namespace arch {
namespace i686 {
namespace gdt {

inline const u32 ENTRY_BASE           {0x00000000};
inline const u32 ENTRY_LIMIT          {0xFFFFFFFF};
inline const u8  ENTRY_FLAGS          {0xCF}; // 32-bit protected mode segment

// Access byte:
inline const u8 KERNEL_ACCESS_CODE    {0x9A};
inline const u8 KERNEL_ACCESS_DATA    {0x92};
inline const u8 KERNEL_ACCESS_STACK   {0x97};
inline const u8 USER_ACCESS_CODE      {0xFA};
inline const u8 USER_ACCESS_DATA      {0xF2};
inline const u8 USER_ACCESS_STACK     {0xF7};

inline const auto ENTRIES {7};

entry_t GDT[ENTRIES];
ptr_t   *gdt_ptr = reinterpret_cast<ptr_t*>(GDT_BASE);

/**
 * @brief Set the GDT entry.
 *
 * @param [in] eno - given GDT entry number.
 * @param [in] base - given linear address where the segment begins.
 * @param [in] limit - given maximum addressable unit.
 * @param [in] access - given segment access byte.
 * @param [in] flags - given segment flags.
 */
constexpr void set_entry(u32 eno, u32 base, u32 limit, u8 access, u8 flags) noexcept
{
    GDT[eno].base_low  = (base & 0xFFFF);           // get first 2 bytes
    GDT[eno].base_mid  = ((base >> 0x10) & 0xFF);   // get third byte
    GDT[eno].base_high = static_cast<u8>((base >> 0x18) & 0xFF);   // get last byte
    GDT[eno].limit     = (limit & 0xFFFF);
    GDT[eno].flags     = ((limit >> 0x10) & 0x0F);
    GDT[eno].flags     |= (flags & 0xF0);
    GDT[eno].access    = access;
}

/**
 * @brief Flush out the old GDT and install the new changes.
 *
 * @param [in] ptr - new GDT pointer to update.
 *
 */
asmlinkage void gdt_flush(u32 ptr);

void init(void) noexcept
{
    // set GDT entries:
    set_entry(0, 0, 0, 0, 0); // (null descriptor) should always contain no data

    // kernel space segments:
    set_entry(1, ENTRY_BASE, ENTRY_LIMIT, KERNEL_ACCESS_CODE, ENTRY_FLAGS);
    set_entry(2, ENTRY_BASE, ENTRY_LIMIT, KERNEL_ACCESS_DATA, ENTRY_FLAGS);
    set_entry(3, ENTRY_BASE, ENTRY_LIMIT, KERNEL_ACCESS_STACK, ENTRY_FLAGS);

    // user space segments:
    set_entry(4, ENTRY_BASE, ENTRY_LIMIT, USER_ACCESS_CODE, ENTRY_FLAGS);
    set_entry(5, ENTRY_BASE, ENTRY_LIMIT, USER_ACCESS_DATA, ENTRY_FLAGS);
    set_entry(6, ENTRY_BASE, ENTRY_LIMIT, USER_ACCESS_STACK, ENTRY_FLAGS);

    // set GDT pointer:
    gdt_ptr->size   = sizeof(GDT) - 1;
    gdt_ptr->offset = reinterpret_cast<u32>(&GDT);

    // update GDT:
    gdt_flush(reinterpret_cast<u32>(gdt_ptr));
}

} // namespace gdt
} // namespace i686
} // namespace arch
} // namespace kernel