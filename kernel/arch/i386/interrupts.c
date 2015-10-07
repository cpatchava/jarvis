
#include "kernel/interrupts.h"
#include "kernel/lib.h"

void divide_by_zero(){
	clear();
	printf("divide by zero exception");
	while(1);
}
