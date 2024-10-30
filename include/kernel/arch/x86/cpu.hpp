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
 * @file  cpu.hpp
 * @brief Contains CPU information declarations.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   29.10.2024
 */

#ifndef _KERNEL_ARCH_X86_CPU_HPP_
#define _KERNEL_ARCH_X86_CPU_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace arch {
namespace x86 {
namespace cpu {

// architecture
#if __i386__
inline const char *arch {"x86_32"};
#elif __x86_64__
inline const char *arch {"x86_64"};
#endif

// byte order
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
inline const char *byte_order {"Big Endian"};
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
inline const char *byte_order {"Little Endian"};
#endif

struct info_t {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
};

enum class TYPE : uint8_t {
    OEM  = 0x0,  // Original Equipment Manufacturer processor
    IOP  = 0x1,  // Intel Overdrive Processor
    DUAL = 0x2,  // dual processor (Intel P5 Pentium processors only)
    RESV = 0x3   // reserved value
};

inline const uint8_t VENDOR_ID_SIZE {13};
inline const uint8_t BRAND_SIZE     {49};

struct details_t {
    char vendor[VENDOR_ID_SIZE];
    char brand[BRAND_SIZE];
    uint32_t high_param;    // highest function parameter (max leaf number)
    uint8_t stepping;       // product revision number
    uint8_t model;          // process model
    uint8_t family;         // process family
    uint8_t type;           // process type
};

/**
 * @brief Set CPU info registers.
 *
 * @param [in] leaf - given category of CPU information to gather.
 * @param [out] info - given CPU registers struct.
 */
inline void cpuid(uint32_t leaf, info_t& info) noexcept
{
    __asm__ __volatile__ (
        "cpuid"
        : "=a"(info.eax), "=b"(info.ebx), "=c"(info.ecx), "=d"(info.edx)
        : "a"(leaf)
    );
}

/**
 * @brief Get the detailed info about CPU.
 *
 * @param [out] details - given CPU detail info struct to fill.
 */
void get_details(details_t& details) noexcept;

/**
 * @brief Get CPU type string representation.
 *
 * @param [in] type - given CPU type.
 * @return CPU type string representation.
 */
const char *type_to_str(TYPE type) noexcept;

/**
 * @brief Get the CPU operating mode(s).
 *
 * @return string representation of CPU operating mode(s).
 */
const char *get_op_modes(void) noexcept;

} // namespace cpu
} // namespace x86
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_X86_CPU_HPP_