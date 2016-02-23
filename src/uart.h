#ifndef __UART_H__
#define __UART_H__

#define UART 	0x3f8
#define IER   	0x3f9
#define LCR		0x3fb
#define LSR		0x3fd

#include "ioport.h"

int printn_uart(const char*, int);

int print_uart(const char*);

void init_uart();

#endif