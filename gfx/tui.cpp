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

#include <kernel/drivers/keyboard.hpp>
#include <kernel/drivers/vesa.hpp>
#include <kernel/kstd/cstring.hpp>
#include <kernel/drivers/pit.hpp>
#include <kernel/gfx/font.hpp>
#include <kernel/terminal.hpp>
#include <kernel/gfx/tui.hpp>
#include <kernel/printk.hpp>
#include <kernel/panic.hpp>
#include <kernel/slab.hpp>


namespace kernel {
namespace gfx {
namespace tui {

using namespace driver;


void frame_t::update(void) noexcept
{
    // save previous terminal state
    prev_x_pos  = tty::terminal.x_pos;
    prev_y_pos  = tty::terminal.y_pos;
    prev_width  = tty::terminal.width;
    prev_height = tty::terminal.height;

    // update current terminal state
    tty::terminal.width       = width;
    tty::terminal.height      = height;
    tty::terminal.x_pos       = x_pos;
    tty::terminal.y_pos       = y_pos - 5;
    tty::terminal.begin_x_pos = tty::terminal.x_pos;
    tty::terminal.begin_y_pos = tty::terminal.y_pos;
}

void frame_t::reset(void) noexcept
{
    // return to previous terminal state
    tty::terminal.x_pos        = prev_x_pos;
    tty::terminal.y_pos        = prev_y_pos;
    tty::terminal.begin_x_pos  = 0;
    tty::terminal.begin_y_pos  = 0;
    tty::terminal.width        = prev_width;
    tty::terminal.height       = prev_height;
}

inline const u8 FRAME_PADDING {25};

void frame_t::init(point_t begin) noexcept
{
    this->begin = begin;
    fg          = color::black;
    bg          = color::white;
    begin_x_pos = begin.x + FRAME_PADDING;
    begin_y_pos = begin.y + FRAME_PADDING;
    x_pos       = begin.x + FRAME_PADDING;
    y_pos       = begin.y + FRAME_PADDING;
    width       = 400;
    height      = 300;
    prev_x_pos  = 0;
    prev_y_pos  = 0;
    prev_width  = 0;
    prev_height = 0;
    buttons     = nullptr;
    last        = buttons;
}

void window_t::init(const frame_t& fr, const char *title) noexcept
{
    frame = fr;

    if (!title)
        title = "window";

    kstd::strncpy(this->title, title, WINDOW_TITLE_SIZE);
    frame.update();
}

static void display_button(button_t *btn) noexcept
{
    tty::terminal.x_pos = btn->begin.x;
    tty::terminal.y_pos = btn->begin.y;

    btn->width = kstd::strlen(btn->label) * FONT_CHAR_WIDTH;

    fill_rectangle(btn->begin, btn->width, btn->height, btn->bg);
    cprintk(btn->fg, btn->bg, "%s", btn->label);
}

void window_t::show(void) noexcept
{
    // display window shadow
    auto shadow_margin = 10;
    auto shadow_begin  = (frame.begin + FRAME_PADDING - shadow_margin);
    fill_rectangle(shadow_begin, frame.width, frame.height, color::gray);

    // display window
    fill_rectangle(frame.begin, frame.width, frame.height, color::white);

    // display frame inside window
    auto fr_padding = 30;
    auto fr_width   = frame.width - fr_padding;
    auto fr_height  = frame.height - fr_padding;
    draw_rectangle(frame.begin + 15, fr_width, fr_height, color::gray);

    // display title
    tty::terminal.x_pos = frame.begin.x + 25;
    tty::terminal.y_pos = frame.begin.y + 10;
    cprintk(color::black, color::white, " %s ", title);

    // display top bar "buttons"
    auto shift = FRAME_PADDING * 4 + 9;
    tty::terminal.x_pos = (frame.begin_x_pos + frame.width) - shift;
    cprintk(color::black, color::white, "%s\n", " - = X ");

    // display window content
    tty::terminal.x_pos = frame.x_pos;
    tty::terminal.y_pos = frame.y_pos;

    if (!content[0])
        kstd::strncpy(this->content, "window content", WINDOW_CONTENT_SIZE);

    cprintk(color::black, color::white, "\n%s", content);

    button_t *current_button {nullptr};
    button_t *btn = frame.buttons;

    while (btn) {
        display_button(btn);
        btn = btn->next;
    }

    current_button    = frame.buttons;
    keyboard::KEY key = keyboard::getch();

    bool is_pressed = false;

    while (key != keyboard::KEY::ESC) {
        key = keyboard::getch();

        switch (key) {
            case keyboard::KEY::A:
                is_pressed = true;
                current_button->bg = color::gray;
                display_button(current_button);
                current_button     = current_button->prev;

                if (!current_button)
                    current_button = frame.buttons;

                current_button->bg = color::blue;
                display_button(current_button);
                break;

            case keyboard::KEY::D:
                is_pressed = true;
                current_button->bg = color::gray;
                current_button->bg = color::gray;
                display_button(current_button);
                current_button = current_button->next;

                if (!current_button)
                    current_button = frame.last;

                current_button->bg = color::blue;
                display_button(current_button);
                break;

            case keyboard::KEY::ENTER:
                if (!is_pressed)
                    break;

                current_button->bg = color::black;
                display_button(current_button);
                driver::pit::delay(80);

                if (current_button)
                    current_button->on_click(current_button->arg);
                return;

            default:
                break;
        }
    }
}

void window_t::add_button(const char *label, action_t on_click, void *arg, point_t begin) noexcept
{
    void *ptr = kmalloc(sizeof(button_t), GFP::KERNEL | GFP::ZERO);

    if (!ptr)
        panic("%s\n", "error to allocate memory for button");

    auto button = static_cast<button_t*>(ptr);

    button->label      = label;
    button->fg         = color::white;
    button->bg         = color::gray;
    button->on_click   = on_click;
    button->prev       = nullptr;
    button->next       = nullptr;
    button->width      = 35;
    button->height     = kstd::strlen(label);
    button->begin.x    = begin.x + frame.begin.x + FRAME_PADDING;
    button->begin.y    = begin.y + 30 + frame.begin.y;
    button->arg        = arg;

    // append button to buttons list
    if (!frame.buttons) {
        frame.buttons = button;
        frame.last    = button;
    }
    else {
        frame.last->next = button;
        button->prev     = frame.last;
        button->next     = nullptr;
        frame.last       = button;
    }
}

void window_t::add_content(const char *content) noexcept
{
    if (!content)
        content = "window content";

    kstd::strncpy(this->content, content, WINDOW_CONTENT_SIZE);
}

void window_t::destroy(void) noexcept
{
    frame.reset();

    // display window shadow
    auto shadow_margin = 10;
    auto shadow_begin  = (frame.begin + FRAME_PADDING - shadow_margin);
    fill_rectangle(shadow_begin, frame.width, frame.height, tty::terminal.bg);

    // display window
    fill_rectangle(frame.begin, frame.width, frame.height, tty::terminal.bg);

    // display frame inside window
    auto fr_padding = 30;
    auto fr_width   = frame.width - fr_padding;
    auto fr_height  = frame.height - fr_padding;
    draw_rectangle(frame.begin + 15, fr_width, fr_height, tty::terminal.bg);

    button_t *btn = frame.buttons;
    button_t *cur {nullptr};

    while (btn) {
        cur = btn;
        btn = btn->next;
        kfree(cur);
    }
}

} // namespace tui
} // namespace gfx
} // namespace kernel