/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "kernel/multiboot.h"
#include "kernel/x86_desc.h"
#include "kernel/lib.h"
#include "kernel/wrapper.h"
#include "kernel/i8259.h"
#include "kernel/debug.h"
#include "kernel/interrupts.h"

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;

	/* Clear the screen. */
	clear();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	if (CHECK_FLAG (mbi->flags, 3)) {
		int mod_count = 0;
		int i;
		module_t* mod = (module_t*)mbi->mods_addr;
		while(mod_count < mbi->mods_count) {
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<16; i++) {
				printf("0x%x ", *((char*)(mod->mod_start+i)));
			}
			printf("\n");
			mod_count++;
			//mod++;
		}
	}
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = 0x0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = 0x0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 0x9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000;
		ltr(KERNEL_TSS);
	}

 /* Setup Trap Entries */                                                                       

  idt_desc_t idt_entry;       // setup idt_entry                                                 
  idt_entry.dpl       = 0;
  idt_entry.present     = 1;
  idt_entry.seg_selector    =KERNEL_CS;
  idt_entry.reserved3     =1; //40    
  idt_entry.reserved2     =1; //41   
  idt_entry.reserved1     =1; //42  
  idt_entry.size        =1; //43   
  idt_entry.reserved0     =0; //44   


  //0x00
  SET_IDT_ENTRY(idt_entry, divide_by_zero);
  idt[0x00] = idt_entry;
  
  /* Setup System Call Entry */
/* 
 idt_entry.dpl       = 3;
  idt_entry.present     = 1;
  idt_entry.seg_selector    =KERNEL_CS;
  idt_entry.reserved3     =1; //40
  idt_entry.reserved2     =1; //41
  idt_entry.reserved1     =1; //42
  idt_entry.size        =1; //43
  idt_entry.reserved0     =0; //44
  */
    
  /* Setup Interrupt Entries */
  idt_entry.dpl       = 0;
  idt_entry.present     = 1;
  idt_entry.seg_selector    =KERNEL_CS;
  idt_entry.reserved3     =0; //40
  idt_entry.reserved2     =1; //41
  idt_entry.reserved1     =1; //42
  idt_entry.size        =1; //43
  idt_entry.reserved0     =0; //44
  

	SET_IDT_ENTRY(idt_entry, divide_by_zero);
	idt[0x00] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, debugger_interrupt);
	idt[0x01] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, non_maskable_interrupt);
	idt[0x02] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, breakpoint_interrupt);
	idt[0x03] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, overflow_error);
	idt[0x04] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, out_of_bounds_error);
	idt[0x05] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, invalid_opcode_interrupt);
	idt[0x06] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, coprocessor_not_avaliable);
	idt[0x07] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, coprocessor_not_avaliable);
	idt[0x08] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, coprocessor_segment_overrun);
	idt[0x09] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, invalid_task_state_segment);
	idt[0x0A] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, segment_not_present);
	idt[0x0B] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, page_fault);
	idt[0x0C] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, general_protection_fault);
	idt[0x0D] = idt_entry;
	SET_IDT_ENTRY(idt_entry, page_fault);
	idt[0x0E] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, reserved_error);
	idt[0x0F] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, reserved_error);
	idt[0x10] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, reserved_error);
	idt[0x11] = idt_entry;	
	SET_IDT_ENTRY(idt_entry, reserved_error);
	idt[0x12] = idt_entry;	
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x13] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x14] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x15] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x16] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x17] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x18] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x19] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x1A] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x1B] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x1C] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x1D] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x1E] = idt_entry;
  SET_IDT_ENTRY(idt_entry, reserved_error);
  idt[0x1F] = idt_entry;

	/*Interrupt Entries setup*/

	clear();
	printf("Testing if its even coming here");

	SET_IDT_ENTRY(idt_entry, &_kb_wrapper);
	idt[0x21] = idt_entry;	
	//clear();	
	
	/* Init the PIC */
	cli();
	i8259_init();
	/* Initialize devices, memory, filesystem, enable device interrupts on the
	 * PIC, any other initialization stuff... */

	/* Enable interrupts */
	/* Do not enable the following until after you have set up your
	 * IDT correctly otherwise QEMU will triple fault and simple close
	 * without showing you any output */
	sti();
	printf("Enabling Interrupts");

	/* Execute the first program (`shell') ... */
	while(1);
	/* Spin (nicely, so we don't chew up cycles) */
	asm volatile(".1: hlt; jmp .1;");
}

