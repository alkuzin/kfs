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


; magic number is a value that BIOS recognize as a kernel
MBOOT_MAGIC      equ 0x1BADB002

; flags can control various functions of the BIOS such as:
; enabling/disabling specific hardware components
; setting up boot options
; adjusting power management settings
MBOOT_PAGE_ALIGN equ 1 << 0 ; memory should be page-aligned to 4 KB
MBOOT_MEM_INFO   equ 1 << 1 ; request memory information from the bootloader
MBOOT_USE_GFX    equ 1 << 2 ; enable graphics mode

; indicates a bootable device
MBOOT_FLAGS      equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_USE_GFX

; the purpose of checksum is to ensure OS stability & security
; by detecting any unauthorised changes to the BIOS firmware
MBOOT_CHECKSUM   equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4                 ; aligns next data element/instruction that is multiple of 4 bytes
    dd MBOOT_MAGIC      ; declare double word (32-bit) of Magic number
    dd MBOOT_FLAGS      ; declare double word (32-bit) of Magic number flags
    dd MBOOT_CHECKSUM   ; declare double word (32-bit) of Magic number checksum
    dd 0, 0, 0, 0, 0

    dd 0                ; linear graphics mode
    dd 1024             ; screen width
    dd 768              ; screen height
    dd 32               ; depth

section .bss            ; stores uninitialized global and static variables
align 16                ; reserving space for the stack
stack_bottom:
    RESB 16384 * 8
stack_top:

section .text           ; contains executable instructions of a program

global boot
boot:
    mov esp, stack_top  ; set stack pointer
    push ebx            ; multiboot info
    push eax            ; magic number
    xor ebp, ebp        ; reset ebp
    extern kmain        ; from kernel/lernel.cpp
    call kmain          ; call kernel entry point
    cli                 ; clear interrupt flag (disables interrupts)

halt:
    hlt	                ; this instruction halts the CPU 
    jmp halt            ; infinite loop


section .data           ; stores initialized global and static variables
align 4096