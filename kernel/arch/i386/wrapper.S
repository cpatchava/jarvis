/**/

#define ASM		1
#include "kernel/x86_desc.h"

.text

.globl	_kb_wrapper


_kb_wrapper:

	push %ebx
	push %ecx
	push %edx
	push %esi
	push %edi
	push %ebp
	push %eax
	push %ds
	push %es
	push %fs

	call keyboard_data

	popl %fs
	popl %es
	popl %ds
	popl %eax
	popl %ebp
	popl %edi
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx
	
	iret
