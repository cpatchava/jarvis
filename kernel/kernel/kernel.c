#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/dt.h>

void kernel_early(void)
{
	terminal_initialize();
	init_descriptor_tables();
}

void kernel_main(void)
{
	//terminal_initialize();
//	init_descriptor_tables();
	char a =0;
	char b =0;
	printf("Hello, kernel World! \n");
	putchar(a);
//	printf("Location of b: \n",b );
}
