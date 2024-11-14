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

inline const u8 BUILTINS_COUNT {14};

static void help(s32 argc, char **argv) noexcept;
static void clear(s32 argc, char **argv) noexcept;
static void uname(s32 argc, char **argv) noexcept;
static void date(s32 argc, char **argv) noexcept;
static void lscpu(s32 argc, char **argv) noexcept;
static void lsmem(s32 argc, char **argv) noexcept;
static void gdt(s32 argc, char **argv) noexcept;
static void ticks(s32 argc, char **argv) noexcept;
static void reboot(s32 argc, char **argv) noexcept;
static void shutdown(s32 argc, char **argv) noexcept;
static void tui(s32 argc, char **argv) noexcept;
static void interrupt(s32 argc, char **argv) noexcept;
static void uptime(s32 argc, char **argv) noexcept;
static void dump(s32 argc, char **argv) noexcept;

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
    {"uptime", "tell how long the system has been running", nullptr, 0, uptime},
    {"dump", "memory dump specific memmory address", nullptr, 0, dump},
};

void exec(const char *cmd) noexcept
{
    const char *target {nullptr};
    s32 len  {0};

    char **argv  {nullptr};
    s32 argc {0};

    for (s32 i = 0; i < BUILTINS_COUNT; i++) {
        target = builtins[i].name;
        len    = kstd::strlen(target);

        if (kstd::strncmp(target, cmd, len) == 0) {
            argv = kstd::split(cmd, " ", &argc);
            builtins[i].func(argc, argv);

            kstd::free_split(argv);
            return;
        }
    }

    printk("sh: %s: command not found \n", cmd);
}

const char *get_suitable_cmd(const char *str, s32 len) noexcept
{
    // warning: for large number of shell commands it is better to use
    // algorithm based on the search tree (with search complexity O(log N))
    // in order to get suitable command instead of this
    // (with search complexity O(N))

    for (s32 i = 0; i < BUILTINS_COUNT; i++) {
        // TODO: handle few similar commands
        if (kstd::strncmp(builtins[i].name, str, len) == 0)
            return builtins[i].name;
    }

    return nullptr;
}

// Builtins -------------------------------------------------------------------

static void help(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    builtin_t *cmd {nullptr};
    s32    len {0};

    for (u8 i = 0; i < BUILTINS_COUNT; i++) {
        cmd = &builtins[i];
        len = kstd::strlen(cmd->name);

        kstd::putk(builtins[i].name);

        for (s32 j = 0; j < 16 - len; j++)
            kstd::putchar(' ');

        kstd::putk(builtins[i].descr);
        kstd::putchar('\n');
    }
}

static void clear(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    tty::clear();
}

static void uname(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    // display general kernel info
    u32 mode = arch::x86::mode();
    u32 ring = arch::x86::ring();

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

static void date(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    printk("%s\n", ktime::get_date());
}

static void lscpu(s32 argc, char **argv) noexcept
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

static void lsmem(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    core::memory::display_memory();
}

static void gdt(s32 argc, char **argv) noexcept
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

static void ticks(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused
    printk("PIT ticks: %u\n", driver::pit::get_ticks());
}

static void reboot(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    arch::x86::cli();               // disable interrupts
    arch::x86::outb(0x64, 0xFE);    // reset CPU
    arch::x86::halt();              // halt CPU
}

static void shutdown(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    arch::x86::outw(0x0604, 0x2000); // for QEMU
    arch::x86::outw(0x4004, 0x3400); // for VirtualBox
}

// Rotating 3D cube demo ----------------------------------------------------

struct vertice_t {
    f32 x;
    f32 y;
    f32 z;
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

inline const s32 cube_edges[12][2] {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

// shift from upper-left corner of the screen
inline const s32 SCREEN_SHIFT {300};

/**
 * @brief Project 3D points to 2D.
 *
 * @param [in] vert - given cube vertice.
 * @param [out] point - given point that holds projected x & y positions.
 * @param [in] angle - given rotation angle.
 */
static void project(const vertice_t& vert, gfx::point_t& point, f32 angle) noexcept
{
    f32 scale     = 200.0f; // scale factor for projection
    f32 z_offset  = 3.0f;   // distance from the viewer

    // applying rotation around the Y-axis
    f32 cos_angle = static_cast<f32>(kstd::cos(angle));
    f32 sin_angle = static_cast<f32>(kstd::sin(angle));
    f32 rotated_x = vert.x * cos_angle - vert.z * sin_angle;
    f32 rotated_z = vert.x * sin_angle + vert.z * cos_angle;

    // perspective projection
    auto center_x = scale * rotated_x / (rotated_z + z_offset);
    auto center_y = scale * vert.y / (rotated_z + z_offset);

    point.x = static_cast<s32>(center_x) + 160;
    point.y = static_cast<s32>(center_y) + 120;
}

/**
 * @brief Draw the cube.
 *
 * @param [in] angle - given cube rotation angle.
 */
static void draw_cube(f32 angle, rgb_t color) noexcept
{
    gfx::point_t p1, p2;
    s32      v1, v2;

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
static rgb_t get_next_color(rgb_t color) noexcept
{
    u8 r = (color >> 16) & 0xFF;
    u8 g = (color >> 8) & 0xFF;
    u8 b = color & 0xFF;

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
static void interation(f32 angle, rgb_t *color) noexcept
{
    using namespace driver;
    rgb_t new_color;

    // update terminal cursor position
    tty::terminal.x_pos = tty::terminal.begin_x_pos;
    tty::terminal.y_pos = tty::terminal.begin_y_pos;

    // clearing the screen
    auto shift  = static_cast<u32>(SCREEN_SHIFT);
    auto bg_col = tty::terminal.bg;

    for (u32 y = 0; y < shift; y++) {
        for (u32 x = 0; x < shift; x++)
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
    f32 angle           = 0.0f;
    f32 angle_increment = 0.05f; // Increment angle for rotation

    rgb_t color = 0xFF0000; // red color
    keyboard::KEY key;

    for (;;) {
        key = keyboard::getch();

        switch (key) {
        case keyboard::KEY::A:
            // updating the angle for the next frame
            angle -= angle_increment;

            // at some point the drawing of the cube becomes incorrect
            // so that code preventing this case
            if (static_cast<s32>(angle) <= 0)
                angle = 32;

            interation(angle, &color);
            break;

        case keyboard::KEY::D:
            // updating the angle for the next frame
            angle += angle_increment;

            // at some point the drawing of the cube becomes incorrect
            // so that code preventing this case
            if (static_cast<s32>(angle) >= 32)
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

static void tui(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    using namespace gfx::tui;

    frame_t frame;
    frame.init();

    window_t window;

    window.init(frame, "TUI window");
    window.add_content("Open an application?\n");

    auto submit_on_click = [](void *wptr) {
        auto win = static_cast<window_t*>(wptr);
        win->destroy();
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
        auto win = static_cast<window_t*>(wptr);
        win->destroy();
    };

    window.add_button("< YES >", submit_on_click, &window, {100, 225});
    window.add_button("< NO >",  reject_on_click, &window, {200, 225});
    window.show();
}

static void interrupt(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused
    __asm__ volatile ("int $0");
}

static void uptime(s32 argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused
    ktime_t boot_time = ktime::get_boot_time();
    ktime_t cur_time  = ktime::clock();
    ktime_t diff_time = cur_time - boot_time;

    auto days    = diff_time / 86400000;
    auto hours   = diff_time / 3600000;
    auto minutes = diff_time / 60000;
    auto seconds = diff_time / 1000;

    if (days > 0) printk("%u days, ", days % 24);
    if (hours > 0) printk("%u hours, ", hours % 60);
    if (minutes > 0) printk("%u minutes, ", minutes % 60);

    printk("%u seconds, ", seconds % 60);
    printk("%u milliseconds\n", diff_time % 1000);
}

static void dump(s32 argc, char **argv) noexcept
{
    if (argc == 1 || argc > 3) {
        printk("dump: %s\n", "incorrect number of arguments");
        printk("dump: %s\n", "try: 'dump <addr> <lines>'");
        return;
    }

    if (argc == 2) {
        phys_addr_t addr = kstd::stoh(argv[1]);
        printk(KERN_DEBUG "addr: %X\n", addr);
        debug::kdump(addr, 32);
    }
    else if (argc == 3) {
        phys_addr_t addr = kstd::stoh(argv[1]);
        u32 lines   = kstd::stou<u32>(argv[2]);
        debug::kdump(addr, lines);
    }
}

} // namespace shell
} // namespace kernel