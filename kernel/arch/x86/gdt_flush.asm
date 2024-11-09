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

.global gdt_flush

gdt_flush:
    mov  4(%esp), %eax  # get function argument from stack (GDT pointer from gdb::init())
    lgdt (%eax)         # load Global Descriptor Table

                        # reload data segment registers:

    mov $0x10, %eax     # kernel data segment selector
    mov %ax, %ds        # allow the CPU to access to kernel data segment
    mov %ax, %es        # allow the CPU to access to kernel extra segment
    mov %ax, %fs        # allow the CPU to access to kernel additional segment
    mov %ax, %gs        # allow the CPU to access to kernel global segment
    mov $0x18, %ax      # offset in the GDT to kernel stack
    mov %ax, %ss        # allow the CPU to access to kernel stack segment

    jmp $0x08, $flush   # return to kernel code segment

flush:
    ret                 # return back to C++ code