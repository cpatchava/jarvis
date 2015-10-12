/* debug.h - Useful macros for debugging
 * vim:ts=4 noexpandtab
 */

#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

void divide_by_zero();

void debugger_interrupt();
void non_maskable_interrupt();
void breakpoint_interrupt();
void overflow_error();
void out_of_bounds_error();
void invalid_opcode_interrupt();
void coprocessor_not_avaliable();
void coprocessor_segment_overrun();
void invalid_task_state_segment();
void segment_not_present();
void page_fault();
void general_protection_fault();
void reserved_error();

#endif /* _DEBUG_H */
