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
 * @file  keyboard.hpp
 * @brief Contains PS/2 keyboard driver declaration.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   26.09.2024
 */

#ifndef _KERNEL_DRIVER_KEYBOARD_HPP_
#define _KERNEL_DRIVER_KEYBOARD_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace driver {
namespace keyboard {

/** @brief Keyboard special keys enumeration.*/
enum class KEY {
    ESC         = 0X01,
    BACKSPACE   = 0X0E,
    TAB         = 0X0F,
    ENTER       = 0X1C,
    LCTRL       = 0X1D,
    LSHIFT      = 0X2A,
    BACKSLASH   = 0X2B,
    LALT        = 0X38,
    SPACE       = 0X39,
    CAPS_LOCK   = 0X3A,
    LEFT_ARROW  = 0X4B,
    RIGHT_ARROW = 0X4D,
    UP_ARROW    = 0X48,
    DOWN_ARROW  = 0X50
};

/** @brief Initialize keyboard.*/
void init(void) noexcept;

/**
 * @brief Keyboard get character on key press.
 *
 * @return Character read from the keyboard.
 */
uint8_t getchar(void) noexcept;

/**
 * @brief Get the line from user.
 *
 * @param [out] buffer - given buffer to store input.
 * @param [out] size - given size of buffer.
 */
void get_line(char *buffer, size_t size) noexcept;

} // namespace keyboard
} // namespace driver
} // namespace kernel

#endif // _KERNEL_DRIVER_KEYBOARD_HPP_