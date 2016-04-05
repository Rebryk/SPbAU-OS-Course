#ifndef __THREAD_H__
#define __THREAD_H__

#include <sys/types.h>
#include "memory.h"
#include "kmem_cache.h"
#include "list.h"
#include "lock.h"

#include "stdio.h"

#define THREADS_COUNT (1 << 16)
#define THREAD_STACK_SIZE (1 << 13)

typedef enum {
	NOT_STARTED,
	RUNNING,
	TERMINATED
} thread_status;

struct thread_t {
	void* stack;
	void* (*func)(void*);
	void* result;
	void* arg;
	void* stack_start;

	thread_status status;
	struct list_head node;
};

void set_main_thread(struct thread_t* thread);

void threads_init();

struct thread_t* thread_create(void* (*func)(void *), void *arg);

void thread_start(struct thread_t* thread);

void thread_destroy(struct thread_t* thread);

void thread_join(struct thread_t* thread, void** result);

void thread_yield();

struct thread_t* thread_get_current();

#endif /* __THREAD_H_ */