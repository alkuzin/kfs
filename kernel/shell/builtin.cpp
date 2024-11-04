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
#include <kernel/shell/builtin.hpp>
#include <kernel/arch/x86/gdt.hpp>
#include <kernel/arch/x86/cpu.hpp>
#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cstdlib.hpp>
#include <kernel/drivers/pit.hpp>
#include <kernel/arch/x86/io.hpp>
#include <kernel/kstd/cmath.hpp>
#include <kernel/gfx/tui.hpp>
#include <kernel/config.hpp>
#include <kernel/debug.hpp>
#include <kernel/ktime.hpp>
#include <kernel/pmm.hpp>


namespace kernel {
namespace shell {

inline const uint8_t BUILTINS_COUNT {12};

static void help(int32_t argc, char **argv) noexcept;
static void clear(int32_t argc, char **argv) noexcept;
static void uname(int32_t argc, char **argv) noexcept;
static void date(int32_t argc, char **argv) noexcept;
static void lscpu(int32_t argc, char **argv) noexcept;
static void lsmem(int32_t argc, char **argv) noexcept;
static void gdt(int32_t argc, char **argv) noexcept;
static void ticks(int32_t argc, char **argv) noexcept;
static void reboot(int32_t argc, char **argv) noexcept;
static void shutdown(int32_t argc, char **argv) noexcept;
static void tui(int32_t argc, char **argv) noexcept;
static void interrupt(int32_t argc, char **argv) noexcept;

static builtin_t builtins[BUILTINS_COUNT] {
    {"help",  "show list of available commands", nullptr, 0, help},
    {"clear", "clear screen", nullptr, 0, clear},
    {"uname", "print system information", nullptr, 0, uname},
    {"date",  "print current date", nullptr, 0, date},
    {"lscpu", "display information about the CPU", nullptr, 0, lscpu},
    {"lsmem", "list the ranges of available memory", nullptr, 0, lsmem},
    {"gdt",   "display information about GDT", nullptr, 0, gdt},
    {"ticks", "display current number of PIT ticks", nullptr, 0, ticks},
    {"reboot", "reboot the machine", nullptr, 0, reboot},
    {"shutdown", "power off the machine", nullptr, 0, shutdown},
    {"tui", "test Terminal User Interface", nullptr, 0, tui},
    {"int", "trigger interrupt", nullptr, 0, interrupt},
};

void exec(const char *cmd) noexcept
{
    const char *target {nullptr};
    int32_t len  {0};

    char **argv  {nullptr};
    int32_t argc {0};

    for (int32_t i = 0; i < BUILTINS_COUNT; i++) {
        target = builtins[i].name;
        len    = kstd::strlen(target);

        if (kstd::strncmp(target, cmd, len) == 0) {
            // TODO: handle shell arguments

            argv = kstd::split(cmd, " ", &argc);
            builtins[i].func(argc, argv);

            kstd::free_split(argv);
            return;
        }
    }

    printk("sh: %s: command not found \n", cmd);
}

const char *get_suitable_cmd(const char *str, int32_t len) noexcept
{
    // warning: for large number of shell commands it is better to use
    // algorithm based on the search tree (with search complexity O(log N))
    // in order to get suitable command instead of this
    // (with search complexity O(N))

    for (int32_t i = 0; i < BUILTINS_COUNT; i++) {
        // TODO: handle few similar commands
        if (kstd::strncmp(builtins[i].name, str, len) == 0)
            return builtins[i].name;
    }

    return nullptr;
}

// Builtins -------------------------------------------------------------------

static void help(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    builtin_t *cmd {nullptr};
    int32_t    len {0};

    for (uint8_t i = 0; i < BUILTINS_COUNT; i++) {
        cmd = &builtins[i];
        len = kstd::strlen(cmd->name);

        kstd::putk(builtins[i].name);

        for (int32_t i = 0; i < 16 - len; i++)
            kstd::putchar(' ');

        kstd::putk(builtins[i].descr);
        kstd::putchar('\n');
    }
}

static void clear(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    tty::clear();
}

static void uname(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    // display general kernel info
    uint32_t mode = arch::x86::mode();
    uint32_t ring = arch::x86::ring();

    printk(
        "kernel name:      |  %s\n"
        "kernel version:   |  v%d.%d.%d\n"
        "Architecture:     |  %s\n"
        "Operating mode:   |  %u-bit protected\n"
        "Privilege level:  |  ring %u (%s)\n"
        "Author:           |  %s - 2024\n",
        info::__kernel_name__,
        info::__kernel_version_major__,
        info::__kernel_version_minor__,
        info::__kernel_version_lower__,
        info::__kernel_arch__,
        mode,
        ring,
        current_space(ring),
        info::__kernel_author__
    );

    // display kernel build info
    printk("\nbuild time: %s %s [g++-%s]\n",
        info::__kernel_build_time__,
        info::__kernel_build_date__,
        info::__kernel_compiler_version__
    );
}

static void date(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    printk("%s\n", ktime::get_date());
}

static void lscpu(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    using namespace arch::x86;

    cpu::details_t details {};
    cpu::get_details(details);

    printk("Architecture:   %s\n", cpu::arch);
    printk("CPU op-mode(s): %s\n", cpu::get_op_modes());
    printk("Byte Order:     %s\n", cpu::byte_order);
    printk("Vendor ID:      %s\n", details.vendor);
    printk("CPU type:       %s\n", cpu::type_to_str(cpu::TYPE(details.type)));
    printk("Model name:     %s\n", details.brand);
    printk("CPU family:     %u\n", details.family);
    printk("CPU model:      %u\n", details.model);
}

static void lsmem(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    core::memory::display_memory();
}

static void gdt(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    // while trying to get GDT info by using GDT pointer struct
    // that set at GDT_BASE <0x00000800> compiler show warnings
    // -Werror=array-bounds=. I know what I'm doing, so
    // I disabled it in this case:
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"

    using namespace arch::x86;

    gdt::ptr_t *gdt_ptr = reinterpret_cast<gdt::ptr_t*>(gdt::GDT_BASE);

    printk("GDT descriptor: <%08p>\n", gdt_ptr);
    printk("offset:         <%08p>\n", gdt_ptr->offset);
    printk("size:             %u bytes\n", gdt_ptr->size);

    printk("\n%s\n", "kernel dump of GDT descriptor:");
    debug::kdump(gdt::GDT_BASE, 0);

    printk("\n%s\n", "kernel dump of Global Descriptor Table:");
    debug::kdump(gdt_ptr->offset, gdt_ptr->size);

    #pragma GCC diagnostic pop
}

static void ticks(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused
    printk("PIT ticks: %u\n", driver::pit::get_ticks());
}

static void reboot(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    arch::x86::cli();               // disable interrupts
    arch::x86::outb(0x64, 0xFE);    // reset CPU
    arch::x86::halt();              // halt CPU
}

static void shutdown(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    arch::x86::outw(0x0604, 0x2000); // for QEMU
    arch::x86::outw(0x4004, 0x3400); // for VirtualBox
}

// Rotating 3D cube demo ----------------------------------------------------

struct vertice_t {
    float32_t x;
    float32_t y;
    float32_t z;
};

inline const vertice_t cube_vertices[8] {
    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f}
};

inline const int32_t cube_edges[12][2] {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

// shift from upper-left corner of the screen
inline const int32_t SCREEN_SHIFT {300};

/**
 * @brief Project 3D points to 2D.
 *
 * @param [in] vert - given cube vertice.
 * @param [out] point - given point that holds projected x & y positions.
 * @param [in] angle - given rotation angle.
 */
static void project(const vertice_t& vert, gfx::point_t& point, float32_t angle) noexcept
{
    float32_t scale     = 200.0f; // scale factor for projection
    float32_t z_offset  = 3.0f;   // distance from the viewer

    // applying rotation around the Y-axis
    float32_t cos_angle = kstd::cos(angle);
    float32_t sin_angle = kstd::sin(angle);
    float32_t rotated_x = vert.x * cos_angle - vert.z * sin_angle;
    float32_t rotated_z = vert.x * sin_angle + vert.z * cos_angle;

    // perspective projection
    auto center_x = scale * rotated_x / (rotated_z + z_offset);
    auto center_y = scale * vert.y / (rotated_z + z_offset);

    point.x = static_cast<int32_t>(center_x) + 160;
    point.y = static_cast<int32_t>(center_y) + 120;
}

/**
 * @brief Draw the cube.
 *
 * @param [in] angle - given cube rotation angle.
 */
static void draw_cube(float32_t angle, gfx::rgb_t color) noexcept
{
    gfx::point_t p1, p2;
    int32_t      v1, v2;

    for (int i = 0; i < 12; i++) {
        v1 = cube_edges[i][0];
        v2 = cube_edges[i][1];

        // projecting the vertices to 2D
        project(cube_vertices[v1], p1, angle);
        project(cube_vertices[v2], p2, angle);

        // drawing the line between the projected points
        gfx::draw_line(p1 + SCREEN_SHIFT, p2 + SCREEN_SHIFT, color);
    }
}

/**
 * @brief Get the next RGB color.
 *
 * @param [in] color - given color.
 * @return next color relative to the given one.
 */
static gfx::rgb_t get_next_color(gfx::rgb_t color) noexcept
{
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    if (r == 255 && g < 255 && b == 0)
        g+=3;
    else if (g == 255 && r > 0 && b == 0)
        r-=3;
    else if (g == 255 && b < 255)
        b+=3;
    else if (b == 255 && g > 0)
        g+=3;
    else if (b == 255 && r < 255)
        r+=3;
    else if (r == 255 && g == 0 && b == 0)
        return 0xFF0000;
    else
        return 0xFF0000;

    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

/**
 * @brief Single cube rotating iteration.
 *
 * @param [in] angle - given cube rotation angle.
 * @param [in] color - given cube color.
 */
static void interation(float32_t angle, gfx::rgb_t *color) noexcept
{
    using namespace driver;
    gfx::rgb_t new_color;

    // update terminal cursor position
    tty::terminal.x_pos = tty::terminal.begin_x_pos;
    tty::terminal.y_pos = tty::terminal.begin_y_pos;

    // clearing the screen
    auto shift  = static_cast<uint32_t>(SCREEN_SHIFT);
    auto bg_col = tty::terminal.bg;

    for (uint32_t y = 0; y < shift; y++) {
        for (uint32_t x = 0; x < shift; x++)
            vesa::draw_pixel(x + SCREEN_SHIFT, y + SCREEN_SHIFT - 50, bg_col);
    }

    // update color
    new_color = get_next_color(*color);
    draw_cube(angle, new_color);
    *color = new_color;
}

/** @brief Display rounding cube.*/
static void round_cube(void) noexcept
{
    using namespace driver;
    float32_t angle           = 0.0f;
    float32_t angle_increment = 0.05f; // Increment angle for rotation

    gfx::rgb_t color = 0xFF0000; // red color
    keyboard::KEY key;

    for (;;) {
        key = keyboard::getch();

        switch (key) {
        case keyboard::KEY::A:
            // updating the angle for the next frame
            angle -= angle_increment;

            // at some point the drawing of the cube becomes incorrect
            // so that code preventing this case
            if (static_cast<int32_t>(angle) <= 0)
                angle = 32;

            interation(angle, &color);
            break;

        case keyboard::KEY::D:
            // updating the angle for the next frame
            angle += angle_increment;

            // at some point the drawing of the cube becomes incorrect
            // so that code preventing this case
            if (static_cast<int32_t>(angle) >= 32)
                angle = 0;

            interation(angle, &color);
            break;

        case keyboard::KEY::ESC:
            return;

        default:
            break;
        }

    }
}

// --------------------------------------------------------------------------

static void tui(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    using namespace gfx::tui;

    frame_t frame;
    frame.init();

    window_t window;

    window.init(frame, "TUI window");
    window.add_content("Open an application?\n");

    auto submit_on_click = [](void *wptr) {
        auto window = static_cast<window_t*>(wptr);
        window->destroy();
        tty::clear();

        kstd::putk("press 'A' to rotate cube in left direction\n");
        kstd::putk("press 'D' to rotate cube in right direction\n");
        kstd::putk("press 'ESC' key to exit\n");
        auto begin_x_pos = tty::terminal.x_pos;
        auto begin_y_pos = tty::terminal.y_pos;

        round_cube();

        tty::terminal.x_pos = begin_x_pos;
        tty::terminal.y_pos = begin_y_pos;
    };

    auto reject_on_click = [](void *wptr) {
        auto window = static_cast<window_t*>(wptr);
        window->destroy();
    };

    window.add_button("< YES >", submit_on_click, &window, {100, 225});
    window.add_button("< NO >",  reject_on_click, &window, {200, 225});
    window.show();
}

static void interrupt(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused
    __asm__ volatile ("int $13");
}

} // namespace shell
} // namespace kernel