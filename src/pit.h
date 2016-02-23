#ifndef __PIT_H__
#define __PIT_H__

#include "pic.h"
#include "uart.h"

#define CONTROL_PORT	0x43
#define DATA_PORT		0x40
#define RATE			4
#define MOD				0x30
#define FREQUENCY_DIV	0xFFFF

void init_pit();

void pit();

void handler();

#endif