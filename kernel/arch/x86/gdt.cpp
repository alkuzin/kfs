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

#include <kernel/arch/x86/gdt.hpp>


namespace kernel {
namespace arch {
namespace x86 {
namespace gdt {

constexpr uint32_t ENTRY_BASE           {0x00000000};
constexpr uint32_t ENTRY_LIMIT          {0xFFFFFFFF};   // 4 KB
constexpr uint8_t  ENTRY_FLAGS          {0xCF};         // 32-bit protected mode segment

// Access byte:
constexpr uint8_t KERNEL_ACCESS_CODE    {0x9A};
constexpr uint8_t KERNEL_ACCESS_DATA    {0x92};
constexpr uint8_t KERNEL_ACCESS_STACK   {0x97};
constexpr uint8_t USER_ACCESS_CODE      {0xFA};
constexpr uint8_t USER_ACCESS_DATA      {0xF2};
constexpr uint8_t USER_ACCESS_STACK     {0xF7};

constexpr uint32_t ENTRIES {7};
entry_t GDT[ENTRIES];
ptr_t   gdt_ptr;

/**
 * @brief Set the GDT entry.
 * 
 * @param [in] eno - given GDT entry number.
 * @param [in] base - given linear address where the segment begins.
 * @param [in] limit - given maximum addressable unit.
 * @param [in] access - given segment access byte.
 * @param [in] flags - given segment flags.
 */
constexpr void set_entry(uint32_t eno, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) noexcept
{
    GDT[eno].m_base_low  = (base & 0xFFFF);           // get first 2 bytes
    GDT[eno].m_base_mid  = ((base >> 0x10) & 0xFF);   // get third byte
    GDT[eno].m_base_high = ((base >> 0x18) & 0xFF);   // get last byte
    GDT[eno].m_limit     = (limit & 0xFFFF);
    GDT[eno].m_flags     = (((flags << 0x4) & 0xF0) | ((limit >> 0x10) & 0x0F));
    GDT[eno].m_access    = access;
}

/**
 * @brief Flush out the old GDT and install the new changes.
 * 
 * @param [in] ptr - new GDT pointer to update.
 * 
 */
extern "C" void gdt_flush(uint32_t ptr);

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
    gdt_ptr.m_size   = sizeof(GDT) - 1;
    gdt_ptr.m_offset = reinterpret_cast<uint32_t>(&GDT);

    // update GDT:
    gdt_flush(reinterpret_cast<uint32_t>(&gdt_ptr));
}
    
} // namespace gdt    
} // namespace x86
} // namespace arch
} // namespace kernel