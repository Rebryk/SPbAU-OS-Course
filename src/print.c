#include "print.h"

int strlen(const char* s) {
	const char* end = s;
	while (*end) { 
		++end;
	}
	return end - s;
}

int strcmp(const char* s1, const char* s2) {
	if (s1 == 0 || s2 == 0) {
		return s1 == s2;
	}
	
	while (*s1 || *s2) {
		if (*s1 != *s2) {
			return *s1 < *s2 ? -1 : 1;
		}
		++s1, ++s2;
	}
	return 0;
}

int strncmp(const char* s1, const char* s2, int n) {
	while (n--) {
		if (s1 == 0 || s2 == 0) {
			return s1 == s2;
		} else if (*s1 != *s2) {
			return 1;
		}
		++s1, ++s2;
	}
	return 0;
}

const char* strchr(const char* str, const char c) {
	if (!str) {
		return 0;
	}

	while (*str) {
		if (*str == c) {
			return str;
		}
		++str;
	}
	return 0;
}

/*
 * SPECIFIERS
 */
#define D (1 << 0)
#define I (1 << 1)
#define U (1 << 2)
#define O (1 << 3)
#define X (1 << 4)
#define P (1 << 5)
#define C (1 << 6)
#define S (1 << 7)

/*
 * LENGTH SPECIFIER
 */ 
#define H (1 << 0)
#define L (1 << 1)
#define Z (1 << 2)
#define HH (1 << 3)
#define LL (1 << 4)

int specifier_diuox(const char* x) {
	if (x) {
		switch (*x) {
			case 'd':
				return D;
			case 'i':
				return I;
			case 'u':
				return U;
			case 'o':
				return O;
			case 'x':
				return X;
		}
	}
	return 0;
}

int specifier_p(const char* x) {
	return (x && *x == 'p') ? P : 0;
}

int specifier_cs(const char* x) {
	if (!x) {
		return 0;
	}
	return *x == 'c' ? C : (*x == 's' ? S : 0);
}

int length_specifier_1(const char* x) {
	if (!x) {
		return 0;
	}
	return *x == 'h' ? H : (*x == 'l' ? L : (*x == 'z' ? Z : 0));
}

int length_specifier_2(const char* x) {
	if (!strncmp(x, "hh", 2)) {
		return HH;
	}
	return !strncmp(x, "ll", 2) ? LL : 0;
}

void next_specifier(const char* str, const char** start, const char** end, int* len_sp, int* sp) {
	*start = str;
	while (*start) {
		*start = strchr(*start, '%');
		
		if (*start == 0) {
			break;
		}

		if ((*sp = specifier_diuox(*start + 1) | specifier_p(*start + 1) | specifier_cs(*start + 1)) != 0) {
			*len_sp = 0;
			*end = *start + 2;
			return;
		}

		if ((*len_sp = length_specifier_2(*start + 1)) != 0) {
			if ((*sp = specifier_diuox(*start + 3)) != 0) {
				*end = *start + 4;
				return;
			}
		}

		if ((*len_sp = length_specifier_1(*start + 1)) != 0) {
			if ((*sp = specifier_diuox(*start + 2) | specifier_cs(*start + 2)) != 0) {
				if (*len_sp != L || (*len_sp == L && (*sp == C || *sp == S))) {
					*end = *start + 3;
					return;
				}
			}
		}
		
		*start += 1;
		if (**start == 0) {
			*start = 0;
		}
	}
	*end = 0;
	*len_sp = *sp = 0;
}

void reverse(char* start, char* end) {
	while (start < end) {
		char tmp = *(--end);
		*end = *start;
		*start++ = tmp; 
	}
}

/*
 * PRINT
 */

#define OCT 8
#define DEC 10
#define HEX 16

#define to_char(x) (x < 10 ? ('0' + x) : ('a' + x - 10))
#define get_base(sp) (sp & (D | I | U)) ? DEC : (sp == O ? OCT : HEX)
#define dec(n) if (n-- == 0) { return -1; }

int snprint_unsigned(char* buff, size_t n, uintmax_t value, const size_t base, size_t fixed_len) {
	char* end = buff;
	do {
		dec(n);
		*end++ = to_char(value % base);
		value /= base;
		if (fixed_len) {
			--fixed_len;
		}
	} while (value || fixed_len);
	reverse(buff, end);
	return end - buff;
}

int snprint_signed(char* buff, size_t n, intmax_t value, const size_t base) {
	if (n == 0) {
		return -1;
	}
	if (value < 0) {
		buff[0] = '-';
		return snprint_unsigned(buff + 1, n - 1, -value, base, 0) + 1;
	}
	return snprint_unsigned(buff, n, value, base, 0);
}

int snprint_str(char* buff, size_t n, const char* s) {
	char* end = buff;
	while (*s) {
		dec(n);
		*end = *s++;
	}
	return end - buff;
}

int snprintn_str(char* buff, size_t n, const char* s, int len) {
	if (len > (int)n) {
		return -1;
	}
	char* end = buff;
	while (*s && len--) {
		*end = *s++;
	}
	return end - buff;
}

int snprint_char(char* buff, size_t n, const char c) {
	dec(n);
	buff[0] = c;
	return 1;
}

int print_ull(char* buff, size_t n, unsigned long long int value, const size_t base, size_t fixed_len) {
	if (buff) {
		return snprint_unsigned(buff, n, value, base, fixed_len);
	}
	char buff2[20];
	return printn_uart(buff2, snprint_unsigned(buff2, 20, value, base, fixed_len));
}

int print_ll(char* buff, size_t n, long long int value, const size_t base) {
	if (buff) {
		return snprint_signed(buff, n, value, base);
	}
	char buff2[21];
	return printn_uart(buff2, snprint_signed(buff2, 21, value, base));
}

int print_str(char* buff, size_t n, const char* s) {
	if (buff) {
		return snprint_str(buff, n, s);
	}
	return print_uart(s);
}

int printn_str(char* buff, size_t n, const char* s, int len) {
	if (buff) {
		return snprintn_str(buff, n, s, len);
	}
	return printn_uart(s, len);
}

int print_char(char* buff, size_t n, const char c) {
	if (buff) {
		return snprint_char(buff, n, c);
	}
	return printn_uart(&c, 1);
}

int print_arg(char* buff, size_t n, int len_sp, int sp, va_list arguments) {
	switch (len_sp) {
		case 0:
			if ((sp & (D | I))) {
				return print_ll(buff, n, va_arg(arguments, int), DEC);
			} else if ((sp & (U | O | X))) {
				return print_ull(buff, n, va_arg(arguments, unsigned int), get_base(sp), 0);
			} else if (sp == C) {
				return print_char(buff, n, va_arg(arguments, int));
			} else if (sp == P) {
				return print_ull(buff, n, va_arg(arguments, unsigned long long int), get_base(sp), 16);
			} else { // S
				return print_str(buff, n, va_arg(arguments, char*));
			}
		case H:
			if ((sp & (D | I))) {
				return print_ll(buff, n, va_arg(arguments, int), DEC); // short int
			} else { // U O X
				return print_ull(buff, n, va_arg(arguments, unsigned int), get_base(sp), 0); // unsigned short int
			}
		case L:
			if ((sp & (D | I))) {
				return print_ll(buff, n, va_arg(arguments, long int), DEC);
			} else if ((sp & (U | O | X))) {
				return print_ull(buff, n, va_arg(arguments, unsigned long int), get_base(sp), 0);
			} else if (sp == C) {
				return print_char(buff, n, va_arg(arguments, size_t));
			} else { // sp == S
				return print_str(buff, n, va_arg(arguments, char*));
			}
		case Z:
			return print_ull(buff, n, va_arg(arguments, size_t), get_base(sp), 0);
		case HH:
			if ((sp & (D | I))) {
				return print_char(buff, n, va_arg(arguments, int)); // signed char
			} else { // U O X
				return print_char(buff, n, va_arg(arguments, unsigned int)); // unsigned char
			}
		case LL:
			if ((sp & (D | I))) {
				return print_ll(buff, n, va_arg(arguments, long long int), DEC);
			} else { // U O X
				return print_ull(buff, n, va_arg(arguments, unsigned long long int), get_base(sp), 0);
			}
	}
	return -1;
}

#define update_counters(ret, buff, count, n) if (ret == -1) { return -1; } else { if (buff) { buff += ret; n -= ret; } count += ret; }

int print(char* buff, size_t n, const char* format, va_list arguments) {
	const char *start, *end;
	int len_sp, sp;
	int ret;
	int count = 0;

	const char* curr = format;
	while (curr) {
		next_specifier(curr, &start, &end, &len_sp, &sp);
		if (start == 0) {
			ret = print_str(buff, n, curr);
		} else {
			ret = printn_str(buff, n, curr, start - curr);
			update_counters(ret, buff, count, n);
			ret = print_arg(buff, n, len_sp, sp, arguments);
		}
		update_counters(ret, buff, count, n);
		curr = end;
	}
	if (buff) {
		if (n == 0) {
			return -1;
		}
		*buff = 0;
	}
	return count;
}

int vprintf(const char* format, va_list arguments) {
	return print(0, 0, format, arguments);
}

int printf(const char* format, ...) {
	va_list arguments;
	va_start(arguments, format);
	int ret = vprintf(format, arguments);
	va_end(arguments);
	return ret;
}


int vsnprintf(char* s, size_t n, const char* format, va_list arguments) {
	return print(s, n, format, arguments);
}

int snprintf(char* s, size_t n, const char* format, ...) {
	va_list arguments;
	va_start(arguments, format);
	int ret = vsnprintf(s, n, format, arguments);
	va_end(arguments);
	return ret;
}