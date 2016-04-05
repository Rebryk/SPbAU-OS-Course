#include "lock.h"
#include "utils.h"
#include "interrupt.h"
#include "thread.h"

#include <stddef.h>

volatile int critical_depth = 0;

void lock(spinlock_t* lock) {
	if (lock == NULL) {
		return;
	}

	const uint16_t ticket = __sync_fetch_and_add(&lock->users, 1);
	while (lock->ticket != ticket) {
		barrier();
		thread_yield();
	}
	__sync_synchronize(); /* we don't use cmpxchg explicitly */
}

void unlock(spinlock_t* lock) {
	if (lock == NULL) {
		return;
	}
	__sync_synchronize();
    __sync_add_and_fetch(&lock->ticket, 1);
}


void start_critical_section() {
	cli();
	__sync_fetch_and_add(&critical_depth, 1);
}

void end_critical_section() {
	if (__sync_fetch_and_add(&critical_depth, -1) == 1) {
		sti();
	}
}