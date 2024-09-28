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

// vsnprintk handler --------------------------------------------------------------

// For null pointer in vsnprintk.
const char *NIL {"(nil)"};

// Flags bit position:
const auto FLAG_PREFIX  {1};
const auto FLAG_PADDING {2};

struct handler_t
{
	char	*m_buffer;
	size_t	 m_size;
	va_list *m_args;
	uint8_t	 m_flags;
	size_t   m_pos;
	uint8_t  m_num;

private:
	/**
	 * @brief Get the specific flag.
	 *
	 * @param [in] pos - given bit position.
	 * @return true if flag = 1.
	 * @return false if flag = 0.
	 */
	inline bool get_flag(uint8_t pos) const noexcept;

	/**
	 * @brief Set the flag bit.
	 *
	 * @param [in] pos - given bit position.
	 */
	inline void set_flag(uint8_t pos) noexcept;

	/**
	 * @brief Handle vsnprintk argument.
	 *
	 * @param [in] type - given type of argument to print.
	 */
	void handle_argument(char type) noexcept;

	/**
	 * @brief Handle vsnprintk flag.
	 *
	 * @param [in] flag - given flag of argument to print.
	 */
	void handle_flag(char flag) noexcept;

	/** @brief Append string argument to buffer.*/
	void append_string(void) noexcept;

	/** @brief Append pointer argument to buffer.*/
	void append_pointer(void) noexcept;

	/** @brief Append integer argument to buffer.*/
	void append_integer(void) noexcept;

	/** @brief Append unsigned integer argument to buffer.*/
	void append_uinteger(void) noexcept;

	/**
	 * @brief Append hexadecimal argument to buffer.
	 *
	 * @param [in] is_upper - given flag to choice what type of digits to use.
	 */
	void append_hex(bool is_upper) noexcept;

public:
	/**
	 * @brief Initialize vsnprintk handler.
	 *
	 * @param [in] buf - given buffer pointer.
	 * @param [in] size - given buffer size.
	 * @param [in] args - given variable number of arguments.
	 */
	void init(char *buf, size_t size, va_list *args) noexcept;

	/**
	 * @brief Append character to buffer.
	 *
	 * @param [in] ch - given character to append.
	 */
	void append(char ch) noexcept;

	/**
	 * @brief Parse format string.
	 *
	 * @param fmt - given format string to parse.
	 */
	void parse(const char *fmt) noexcept;
};

void handler_t::init(char *buf, size_t size, va_list *args) noexcept
{
	m_buffer = buf;
	m_size   = size;
	m_args   = args;
	m_flags	 = 0;
	m_pos    = 0;
	m_num	 = 0;
}

void handler_t::append(char ch) noexcept
{
	m_buffer[m_pos] = ch;
	m_pos++;
}

inline bool handler_t::get_flag(uint8_t pos) const noexcept
{
	return (m_flags >> pos) & 0x1;
}

inline void handler_t::set_flag(uint8_t pos) noexcept
{
	m_flags |= (1 << pos);
}

void handler_t::append_string(void) noexcept
{
	char *str = static_cast<char*>(va_arg(*m_args, char*));
	auto i   = 0;

	while(str[i]) {
		append(str[i]);
		i++;
	}
}

/**
 * @brief Digit to hexadecimal.
 *
 * @param [in] v - given digit to convert.
 * @param [in] is_upper - given flag to choice what type of digits to use.
 * @return hex representation of digit.
 */
inline char dtoh(int32_t v, bool is_upper = false) noexcept
{
	char a = (is_upper) ? 'A' : 'a';

	if (v >= 0 && v < 10)
		return '0' + v;
	else
		return a + v - 10;
}

void handler_t::append_pointer(void) noexcept
{
	void *raw = static_cast<void*>(va_arg(*m_args, void *));

	if(!raw) {
		auto i = 0;

		while(NIL[i]) {
			append(NIL[i]);
			i++;
		}
	}
	else {
		append('0');
		append('x');

		auto ptr   = reinterpret_cast<uint32_t>(raw);
		int32_t i  = (sizeof(ptr) << 3) - 4;
		auto count = 0;

		if (m_num > 8)
			m_num = 8;

		auto skip_count = 8 - m_num;

		// skip first zeros
		while(i >= 0 && skip_count && ((dtoh((ptr >> i) & 0xf) == '0'))) {
			skip_count--;
			i -= 4;
		}

		while(i >= 0) {
			append(dtoh((ptr >> i) & 0xf));
			i -= 4;
			count++;
		}
	}
}

/**
 * @brief Integer to alphanumeric length.
 *
 * @param [in] n - given number.
 * @return length of int string representation of @a n.
 */
size_t itoa_len(int32_t n) noexcept
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

void handler_t::append_integer(void) noexcept
{
	int32_t n 	  = va_arg(*m_args, int32_t);
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
		append(int_buffer[i]);
		i++;
	}
}

/**
 * @brief Unsigned integer to alphanumeric length.
 *
 * @param [in] n - given number.
 * @return length of unsigned int string representation of @a n.
 */
size_t utoa_len(uint32_t n) noexcept
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

void handler_t::append_uinteger(void) noexcept
{
	uint32_t n 	  = va_arg(*m_args, uint32_t);
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

	while (uint_buffer[i]) {
		append(uint_buffer[i]);
		i++;
	}
}

void handler_t::append_hex(bool is_upper) noexcept
{
	uint32_t n = va_arg(*m_args, uint32_t);
	int32_t i  = (sizeof(n) << 3) - 4;
	auto count = 0;

	if(get_flag(FLAG_PREFIX)) {
		append('0');
		append('x');
	}

	if (m_num > 8)
		m_num = 8;

	auto skip_count = 8 - m_num;

	// skip first zeros
	while(i >= 0 && skip_count && ((dtoh((n >> i) & 0xf, is_upper) == '0'))) {
		skip_count--;
		i -= 4;
	}

	while(i >= 0) {
		append(dtoh((n >> i) & 0xf, is_upper));
		i -= 4;
		count++;
	}
}

void handler_t::handle_flag(char flag) noexcept
{
	switch (flag) {
	// handle output prefix
	case '#':
		set_flag(FLAG_PREFIX);
		break;

	// handle padding
	case '0':
		set_flag(FLAG_PADDING);
		break;

	// handle incorrect flag
	default:
		handle_argument(flag);
	}
}

void handler_t::handle_argument(char type) noexcept
{
	switch (type) {
	// handle character
	case 'c':
		append(static_cast<char>(va_arg(*m_args, int32_t)));
		break;

	// handle string
	case 's':
		append_string();
		break;

	// handle pointer
	case 'p':
		append_pointer();
		break;

	// handle integer
	case 'd': case 'i':
		append_integer();
		break;

	// handle unsigned integer
	case 'u':
		append_uinteger();
		break;

	// handle hexadecimal number
	case 'x': case 'X':
		append_hex(isupper(type));
		break;

	// handle incorrect type
	default:
		// handle padding
		if (get_flag(FLAG_PADDING) && kstd::isdigit(type))
			m_num = type - '0';
		else
			append(type);

		break;
	}
}

void handler_t::parse(const char *fmt) noexcept
{
	size_t i = 0;
	char ch  = 0;

	do {
		ch = fmt[i];

		// handle '%' if it isn't the last one
		if ((ch == '%') && (i < m_size - 1)) {
			ch = fmt[i + 1];

			while (ch) {

				handle_flag(ch);

				i++;
				ch = fmt[i + 1];

				// skip non-alphanumerical characters
				if (!kstd::isalnum(ch)) {
					i++;
					break;
				}
			}

			m_flags = 0x0;
			m_num   = 0;
		}

		append(ch);
		i++;
	} while (ch);
}

static handler_t handler;

void vsnprintk(char *buf, size_t size, const char *fmt, va_list args) noexcept
{
    va_list args_copy;

    va_copy(args_copy, args);
    handler.init(buf, size, &args_copy);
    handler.parse(fmt);
    va_end(args);
}

} // namespace kstd
} // namespace kernel