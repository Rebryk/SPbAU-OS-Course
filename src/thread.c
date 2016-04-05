#include "thread.h"
#include "stdio.h"
#include "utils.h"

struct thread_t threads[THREADS_COUNT];

struct list_head free_threads;
struct list_head busy_threads;
struct list_head terminated_threads;

static struct thread_t* volatile current_thread = NULL;
static struct thread_t* volatile main_thread = NULL;

/*
const int MAX_CNT = 40;
int cnt = 0;
*/

void threads_init() {
	//if (++cnt < MAX_CNT) printf("threads_init: begin\n");

	start_critical_section();
	list_init(&busy_threads);
	list_init(&free_threads);
	list_init(&terminated_threads);
	
	for (size_t i = 0; i < THREADS_COUNT; ++i) {
		//threads[i].id = i;
		list_init(&threads[i].node);
		list_add_tail(&threads[i].node, &free_threads);
	}

	current_thread = thread_create(NULL, NULL);
	current_thread->status = RUNNING;

	end_critical_section();

	//if (++cnt < MAX_CNT) printf("threads init end\n");
}

void set_main_thread(struct thread_t* thread) { 
	current_thread = thread;
}

struct thread_t* thread_create(void* (*func)(void *), void *arg) {	
	start_critical_section();

	//if (++cnt < MAX_CNT) printf("%d: thread_create: begin\n", current_thread ? current_thread->id : -1);

	struct thread_t* thread = LIST_ENTRY(list_first(&free_threads), struct thread_t, node);
	list_del(list_first(&free_threads));
 	
 	thread->stack_start = (uint8_t *)kmem_alloc(THREAD_STACK_SIZE);
 	thread->stack = (void*)((uint8_t *)thread->stack_start + THREAD_STACK_SIZE - 1);
	thread->func = func;
	thread->arg = arg;
	thread->result = NULL;
	thread->status = NOT_STARTED;

	//if (++cnt < MAX_CNT) printf("%d: thread_create: end\n", current_thread ? current_thread->id : -1);

	end_critical_section();

	return thread;
}

struct thread_t* thread_get_current() {
	return current_thread;
}

void thread_terminate() {
	start_critical_section();

	//if (++cnt < MAX_CNT) printf("%d: thread_terminate: begin\n", current_thread ? current_thread->id : -1);	

	current_thread->status = TERMINATED;
	list_add_tail(&current_thread->node, &terminated_threads);
	end_critical_section();
	thread_yield();
}

void thread_start(struct thread_t* thread) {
	start_critical_section();

	//if (++cnt < MAX_CNT) printf("%d: thread_start: begin\n", current_thread ? current_thread->id : -1);

	list_add_tail(&thread->node, &busy_threads);

	//if (++cnt < MAX_CNT) printf("%d: thread_start: end\n", current_thread ? current_thread->id : -1);

	end_critical_section();
}

void thread_destroy(struct thread_t* thread) {
	start_critical_section();

	//if (++cnt < MAX_CNT) printf("%d: thread_destroy: begin\n", current_thread ? current_thread->id : -1);

	kmem_free(thread->stack_start);
	list_del(&thread->node);
	list_add_tail(&thread->node, &free_threads);

	//if (++cnt < MAX_CNT) printf("%d: thread_destroy: end\n", current_thread ? current_thread->id : -1);

	end_critical_section();
}

void thread_join(struct thread_t* thread, void** result) {
	while (thread->status != TERMINATED) {
		thread_yield();
		barrier();
	}
	*result = thread->result;
}

void switch_context(struct thread_t * new_thread, struct thread_t *volatile* old_thread);

void thread_yield() {
	start_critical_section();

    if (current_thread != NULL && current_thread->status == RUNNING) {
		list_add_tail(&current_thread->node, &busy_threads);
    }

	struct thread_t* thread = LIST_ENTRY(list_first(&busy_threads), struct thread_t, node);
	list_del(&thread->node);
	
	//if (++cnt < MAX_CNT) printf("next thread: %d\n", thread->id);

	switch_context(thread, &current_thread);

    end_critical_section();
}

void try_wrapper_entry(struct thread_t* thread) {
	//if (++cnt < MAX_CNT) printf("%d: try_wrapper_entry: begin\n", current_thread ? current_thread->id : -1);

    if (thread->status == NOT_STARTED) {
		//if (++cnt < MAX_CNT) printf("try_wrapper_yield: before critical section\n");

	    thread->status = RUNNING;
		
		//if (++cnt < MAX_CNT) printf("critical section depth: %d\n", critical_depth);
		//if (++cnt < MAX_CNT) printf("%d: try_wrapper_entry: func call\n", current_thread ? current_thread->id : -1);
		
		end_critical_section();

		thread->result = thread->func(thread->arg);
		thread_terminate();
	}

	//if (++cnt < MAX_CNT) printf("%d: try_wrapper_entry: end\n", current_thread ? current_thread->id : -1);
}