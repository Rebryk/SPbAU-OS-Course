#include "uart.h"

#define BIT(x, n) (x & (1 << n))

void putc_uart(const char c) {
	while (!BIT(in8(LSR), 5)) {}
	out8(UART, c);
}

int printn_uart(const char* s, int n) {
	int len = n;
	while (*s && n--) {
		putc_uart(*s++);
	}
	return len;
}

int print_uart(const char* s) {
	const char* end = s;
	while (*end) {
		putc_uart(*end++);
	}
	return end - s;
}

void init_uart() {
	// Line Control Register
	out8(LCR, 0x83);

	// Setup division coefficient (600)
	out8(UART, 0xc0);
	out8(IER, 0x00);

	out8(LCR, 0x03);

	// Interrupt Enable Register
	out8(IER, 0x00);

	print_uart("UART has been initialized!\n");
}
