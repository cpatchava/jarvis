# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	# Initialize the core kernel before running the global constructors.
	call kernel_early

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
.Lhang:
	hlt
	jmp .Lhang

.global gdt_flush    # Allows the C code to call gdt_flush().

gdt_flush:
   movl %eax,0x4(%esp,1)  # Get the pointer to the GDT, passed as a parameter.
   lgdt (%eax)        # Load the new GDT pointer

   mov %ax, 0x10      # 0x10 is the offset in the GDT to our data segment
   mov %ds, %ax        # Load all data segment selectors
   mov %es, %ax
   mov %fs, %ax
   mov %gs, %ax
   mov %ss, %ax
   jmp	$0x08, $.flush   # 0x08 is the offset to our code segment: Far jump!
.flush:
   ret

.size _start, . - _start
