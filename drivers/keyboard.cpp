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
#include <kernel/arch/x86/irq.hpp>
#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cstdio.hpp>
#include <kernel/kstd/cctype.hpp>
#include <kernel/arch/x86/io.hpp>


namespace kernel {
namespace driver {
namespace keyboard {

inline const uint32_t UNKNOWN = 0xFFFFFFFF;
inline const uint32_t ESC     = 0xFFFFFFFF - 1;
inline const uint32_t CTRL    = 0xFFFFFFFF - 2;
inline const uint32_t LSHFT   = 0xFFFFFFFF - 3;
inline const uint32_t RSHFT   = 0xFFFFFFFF - 4;
inline const uint32_t ALT     = 0xFFFFFFFF - 5;
inline const uint32_t F1      = 0xFFFFFFFF - 6;
inline const uint32_t F2      = 0xFFFFFFFF - 7;
inline const uint32_t F3      = 0xFFFFFFFF - 8;
inline const uint32_t F4      = 0xFFFFFFFF - 9;
inline const uint32_t F5      = 0xFFFFFFFF - 10;
inline const uint32_t F6      = 0xFFFFFFFF - 11;
inline const uint32_t F7      = 0xFFFFFFFF - 12;
inline const uint32_t F8      = 0xFFFFFFFF - 13;
inline const uint32_t F9      = 0xFFFFFFFF - 14;
inline const uint32_t F10     = 0xFFFFFFFF - 15;
inline const uint32_t F11     = 0xFFFFFFFF - 16;
inline const uint32_t F12     = 0xFFFFFFFF - 17;
inline const uint32_t SCRLCK  = 0xFFFFFFFF - 18;
inline const uint32_t HOME    = 0xFFFFFFFF - 19;
inline const uint32_t UP      = 0xFFFFFFFF - 20;
inline const uint32_t LEFT    = 0xFFFFFFFF - 21;
inline const uint32_t RIGHT   = 0xFFFFFFFF - 22;
inline const uint32_t DOWN    = 0xFFFFFFFF - 23;
inline const uint32_t PGUP    = 0xFFFFFFFF - 24;
inline const uint32_t PGDOWN  = 0xFFFFFFFF - 25;
inline const uint32_t END     = 0xFFFFFFFF - 26;
inline const uint32_t INS     = 0xFFFFFFFF - 27;
inline const uint32_t DEL     = 0xFFFFFFFF - 28;
inline const uint32_t CAPS    = 0xFFFFFFFF - 29;
inline const uint32_t NONE    = 0xFFFFFFFF - 30;
inline const uint32_t ALTGR   = 0xFFFFFFFF - 31;
inline const uint32_t NUMLCK  = 0xFFFFFFFF - 32;

const uint32_t lowercase[128] = {
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

const uint32_t uppercase[128] = {
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

using namespace arch::x86;

static bool is_caps      = false;
static bool is_caps_lock = false;

KEY getch(void) noexcept
{
    while((inb(0x64) & 0x01) == 0)
        continue;

    uint8_t scan_code = inb(0x60) & 0x7F; // get code of key that is pressed
    uint8_t press     = inb(0x60) & 0x80; // is key is pressed down or released

    (void)press; // unused
    return static_cast<KEY>(scan_code);
}

uint8_t getchar(void) noexcept
{
    while((inb(0x64) & 0x01) == 0)
        continue;

    uint8_t scan_code = inb(0x60) & 0x7F; // get code of key that is pressed
    uint8_t press     = inb(0x60) & 0x80; // is key is pressed down or released

    switch(static_cast<KEY>(scan_code)) {
        case KEY::UP_ARROW:
        case KEY::DOWN_ARROW:
        case KEY::LEFT_ARROW:
        case KEY::RIGHT_ARROW:
        case KEY::LSHIFT:
            is_caps = !press;
            break;

        case KEY::CAPS_LOCK:
            if (!press)
                is_caps_lock = !is_caps_lock;
            break;

        default:
            if(!press) {
                bool    is_upper = (is_caps || is_caps_lock);
                uint8_t cc {0};

                if(is_upper && (lowercase[scan_code] != UNKNOWN))
                    cc = uppercase[scan_code];
                else
                    cc = lowercase[scan_code];

                return cc;
            }
            break;
    }
    return 0;
}

static key_handler tab_handler {nullptr};

void set_tab_handler(key_handler handler) noexcept
{
    tab_handler = handler;
}

void get_line(char *buffer, size_t size) noexcept
{
    uint32_t pos = 0;
    char     ch  = 0;

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
    // TODO: implement?
}

void init(void) noexcept
{
    irq::request(IRQ::KEYBOARD, &keyboard_handler);
}

} // namespace keyboard
} // namespace driver
} // namespace kernel