
#include "kernel/interrupts.h"
#include "kernel/lib.h"

void divide_by_zero(){
	clear();
	printf("divide by zero exception");
	while(1);
}

void debugger_interrupt(){
	clear();
	printf("debugger_interrupt");
	while(1);
}

void non_maskable_interrupt(){
	clear();
	printf("non maskable interrupt");
	while(1);
}

void breakpoint_interrupt(){
	clear();
	printf("breakpoint interrupt");
	while(1);
}

void overflow_error(){
	clear();
	printf("overflow error");
	while(1);
}

void out_of_bounds_error(){
	clear();
	printf("out of bounds error");
	while(1);
}

void invalid_opcode_interrupt(){
	clear();
	printf("invalid_opcode_interrupt");
	while(1);
}

void coprocessor_not_avaliable(){
	clear();
	printf("coprocessor not avaliable");
	while(1);
}

void coprocessor_segment_overrun(){
	clear();
	printf("coprocessor not avaliable");
	while(1);

}
void invalid_task_state_segment(){
	clear();
	printf("invalid task state segment");
	while(1);
}

void segment_not_present(){
	clear();
	printf("segment not present");
	while(1);
}

void page_fault(){
	clear();
	printf("page fault");
	while(1);
}

void general_protection_fault(){
	clear();
	printf("general protection fault");
	while(1);
}

void reserved_error(){
	clear();
	printf("reserved error");
	while(1);
}
