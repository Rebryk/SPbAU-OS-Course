#include "pic.h"
#include "pit.h"
#include "print.h"

void test_print() {
	printf("%s\n", "printf tests:");
	printf("%c = c\n", 'c');
	printf("%d = 10\n", 10);
	printf("%x = f\n", 15);
	printf("%o = 11\n", 9);
	printf("%zu != -10\n", -10);
	printf("%llu = 2^63\n", ((long long int)1 << 63));
	printf("%lld is negative\n", ((long long int)1 << 63));
	printf("Where is my %lld$?\n", (long long int)1e18);
	printf("0x%p = 0xffffffff80000000\n", KERNEL_BASE);

	printf("\n%s\n", "snprintf tests:");
	char buff[20];
	int len = snprintf(buff, 20, "%d%d", 1234567890, 123456789);
	printf("len (%s) = %d = 19\n", buff, len);

	len = snprintf(buff, 20, "%d%d", 1234567890, 1234567890);
	printf("%d = -1\n", len);
}

void main(void) { 
	init_uart();
	test_print();

	init_pic();
	init_idt();
	init_pit();
	sti();

	while (1) {} 
}
