#include "kmem_cache.h"
#include "interrupt.h"
#include "threads.h"
#include "memory.h"
#include "serial.h"
#include "paging.h"
#include "stdio.h"
#include "string.h"
#include "misc.h"
#include "time.h"
#include "fs.h"
#include "balloc.h"
#include "initramfs.h"

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
	DBG_INFO("Start buddy test");
	struct page *page[10];
	int order[ARRAY_SIZE(page)];

	for (int i = 0; i != ARRAY_SIZE(page); ++i) {
		page[i] = alloc_pages(i);
		if (page[i]) {
			const phys_t begin = page_paddr(page[i]);
			const phys_t end = begin + (PAGE_SIZE << i);
			DBG_INFO("allocated [%#llx-%#llx]", begin, end - 1);
			order[i] = i;
		}
	}

	for (int i = 0; i != ARRAY_SIZE(page) - 1; ++i) {
		if (!page[i])
			break;
		for (int j = i + 1; j != ARRAY_SIZE(page); ++j) {
			if (!page[j])
				break;

			const phys_t ibegin = page_paddr(page[i]);
			const phys_t iend = ibegin + (PAGE_SIZE << order[i]);

			const phys_t jbegin = page_paddr(page[j]);
			const phys_t jend = jbegin + (PAGE_SIZE << order[j]);

			DBG_ASSERT(!range_intersect(ibegin, iend, jbegin, jend));
		}
	}

	for (int i = 0; i != ARRAY_SIZE(page); ++i) {
		if (!page[i])
			continue;

		const phys_t begin = page_paddr(page[i]);
		const phys_t end = begin + (PAGE_SIZE << i);
		DBG_INFO("freed [%#llx-%#llx]", begin, end - 1);
		free_pages(page[i], order[i]);
	}
	DBG_INFO("Buddy test finished");
}

struct intlist {
	struct list_head link;
	int data;
};

static void slab_smoke_test(void)
{
	DBG_INFO("Start SLAB test");
	struct kmem_cache *cache = KMEM_CACHE(struct intlist);
	LIST_HEAD(head);
	int i;

	for (i = 0; i != 1000000; ++i) {
		struct intlist *node = kmem_cache_alloc(cache);

		if (!node)
			break;
		node->data = i;
		list_add_tail(&node->link, &head);
	}

	DBG_INFO("Allocated %d nodes", i);

	while (!list_empty(&head)) {
		struct intlist *node = LIST_ENTRY(list_first(&head),
					struct intlist, link);

		list_del(&node->link);
		kmem_cache_free(cache, node);
	}

	kmem_cache_destroy(cache);
	DBG_INFO("SLAB test finished");
}

static int test_function(void *dummy)
{
	(void) dummy;
	return 0;
}

static void test_threading(void)
{
	DBG_INFO("Start threading test");
	for (int i = 0; i != 10000; ++i) {
		const pid_t pid = create_kthread(&test_function, 0);

		DBG_ASSERT(pid >= 0);
		wait(pid);
	}
	DBG_INFO("Threading test finished");
}*/


static void test_dir() {
	bool ok = true;

	ok &= (mkdir("dir1") == true);
	ok &= (mkdir("dir2") == true);
	ok &= (mkdir("dir1/dir3") == true);
	ok &= (mkdir("dir1/dir4") == true);
	ok &= (mkdir("dir1/dir3") == false);
	
	printf("test_dir: %s.\n", ok ? "OK" : "FAIL");
}

static void test_files() {
	bool ok = true;

	struct file_desc_t* file = open("dir1/test.txt", WRITE);
	const char* text = "Hello, World!";
	write(file, text, strlen(text));
	close(file);
	
	file = open("dir1/test.txt", READ);
	char buff[100];
	int count = read(file, buff, 100);
	ok &= !strcmp(buff, text);
	ok &= (count == 13);
	close(file);

	file = open("dir1/dir3/dir5/test.txt", READ);
	ok &= (file == 0);
	close(file);

	readdir("");
	readdir("dir1");
	readdir("dir2");

	printf("test_files: %s.\n", ok ? "OK" : "FAIL");
}

static void test_fs() {
	test_dir();
	test_files();
}


static int start_kernel(void *dummy)
{
	(void) dummy;

	//buddy_smoke_test();
	//slab_smoke_test();
	//test_threading();
	test_fs();

	return 0;
}

// make clean && make && qemu-system-x86_64 -kernel kernel -nographic -initrd files

void main(void)
{
	setup_serial();
	setup_misc();
	setup_ints();
	setup_memory();
	setup_initramfs();

	setup_buddy();
	setup_paging();
	setup_alloc();
	setup_time();
	setup_threading();
	fs_init();
	read_initramfs();

	local_irq_enable();

	create_kthread(&start_kernel, 0);
	local_preempt_enable();
	idle();

	while (1);
}
