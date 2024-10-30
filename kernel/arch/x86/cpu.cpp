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

#include <kernel/arch/x86/register.hpp>
#include <kernel/arch/x86/cpu.hpp>
#include <kernel/kstd/cstring.hpp>
#include <kernel/printk.hpp>


namespace kernel {
namespace arch {
namespace x86 {
namespace cpu {

static info_t cpu_info {};

/**
 * @brief Get highest function parameter and manufacturer ID.
 *
 * @param [out] details - given CPU detail info struct to fill.
 */
static void handle_leaf0(details_t& details) noexcept
{
    cpuid(0, cpu_info);

    // set vendor ID
    *(reinterpret_cast<uint32_t*>(&details.vendor[0])) = cpu_info.ebx;
    *(reinterpret_cast<uint32_t*>(&details.vendor[4])) = cpu_info.edx;
    *(reinterpret_cast<uint32_t*>(&details.vendor[8])) = cpu_info.ecx;

    details.high_param = cpu_info.eax;
}

/**
 * @brief Get processor info and feature bits.
 *
 * @param [out] details - given CPU detail info struct to fill.
 */
static void handle_leaf1(details_t& details) noexcept
{
    cpuid(1, cpu_info);

    auto stepping    = (cpu_info.eax & 0x0F);
    auto model       = ((cpu_info.eax >> 4) & 0x0F);
    auto family      = ((cpu_info.eax >> 8) & 0x0F);
    auto type        = ((cpu_info.eax >> 12) & 0x03);
    auto ext_model   = ((cpu_info.eax >> 16) & 0x0F);
    auto ext_family  = ((cpu_info.eax >> 20) & 0xFF);

    details.stepping = stepping;
    details.type     = type;

    // set actual processor model
    if (family == 6 || family == 15)
        details.model = (ext_model << 4) + model;
    else
        details.model = model;

    // set actual processor family
    details.family = family;

    if (family == 15)
        details.family += ext_family;
}

/**
 * @brief Get processor model name.
 *
 * @param [out] details - given CPU detail info struct to fill.
 */
static void set_brand(details_t& details) noexcept
{
    // get first part of model name
    cpuid(0x80000002, cpu_info);
    kstd::memcpy(&details.brand, &cpu_info, sizeof(cpu_info));

    // get second part of model name
    cpuid(0x80000003, cpu_info);
    kstd::memcpy(&details.brand + 16, &cpu_info, sizeof(cpu_info));

    // get third part of model name
    cpuid(0x80000004, cpu_info);
    kstd::memcpy(&details.brand + 32, &cpu_info, sizeof(cpu_info));
}

void get_details(details_t& details) noexcept
{
    handle_leaf0(details);
    handle_leaf1(details);
    set_brand(details);
}

const char *get_op_modes(void) noexcept
{
    // check for 64-bit support
    cpuid(0x80000001, cpu_info);

    if (cpu_info.edx & (1 << 29))
        return "32-bit, 64-bit";
    else
        return "32-bit";
}

const char *type_to_str(TYPE type) noexcept
{
    switch (type) {
    case TYPE::OEM:
        return "Original Equipment Manufacturer (OEM) Processor ";

    case TYPE::IOP:
        return "Intel Overdrive Processor";

    case TYPE::DUAL:
        return "Dual processor (Intel P5 processor)";

    case TYPE::RESV:
        return "<reserved>";

    default:
        return "unknown processor";
    }
}

} // namespace cpu
} // namespace x86
} // namespace arch
} // namespace kernel