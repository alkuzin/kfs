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
 * @file  tui.hpp
 * @brief Contains Terminal User Interface (TUI) declarations.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   31.10.2024
 */

#ifndef _KERNEL_GFX_TUI_HPP_
#define _KERNEL_GFX_TUI_HPP_

#include <kernel/gfx/graphics.hpp>


namespace kernel {
namespace gfx {
namespace tui {

using action_t = void(*)(void);

struct button_t {
    const char *label;
    rgb_t    fg;        // foreground color
    rgb_t    bg;        // background color
    action_t on_click;  // function to execute on button click
    point_t  begin;       // begin point of button
    int32_t  width;
    int32_t  height;
    button_t *prev;     // previous button in list
    button_t *next;     // next button in list
};

struct frame_t {
    point_t begin;          // begin point of frame
    rgb_t   fg;             // foreground color
    rgb_t   bg;             // background color
    int32_t begin_x_pos;    // x-position of the cursor
    int32_t begin_y_pos;    // y-position of the cursor
    int32_t x_pos;          // x-position of the cursor
    int32_t y_pos;          // y-position of the cursor
    int32_t width;          // output width
    int32_t height;         // output height
    int32_t prev_x_pos;     // previous x-position of the cursor
    int32_t prev_y_pos;     // previous y-position of the cursor
    int32_t prev_width;     // previous output width
    int32_t prev_height;    // previous output height
    button_t *buttons;      // list of buttons connected to the frame
    button_t *last;         // last button in buttons list

    /**
     * @brief Initialize frame.
     *
     * @param [in] begin - given frame begin point.
     */
    void init(point_t begin = {300, 200}) noexcept;

    /** @brief Update terminal state.*/
    void update(void) noexcept;

    /** @brief Reset terminal state.*/
    void reset(void) noexcept;
};

inline const uint8_t WINDOW_TITLE_SIZE {32};

struct window_t {
    char    title[WINDOW_TITLE_SIZE];
    frame_t frame;

    /**
     * @brief Initialize TUI window.
     *
     * @param [in] fr - given frame to connect to the window.
     * @param [in] title - given window title.
     */
    void init(const frame_t& fr, const char *title = "window") noexcept;

    /** @brief Display window on the screen.*/
    void show(void) noexcept;

    /**
     * @brief Add button to the window frame.
     *
     * @param [in] label - given button label.
     * @param [in] on_click - given function to execute on button click.
     * @param [in] begin - given button begin point.
     */
    void add_button(const char *label, action_t on_click, point_t begin = {0, 0}) noexcept;
};

/** @brief Initialize Terminal User Interface (TUI).*/
void init(void) noexcept;

} // namespace tui
} // namespace gfx
} // namespace kernel

#endif // _KERNEL_GFX_TUI_HPP_