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
#include <kernel/arch/i386/irq.hpp>
#include <kernel/arch/i386/io.hpp>
#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cstdio.hpp>
#include <kernel/kstd/cctype.hpp>
#include <kernel/panic.hpp>


namespace kernel {
namespace driver {
namespace keyboard {

inline const u32 UNKNOWN = 0xFFFFFFFF;
inline const u32 ESC     = 0xFFFFFFFF - 1;
inline const u32 CTRL    = 0xFFFFFFFF - 2;
inline const u32 LSHFT   = 0xFFFFFFFF - 3;
inline const u32 RSHFT   = 0xFFFFFFFF - 4;
inline const u32 ALT     = 0xFFFFFFFF - 5;
inline const u32 F1      = 0xFFFFFFFF - 6;
inline const u32 F2      = 0xFFFFFFFF - 7;
inline const u32 F3      = 0xFFFFFFFF - 8;
inline const u32 F4      = 0xFFFFFFFF - 9;
inline const u32 F5      = 0xFFFFFFFF - 10;
inline const u32 F6      = 0xFFFFFFFF - 11;
inline const u32 F7      = 0xFFFFFFFF - 12;
inline const u32 F8      = 0xFFFFFFFF - 13;
inline const u32 F9      = 0xFFFFFFFF - 14;
inline const u32 F10     = 0xFFFFFFFF - 15;
inline const u32 F11     = 0xFFFFFFFF - 16;
inline const u32 F12     = 0xFFFFFFFF - 17;
inline const u32 SCRLCK  = 0xFFFFFFFF - 18;
inline const u32 HOME    = 0xFFFFFFFF - 19;
inline const u32 UP      = 0xFFFFFFFF - 20;
inline const u32 LEFT    = 0xFFFFFFFF - 21;
inline const u32 RIGHT   = 0xFFFFFFFF - 22;
inline const u32 DOWN    = 0xFFFFFFFF - 23;
inline const u32 PGUP    = 0xFFFFFFFF - 24;
inline const u32 PGDOWN  = 0xFFFFFFFF - 25;
inline const u32 END     = 0xFFFFFFFF - 26;
inline const u32 INS     = 0xFFFFFFFF - 27;
inline const u32 DEL     = 0xFFFFFFFF - 28;
inline const u32 CAPS    = 0xFFFFFFFF - 29;
inline const u32 NONE    = 0xFFFFFFFF - 30;
inline const u32 ALTGR   = 0xFFFFFFFF - 31;
inline const u32 NUMLCK  = 0xFFFFFFFF - 32;

const u32 lowercase[128] = {
UNKNOWN,ESC,'1','2','3','4','5','6','7','8', '9','0','-','=','\b','\t','q','w',
'e','r','t','y','u','i','o','p','[',']','\n',CTRL, 'a','s','d','f','g','h','j',
'k','l',';','\'','`',LSHFT,'\\','z','x','c','v','b','n', 'm',',','.','/',RSHFT,
'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK, SCRLCK,HOME,UP,PGUP,
'-',LEFT,UNKNOWN,RIGHT,'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,
F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN
};

const u32 uppercase[128] = {
UNKNOWN,ESC,'!','@','#','$','%','^','&','*', '(',')','_','+','\b','\t','Q','W',
'E','R','T','Y','U','I','O','P','{','}','\n',CTRL, 'A','S','D','F','G','H','J',
'K','L',':','"','~',LSHFT,'|','Z','X','C','V','B','N', 'M','<','>','?',RSHFT,
'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK, SCRLCK,HOME,UP,PGUP,
'-',LEFT,UNKNOWN,RIGHT,'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,
F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN
};

using namespace arch::i386;

static key_handler ctrl_handler[128] {nullptr};

static volatile u8 scan_code   {0};
static volatile u8 press       {0};

static bool is_caps      = false;
static bool is_caps_lock = false;
static bool is_ctrl      = false;

KEY getch(void) noexcept
{
    auto key  = static_cast<KEY>(scan_code);
    scan_code = 0;
    return key;
}

void set_ctrl_handler(KEY key, key_handler handler) noexcept
{
    auto pos = static_cast<s32>(key);
    ctrl_handler[pos] = handler;
}

/**
 * @brief Handle CTRL+key.
 *
 * @param [in] key - given key to handle.
 */
static void handle_ctrl(s32 key) noexcept
{
    auto pos = static_cast<s32>(key);
    if (pos <= 0 || pos >= 128)
        panic(PANIC_ERR "%s\n", "incorrect key");

    auto handler = ctrl_handler[pos];

    // handle case when there is no custom key handler
    if (handler && !press)
        handler();

    press   = true;
    is_ctrl = false;
}

u8 getchar(void) noexcept
{
    while((inb(0x64) & 0x01) == 0)
        continue;

    auto key = static_cast<KEY>(scan_code);

    switch(key) {
        case KEY::LCTRL:
            is_ctrl = true;
            break;

        case KEY::LSHIFT:
            is_caps = !press;
            break;

        case KEY::CAPS_LOCK:
            if (!press)
                is_caps_lock = !is_caps_lock;
            break;

        default:
            if(!press && !is_ctrl) {
                bool    is_upper = (is_caps || is_caps_lock);
                u8 cc {0};

                if(is_upper && (lowercase[scan_code] != UNKNOWN))
                    cc = static_cast<u8>(uppercase[scan_code]);
                else
                    cc = static_cast<u8>(lowercase[scan_code]);

                return cc;
            }
            else if (!press && is_ctrl)
                handle_ctrl(scan_code);

            is_ctrl = false;
            break;
    }
    return 0;
}

static key_handler tab_handler {nullptr};

void set_tab_handler(key_handler handler) noexcept
{
    tab_handler = handler;
}

static u32 pos {0};

u32 get_pos(void) noexcept
{
    return pos;
}

void set_pos(u32 p) noexcept
{
    pos = p;
}

void get_line(char *buffer, usize size) noexcept
{
    pos      = 0;
    char ch  = 0;

    do {
        ch = getchar();

        if (ch && ch != '\n') {
            // handle backspace character
            if (ch == '\b') {
                if (pos == 0)
                    continue;
                else {
                    pos--;
                    buffer[pos] = 0;
                }
            }
            else if (ch == '\t') {
                if (tab_handler) {
                    bool ret = tab_handler();

                    if (ret)
                        pos = kstd::strlen(buffer);

                    continue;
                }
                else {
                    while (pos < size) {
                        buffer[pos] = ' ';
                        pos++;
                    }
                }
            }

            kstd::putchar(ch);

            // add character to the kernel shell buffer
            if (pos < size && kstd::isprint(ch)) {
                buffer[pos] = ch;
                pos++;
            }
        }
    } while (ch != '\n');

    // truncate buffer
    buffer[pos] = 0;
    kstd::putchar('\n');

    tab_handler = nullptr;
}

void keyboard_handler(irq::int_regs_t *regs) noexcept
{
    (void)regs; // unused

    scan_code = inb(0x60) & 0x7F; // get code of key that is pressed
    press     = inb(0x60) & 0x80; // is key is pressed down or released
}

void init(void) noexcept
{
    irq::request(IRQ::KEYBOARD, &keyboard_handler);
}

} // namespace keyboard
} // namespace driver
} // namespace kernel