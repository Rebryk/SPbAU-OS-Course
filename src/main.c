#include "kmem_cache.h"
#include "interrupt.h"
#include "memory.h"
#include "serial.h"
#include "paging.h"
#include "stdio.h"
#include "misc.h"
#include "time.h"
#include "utils.h"
#include "thread.h"

/*
static bool range_intersect(phys_t l0, phys_t r0, phys_t l1, phys_t r1)
{
	if (r0 <= l1)
		return false;
	if (r1 <= l0)
		return false;
	return true;
}

static void buddy_smoke_test(void)
{
#define PAGES 10
	struct page *page[PAGES];
	int order[PAGES];

	for (int i = 0; i != PAGES; ++i) {
		page[i] = alloc_pages(i);
		if (page[i]) {
			const phys_t begin = page_paddr(page[i]);
			const phys_t end = begin + (PAGE_SIZE << i);
			printf("allocated [%#llx-%#llx]\n", begin, end - 1);
			order[i] = i;
		}
	}

	for (int i = 0; i != PAGES - 1; ++i) {
		if (!page[i])
			break;
		for (int j = i + 1; j != PAGES; ++j) {
			if (!page[j])
				break;

			const phys_t ibegin = page_paddr(page[i]);
			const phys_t iend = ibegin + (PAGE_SIZE << order[i]);

			const phys_t jbegin = page_paddr(page[j]);
			const phys_t jend = jbegin + (PAGE_SIZE << order[j]);

			DBG_ASSERT(!range_intersect(ibegin, iend, jbegin, jend));
		}
	}

	for (int i = 0; i != PAGES; ++i) {
		if (!page[i])
			continue;

		const phys_t begin = page_paddr(page[i]);
		const phys_t end = begin + (PAGE_SIZE << i);
		printf("freed [%#llx-%#llx]\n", begin, end - 1);
		free_pages(page[i], order[i]);
	}
#undef PAGES
}

struct intlist {
	struct list_head link;
	int data;
};

static void slab_smoke_test(void)
{
#define ALLOCS 1000000
	struct kmem_cache *cache = KMEM_CACHE(struct intlist);
	LIST_HEAD(head);
	int i;

	for (i = 0; i != ALLOCS; ++i) {
		struct intlist *node = kmem_cache_alloc(cache);

		if (!node)
			break;
		node->data = i;
		list_add_tail(&node->link, &head);
	}

	printf("Allocated %d nodes\n", i);

	while (!list_empty(&head)) {
		struct intlist *node = LIST_ENTRY(list_first(&head),
					struct intlist, link);

		list_del(&node->link);
		kmem_cache_free(cache, node);
	}

	kmem_cache_destroy(cache);
#undef ALLOCS
}*/

struct join_pair_t {
	struct thread_t* thread;
	void* arg;
};

struct pair_t {
	void* first;
	void* second;
};

static void pow2(void* value) {
	*(int*) value *= *(int*) value;
}

static void* thread_run_1(void* value) {
	pow2(value);
	return value;
}

static void* thread_run_2(void* value) {
	struct join_pair_t* data = (struct join_pair_t*) value;
	void* res = 0;
	thread_join(data->thread, &res);
	pow2(data->arg);
	*(int*) data->arg += *(int*) res;
	return value;
}

static void thread_test() {
	int arg1 = 2;
	int arg2 = 3;

	struct thread_t* thread_1 = thread_create(thread_run_1, &arg1);

	struct join_pair_t pair;
	pair.thread = thread_1;
	pair.arg = (void*) &arg2;

	struct thread_t* thread_2 = thread_create(thread_run_2, &pair);

	thread_start(thread_2);
	thread_start(thread_1);

	void* res = 0;
	thread_join(thread_2, &res);

	thread_destroy(thread_1);
	thread_destroy(thread_2);

	if (*(int*) ((struct join_pair_t*) res)->arg == 13) {
		printf("thread_test: passed! \n");
	} else {
		printf("thread_test: failed! \n");
	}
}

static spinlock_t test_lock;

static int wait(int id, int* arg) {
	int ok = 1;
	for (int i = 0; i < (int)1e6 && ok; ++i) {
		if (id != *arg) {
			ok = 0;
		}
		barrier();
	}
	return ok;
}

static void* func1(void* arg) {
	struct pair_t* pair = (struct pair_t*) arg;

	lock(&test_lock);
	*(int*) pair->first = 1;
	*(int*) pair->second = wait(1, (int*) pair->first);
	unlock(&test_lock);	

	return pair->second;
}

static void* func2(void* arg) {
	struct pair_t* pair = (struct pair_t*) arg;

	lock(&test_lock);
	*(int*) pair->first = 2;
	*(int*) pair->second = wait(2, (int*) pair->first);
	unlock(&test_lock);

	return pair->second;
}

static void lock_test() {
	int thread_id;
	int ret1;
	int ret2;
	struct pair_t arg1;
	struct pair_t arg2;

	arg1.first = (void*) &thread_id;
	arg1.second = (void*) &ret1;
	struct thread_t* thread_1 = thread_create(func1, &arg1);

	arg2.first = (void*) &thread_id;
	arg2.second = (void*) &ret2;
	struct thread_t* thread_2 = thread_create(func2, &arg2);

	thread_start(thread_1);
	thread_start(thread_2);

	void* res1;
	thread_join(thread_1, &res1);

	void* res2;
	thread_join(thread_2, &res2);

	thread_destroy(thread_1);
	thread_destroy(thread_2);

	if (*(int*) res1 == 1 && *(int*) res2 == 1) {
		printf("lock_test: passed! \n");
	} else {
		printf("lock_test: failed! \n");
	}
}

// cd /media/sf_shared/os-course/sol/ && make clean && make && qemu-system-x86_64 -kernel kernel -nographic

void main(void)
{
	setup_serial();
	setup_misc();
	setup_ints();
	setup_memory();
	setup_buddy();
	setup_paging();
	setup_alloc();
	setup_time();

	threads_init();
	thread_test();
	lock_test();	

	printf("main: end\n");
	while (1) {
		barrier();
	}
}
