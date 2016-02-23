#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdint.h>
#include "memory.h"
#include "uart.h"

#define DESCRIPTORS_COUNT				256
#define INTERRUPTION					14
#define P_BIT						(1 << 7)

#define bits(x, l, r) ((x >> l) & ((1ll << (r - l + 1)) - 1))

struct IDT_PTR  {
	uint16_t size;
	uint64_t base;
} __attribute__((packed));

struct IDT_DESCRIPTOR {
	uint16_t 	offset_0_15;
	uint16_t 	segment_selector; 
	uint8_t	 	interrupt_stack_table;
	uint8_t 	flags;
	uint16_t	offset_16_31;
	uint32_t	offset_32_63;
	uint32_t 	reserved;
} __attribute__((packed));

static struct IDT_DESCRIPTOR descriptors[DESCRIPTORS_COUNT];
static struct IDT_PTR idt;

void init_idt();

void init_descriptor(uint8_t, uint64_t, uint8_t);

void empty();

void pop_error();

static inline void set_idt(const struct IDT_PTR *ptr) { 
	__asm__ volatile ("lidt (%0)" : : "a"(ptr));
}

// enable interruptions
static inline void sti() { 
	__asm__ volatile ("sti");
}

// disable interruptions
static inline void cli() { 
	__asm__ volatile ("cli");
}

#endif
