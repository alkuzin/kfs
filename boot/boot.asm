# Monolithic Unix-like kernel from scratch.
# Copyright (C) 2024 Alexander (@alkuzin).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.


# magic number is a value that BIOS recognize as a kernel
.set MBOOT_MAGIC, 0x1BADB002

# flags can control various functions of the BIOS such as:
# enabling/disabling specific hardware components
# setting up boot options
# adjusting power management settings
.set MBOOT_PAGE_ALIGN, 1 << 0 # memory should be page-aligned to 4 KB
.set MBOOT_MEM_INFO,   1 << 1 # request memory information from the bootloader
.set MBOOT_USE_GFX,    1 << 2 # enable graphics mode

# indicates a bootable device
.set MBOOT_FLAGS, (MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_USE_GFX)

# the purpose of checksum is to ensure OS stability & security
# by detecting any unauthorised changes to the BIOS firmware
.set MBOOT_CHECKSUM, -(MBOOT_MAGIC + MBOOT_FLAGS)

.section .multiboot
.align 4                    # aligns next data element/instruction that is multiple of 4 bytes
    .long MBOOT_MAGIC       # declare double word (32-bit) of Magic number
    .long MBOOT_FLAGS       # declare double word (32-bit) of Magic number flags
    .long MBOOT_CHECKSUM    # declare double word (32-bit) of Magic number checksum
    .long 0, 0, 0, 0, 0

    .long 0                 # linear graphics mode
    .long 1024              # screen width
    .long 768               # screen height
    .long 32                # depth

.section .bss               # stores uninitialized global and static variables
.align 16                   # reserving space for the stack
stack_bottom:
    .skip 65536             # 64 KB
stack_top:

.section .text              # contains executable instructions of a program

.global boot
boot:
    mov $stack_top, %esp    # set stack pointer
    push %ebx               # multiboot info
    push %eax               # magic number
    xor %ebp, %ebp          # reset ebp
    .extern kmain           # from kernel/lernel.cpp
    call kmain              # call kernel entry point
    cli                     # clear interrupt flag (disables interrupts)

halt:
    hlt	                    # this instruction halts the CPU
    jmp halt                # infinite loop

.section .data              # stores initialized global and static variables
.align 0x1000