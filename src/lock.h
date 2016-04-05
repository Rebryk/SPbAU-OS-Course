#ifndef __LOCK_H__
#define __LOCK_H__

#include <stdint.h>

volatile int critical_depth;

struct spinlock {
	uint16_t users;
	uint16_t ticket;
};

typedef struct spinlock spinlock_t;

void lock(spinlock_t* lock);

void unlock(spinlock_t* lock);

void start_critical_section();

void end_critical_section();

#endif /* __LOCK_H__ */