#include "pit.h"

void init_pit() {
	out8(CONTROL_PORT, MOD | RATE);
	out8(DATA_PORT, bits(FREQUENCY_DIV, 0, 7));
	out8(DATA_PORT, bits(FREQUENCY_DIV, 8, 15));
	
	init_descriptor(0x20, (uint64_t)&pit, INTERRUPTION | P_BIT);
}

void handler() {
	print_uart("IT WORKS!\n");
	reset_master_eoi();
}