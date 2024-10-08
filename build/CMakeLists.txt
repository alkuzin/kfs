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


cmake_minimum_required(VERSION 3.20.0)

# Set project name & programming languages
project(NOS LANGUAGES CXX ASM_NASM)

# Check if the compiler is g++
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "This project requires g++ as the C++ compiler!")
endif()

set(CMAKE_CXX_STANDARD 23)          # Using C++23 standard
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Enforce the standard
set(CMAKE_CXX_EXTENSIONS OFF)       # Disable compiler-specific extensions

# Set paths
set(BOOT_DIR                ${CMAKE_SOURCE_DIR}/../boot)
set(KERNEL_DIR              ${CMAKE_SOURCE_DIR}/../kernel)
set(KERNEL_ARCH_X86_DIR     ${KERNEL_DIR}/arch/x86)
set(KERNEL_SHELL_DIR        ${KERNEL_DIR}/shell)
set(KERNEL_DEBUG_DIR        ${KERNEL_DIR}/debug)
set(KERNEL_MM_DIR           ${KERNEL_DIR}/mm)
set(KERNEL_COMMON_DIR       ${KERNEL_DIR}/common)
set(DRIVERS_DIR             ${CMAKE_SOURCE_DIR}/../drivers)
set(GFX_DIR                 ${CMAKE_SOURCE_DIR}/../gfx)
set(KSTD_DIR                ${CMAKE_SOURCE_DIR}/../kstd)
set(INCLUDE_DIR             ${CMAKE_SOURCE_DIR}/../include)

set(INCLUDE_DIRECTORIES  ${INCLUDE_DIR})

# List of kernel drivers source files
set(GFX_SOURCES "${GFX_DIR}/font.cpp")

# List of kernel drivers source files
set(DRIVERS_SOURCES
    "${DRIVERS_DIR}/vesa.cpp"
    "${DRIVERS_DIR}/keyboard.cpp"
)

# List of kernel standard library source files
set(KSTD_SOURCES
    "${KSTD_DIR}/cstring.cpp"
    "${KSTD_DIR}/vsnprintk.cpp"
)

# List of kernel core source files
set(KERNEL_SOURCES
    "${KERNEL_DIR}/kernel.cpp"

    # Kernel commond directory:
    "${KERNEL_COMMON_DIR}/terminal.cpp"
    "${KERNEL_COMMON_DIR}/printk.cpp"

    # Kernel arch/x86 directory:
    "${KERNEL_ARCH_X86_DIR}/gdt.cpp"

    # Kernel shell directory:
    "${KERNEL_SHELL_DIR}/shell.cpp"

    # Kernel debug directory:
    "${KERNEL_DEBUG_DIR}/kdump.cpp"

    # Kernel memory management directory:
    "${KERNEL_MM_DIR}/pmm.cpp"
    "${KERNEL_MM_DIR}/slab.cpp"
)

# List of kernel assembly source files
set(ASM_SOURCES
    "${BOOT_DIR}/boot.asm"
    "${KERNEL_ARCH_X86_DIR}/gdt_flush.asm"
)

# List of all source files
set(SOURCES
    ${KERNEL_SOURCES}
    ${DRIVERS_SOURCES}
    ${GFX_SOURCES}
    ${KSTD_SOURCES}
)

set(CXXFLAGS
    "-Wall"                 # Enable all compiler's warning messages
    "-Wextra"               # Enable additional warning messages not covered by -Wall
    "-Werror"               # Treat all warnings as errors
    "-g"                    # Generate debug information
    "-std=c++23"            # Use the C++23 standard
    "-m32"                  # Generate 32-bit code
    "-O2"                   # Optimize the code for performance (level 2 optimization)
    "-nostdlib"             # Do not link against the standard library
    "-nodefaultlibs"        # Do not use the default libraries
    "-fno-pie"              # Disable Position Independent Executable (PIE) generation (security feature)
    "-fno-exceptions"       # Disable support for C++ exceptions
    "-fno-rtti"             # Disable Run-Time Type Information (RTTI) support
    "-fno-stack-protector"  # Disable stack protection (security feature)
    "-fno-builtin"          # Disable built-in functions
    "-ffreestanding"        # Indicate that the code does not rely on any standard library features
)

set(LDFLAGS
    "-z" "noexecstack"      # Preventing execution of code on the stack (security feature)
    "-m" "elf_i386"         # Specify the output format as ELF for 32-bit x86 architecture
)

set(ASMFLAGS
    "-f elf"                # Generate output in the ELF format.
)

# List to hold the object files generated from assembly
set(ASM_OBJECTS)

# Custom command to assemble each assembly file using NASM
foreach(ASM_FILE ${ASM_SOURCES})
    get_filename_component(ASM_NAME ${ASM_FILE} NAME_WE)     # Base name without extension
    set(OBJ_FILE ${CMAKE_CURRENT_BINARY_DIR}/${ASM_NAME}.o)  # Output object file
    list(APPEND ASM_OBJECTS ${OBJ_FILE})

    add_custom_command(
        OUTPUT ${OBJ_FILE}
        COMMAND nasm ${ASMFLAGS} -o ${OBJ_FILE} ${ASM_FILE}
        DEPENDS ${ASM_FILE}
    )
endforeach()

# Single object file from the C++ source files and the assembly files
add_library(kernel_objects OBJECT ${SOURCES} ${ASM_OBJECTS})

# Compiler and linker flags
target_compile_options(kernel_objects PRIVATE ${CXXFLAGS})

# Include directories
target_include_directories(kernel_objects PRIVATE ${INCLUDE_DIRECTORIES})

# Executable that uses the object file
add_executable(kernel ${ASM_OBJECTS} $<TARGET_OBJECTS:kernel_objects>)

set_source_files_properties(
  ${ASM_OBJECTS}
  properties
  external_object true
  generated true
)

# Link the additional object files
target_link_libraries(kernel PRIVATE ${ASM_OBJECTS})

# Set the linker to ld & link with custom linker script
set_target_properties(${NAME} PROPERTIES LINK_FLAGS "${LDFLAGS} -T ${LINKER_SCRIPT}")
set(CMAKE_CXX_LINK_EXECUTABLE "ld <LINK_FLAGS> -T${BOOT_DIR}/linker.ld -o <TARGET> <OBJECTS> ")
target_link_options(kernel PRIVATE ${LDFLAGS})

# Set project variables
set(NAME "kernel")
set(ISO_DIR "${CMAKE_BINARY_DIR}/iso")
set(NAME_ISO "${CMAKE_BINARY_DIR}/kfs.iso")

# Custom command to build the ISO
add_custom_target(build-iso ALL DEPENDS ${NAME}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ISO_DIR}/boot/grub
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${NAME}> ${ISO_DIR}/boot/kernel.elf
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/../boot/grub.cfg ${ISO_DIR}/boot/grub/grub.cfg
    COMMAND grub-mkrescue -o ${NAME_ISO} ${ISO_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Building ISO image"
)

# Custom command to initialize the ISO with QEMU
add_custom_target(init-iso
    COMMAND qemu-system-i386 -m 2024 -cdrom ${NAME_ISO}
    COMMENT "Initializing ISO with QEMU"
)

# Custom command to initialize the build
add_custom_target(init DEPENDS init-iso)

# Custom command for debugging
add_custom_target(debug
    COMMAND qemu-system-i386 -s -S -m 2024 -cdrom ${NAME_ISO} & gdb ${CMAKE_BINARY_DIR}/${NAME} -ex "target remote localhost:1234" -tui
    COMMENT "Debugging with GDB"
)