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
#include <kernel/arch/x86/io.hpp>


namespace kernel {
namespace driver {

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
UNKNOWN,ESC,'1','2','3','4','5','6','7','8', '9','0','-','=','\b','\t','q','w','e','r',
't','y','u','i','o','p','[',']','\n',CTRL, 'a','s','d','f','g','h','j','k','l',';',
'\'','`',LSHFT,'\\','z','x','c','v','b','n', 'm',',','.','/',RSHFT,'*',ALT,' ',CAPS,F1,
F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK, SCRLCK,HOME,UP,PGUP,'-',LEFT,UNKNOWN,RIGHT,'+',END,
DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN, UNKNOWN,UNKNOWN,UNKNOWN
};

const uint32_t uppercase[128] = {
UNKNOWN,ESC,'!','@','#','$','%','^','&','*', '(',')','_','+','\b','\t','Q','W','E','R',
'T','Y','U','I','O','P','{','}','\n',CTRL, 'A','S','D','F','G','H','J','K','L',':',
'"','~',LSHFT,'|','Z','X','C','V','B','N', 'M','<','>','?',RSHFT,'*',ALT,' ',CAPS,F1,
F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK, SCRLCK,HOME,UP,PGUP,'-',LEFT,UNKNOWN,RIGHT,'+',END,
DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN 
};


void keyboard_t::set(void) noexcept
{
    m_is_caps      = false;
    m_is_caps_lock = false;
}

inline void keyboard_t::wait(void) const noexcept
{
    while((arch::x86::inb(0x64) & 0x01) == 0)
        continue;
}

uint8_t keyboard_t::getchar(void) const noexcept
{
    wait();

    uint8_t scan_code = arch::x86::inb(0x60) & 0x7F; // get code of key that is pressed
    uint8_t press     = arch::x86::inb(0x60) & 0x80; // is key is pressed down or released

    switch(static_cast<key>(scan_code)) {
        case key::up_arrow:
        case key::down_arrow:
        case key::left_arrow:
        case key::right_arrow:

        case key::lshft:
            if(!press)
                m_is_caps = true;
            else
                m_is_caps = false;
            break;

        case key::caps_lock:
            if(!m_is_caps_lock && !press)
                m_is_caps_lock = true;
            else if(m_is_caps_lock && !press)
                m_is_caps_lock = false;
            break;

        default:
            if(!press) {
                uint8_t cc;

                if((m_is_caps || m_is_caps_lock) && (lowercase[scan_code] != UNKNOWN))
                    cc = uppercase[scan_code];
                else
                    cc = lowercase[scan_code];

                return cc;
            }
            break;
    }
    return 0;
}

keyboard_t keyboard;

} // namespace driver
} // namespace kernel