; Monolithic Unix-like kernel from scratch.
; Copyright (C) 2024 Alexander (@alkuzin).
; 
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <https://www.gnu.org/licenses/>.

global gdt_flush

gdt_flush:
    mov eax, [esp + 4]  ; get function argument from stack (GDT pointer from gdb::init())
    lgdt [eax]          ; load Global Descriptor Table

                        ; reload data segment registers:
    
    mov eax, 0x10       ; kernel data segment selector
    mov ds, ax          ; allow the CPU to access to kernel data segment
    mov es, ax          ; allow the CPU to access to kernel extra segment
    mov fs, ax          ; allow the CPU to access to kernel additional segment
    mov gs, ax          ; allow the CPU to access to kernel global segment
    mov ss, ax          ; allow the CPU to access to kernel stack segment

    jmp 0x08: flush     ; return to kernel code segment

flush:
    ret                 ; return back to C++ code