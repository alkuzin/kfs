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

#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cstdio.hpp>
#include <kernel/kstd/cctype.hpp>
#include <kernel/kstd/cmath.hpp>


namespace kernel {
namespace kstd {
    
// For null pointer in vsnprintk.
constexpr auto __NIL__ {"(nil)"};

// Print buffer info struct.
typedef struct pinfo_s {
    char   *m_buf; 
    size_t  m_size; 		// Buffer size.
    int32_t m_pos;  		// Current buffer position.
    bool    m_pref_flag; 	// '#' flag
} pinfo_t;

static pinfo_t pinfo;

/**
 * @brief Append character to pinfo struct buffer.
 * 
 * @param [in] c - given character to append.
 */
static inline void buf_append(char c) noexcept
{
	pinfo.m_buf[pinfo.m_pos] = c;
	pinfo.m_pos++;
}

/**
 * @brief Initialize pinfo struct.
 * 
 * @param [out] buf - given buffer for containing printk output.
 * @param [in] size - given size of the buffer.
 */
static void pinfo_init(char *buf, size_t size) noexcept
{
	memset(&pinfo, 0, sizeof(pinfo));
	memset(buf, 0, size);

    pinfo.m_buf       = buf;
    pinfo.m_size      = size;
    pinfo.m_pos       = 0;
    pinfo.m_pref_flag = 0;
}

// Print Unsigned int32_t --------------------------------------------------------------------------

/**
 * @brief Unsigned int32_t to alphanumeric length.
 * 
 * @param [in] n - given number.
 * @return length of unsigned int string representation of @a n.
 */
static constexpr size_t utoa_len(uint32_t n) noexcept
{
	size_t len = 0;

	if (n == 0)
		return 1;

	while (n >= 1) {
		len++;
		n /= 10;
	}

	return len;
}

/**
 * @brief %u option (unsigned int).
 * 
 * @param [in] n - given number.
 */
static void print_uint(uint32_t n) noexcept
{
	size_t i      = utoa_len(n);
	size_t length = i;

	char uint_buffer[i];

	i--;

	while (i) {
		uint_buffer[i] = (n % 10) + '0';
		n /= 10;
		i--;
	}

	uint_buffer[0]      = (n % 10) + '0';
	uint_buffer[length] = '\0';

	i = 0;

    // TODO: use width alignment flag here
	while (uint_buffer[i]) {
		buf_append(uint_buffer[i]);
		i++;
	}
}

// Print Pointer -----------------------------------------------------------------------------------------

/**
 * @brief Digit to hex.
 * 
 * @param [in] v - given digit to convert. 
 * @return hex representation of digit.
 */
static constexpr inline char dtoh(int32_t v) noexcept
{
   if (v >= 0 && v < 10)
       return '0' + v;
   else
       return 'a' + v - 10;
}

/**
 * @brief %p option (pointer).
 * 
 * @param [in] p - given pointer to convert.
 */
static void print_ptr(uint64_t p) noexcept
{
	int32_t count, i;
		
	if(p == 0) {
		i = 0;
		
		while(__NIL__[i]) {
			buf_append(__NIL__[i]);
			i++;
		}
		
		return;
	}
	
	buf_append('0');
	buf_append('x');
	
	i     = (sizeof(p) << 3) - 4;	
	count = 0;

	// skip first zeros
	while(i >= 0 && ((dtoh((p >> i) & 0xf) == '0')))
		i -= 4;
	
	while(i >= 0) {
		buf_append(dtoh((p >> i) & 0xf));
		i -= 4;
		count++;
	}
}

// Print int ----------------------------------------------------------------------------------

/**
 * @brief Integer to alphanumeric length.
 * 
 * @param [in] n - given number.
 * @return length of int string representation of @a n.
 */
static constexpr size_t itoa_len(int32_t n) noexcept
{
	size_t len = 0;

	if (n == 0)
		return 1;
	
	if (n < 0) {
		n = -n;
		len++;
	}

	while (n >= 1) {
		len++;
		n /= 10;
	}

	return len;
}

/**
 * @brief %i %d options (int)
 * 
 * @param [in] n - given number to convert.
 */
static void print_int(int32_t n) noexcept
{
	size_t i      = itoa_len(n);
	size_t length = i;

	char int_buffer[length];
	i--;

	if (n < 0) {
		int_buffer[0] = '-';
		n = -n;
	}

	while (i) {
		int_buffer[i] = (n % 10) + '0';
		n /= 10;
		i--;
	}
	
	if (int_buffer[0] != '-')
		int_buffer[0] = (n % 10) + '0';

	int_buffer[length] = '\0';
	
	while (int_buffer[i]) {
		buf_append(int_buffer[i]);
		i++;
	}
}

// Printf hex -----------------------------------------------------------------------------------

/**
 * @brief Hexadecimal to alphanumeric length.
 * 
 * @param [in] n - given number.
 * @return length of hex string representation of @a n.
 */
static constexpr inline int32_t xtoa_len(uint32_t n) noexcept
{
	if (n == 0)
	   return 1;
	
	return static_cast<int32_t>(log(n) / log(16)) + 1;
}

/**
 * @brief %x %X options (hexadecimal).
 * 
 * @param [in] n - given number to convert.
 * @param [in] is_upper - given is uppercase/lowercase flag.
 */
static void print_hex(uint64_t n, int32_t is_upper) noexcept
{
	static const char digits_lower[] = "0123456789abcdef";
	static const char digits_upper[] = "0123456789ABCDEF";
	static const char *digits        = nullptr;
	int32_t i;
	
	i = xtoa_len(n);
	char hex[i + 1];
	
	if(pinfo.m_pref_flag) {
		buf_append('0');
		buf_append('x');
        pinfo.m_pref_flag = false;
	}
	
	if(n == 0) {
		buf_append('0');
		return;
	}

	if(is_upper) 
		digits = digits_upper;
	else
		digits = digits_lower;
	
	i = 7;

	while(i >= 0) {
		hex[i] = digits[(n & 0x0F)];
		n >>= 4;
		--i;
	}

	hex[8] = '\0';
	i      = 0;
		
	while(hex[i] == '0')
		i++;

	while(hex[i]) {
		buf_append(hex[i]);
		i++;
	}
}

// Argument Parsing ----------------------------------------------------------------------- 

/**
 * @brief Print vsnprintk arguments.
 * 
 * @param [in] type - given type of argument.
 * @param [in] args - given list of arguments.
 */
static void print_args(char type, va_list *args) noexcept
{
	char *str = nullptr;
	int32_t i 	  = 0;

	switch(type) {
		case '%':
			buf_append(type);
			break;

		case 'c':
			buf_append(va_arg(*args, int));
			break;

		case 's':
			str = va_arg(*args, char *);
			
			while(str[i]) {
				buf_append(str[i]);
				i++;
			}
			break;

		case 'p':
			print_ptr(reinterpret_cast<uint64_t>(va_arg(*args, void *)));
			break;

		case 'd': case 'i':
		    print_int(va_arg(*args, int));
			break;

		case 'u':
			print_uint(va_arg(*args, uint32_t));
			break;

		case 'x': case 'X':
			print_hex(va_arg(*args, uint64_t), isupper(type));
			break;
	};
}

/**
 * @brief Parse printk arguments.
 * 
 * @param [in] str - given format string.
 * @param [in] args - given list of arguments.
 */
static void parse(const char *str, va_list *args) noexcept
{
    int32_t i = -1;

    while(str[++i]) {

        if(str[i] == '%' && str[i + 1] != '\0') {
			
            if(str[i + 1] == '#' && str[i + 1] != '\0') {
                pinfo.m_pref_flag = true;
				i++;
                print_args(str[i + 1], args);
			}
			else
            	print_args(str[i + 1], args);
            i++;
        }
        else
			buf_append(str[i]);
    }
}

void vsnprintk(char *buf, size_t size, const char *fmt, va_list args) noexcept
{
    va_list args_copy;
	
    pinfo_init(buf, size);

    va_copy(args_copy, args);
    parse(fmt, &args);
    va_end(args_copy);
}

} // namespace kstd
} // namespace kernel