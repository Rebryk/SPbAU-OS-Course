#include "interrupt.h"

static const uint8_t ERROR_INTERRUPTS_COUNT 	= 7;
static const uint8_t ERROR_INTERRUPTS[] 	= {8, 10, 11, 12, 13, 14, 17};

void init_descriptor(uint8_t index, uint64_t offset, uint8_t flags) {
	descriptors[index].reserved 			= 0;
	descriptors[index].segment_selector 		= KERNEL_CODE;
	descriptors[index].interrupt_stack_table	= 0;
	descriptors[index].flags 			= flags;
	descriptors[index].offset_0_15 			= bits(offset, 0, 15); 
	descriptors[index].offset_16_31	 		= bits(offset, 16, 31);
	descriptors[index].offset_32_63			= bits(offset, 32, 63);
}

void init_idt() {
	for (int i = 0; i < DESCRIPTORS_COUNT; i++) {
		init_descriptor(i, (uint64_t)&empty, INTERRUPTION | P_BIT);
	}
	
	for (int i = 0; i < ERROR_INTERRUPTS_COUNT; i++) {
		init_descriptor(ERROR_INTERRUPTS[i], (uint64_t)&pop_error, INTERRUPTION | P_BIT);	
	}

	idt.base = (uint64_t)descriptors;
	idt.size = sizeof(descriptors) - 1;
	set_idt(&idt);
}
