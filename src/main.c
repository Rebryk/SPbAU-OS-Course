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

void* foo(void* str) {
	printf("%s\n", (char*)str);

	while (1) {
	//for (int i = 0; i < (int)1e8; ++i) {
		barrier();
	}

	return str;
}

// cd /media/sf_shared/os-course/sol/ && make clean && make && qemu-system-x86_64 -kernel kernel -nographic

void main(void)
{
	printf("Main: begin\n");

	setup_serial();
	setup_misc();
	setup_ints();
	setup_memory();
	setup_buddy();
	setup_paging();
	setup_alloc();
	setup_time();
	
	start_critical_section();	
	threads_init();

	struct thread_t* main_thread = thread_create(NULL, NULL);
	main_thread->status = RUNNING;
	set_main_thread(main_thread);
	
	char str1[] = "thread1";
	struct thread_t* thread1 = thread_create(foo, str1);
	thread_start(thread1);

	/*
	char str2[] = "thread2";
	struct thread_t* thread2 = thread_create(foo, str2);
	thread_start(thread2);
	
	char str3[] = "thread3";
	struct thread_t* thread3 = thread_create(foo, str3);
	thread_start(thread3);*/

	end_critical_section();
	
	/*char str[] = "foo!";
	struct thread_t* thread = thread_create(foo, str);
	thread_start(thread);*/

	//local_irq_enable();

	//buddy_smoke_test();
	//slab_smoke_test();

	printf("main finised!");
	while (1) {
		barrier();
	}
}
