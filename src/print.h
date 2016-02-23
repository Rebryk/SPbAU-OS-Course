#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdarg.h>
#include "uart.h"

//#define DEBUG

typedef unsigned int size_t;

int printf(const char*, ...);

int snprintf(char*, size_t, const char*, ...);

#endif
