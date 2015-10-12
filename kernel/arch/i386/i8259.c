/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
unsigned int cached_irq_mask = 0xffff;


#include "kernel/i8259.h"
#include "kernel/lib.h"
#define __byte(x,y)					(((unsigned char *) &(y))[x])
#define	master_mask		(__byte(0,	cached_irq_mask))
#define	slave_mask		(__byte(1,	cached_irq_mask))



/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
//uint8_t master_mask; /* IRQs 0-7 */
//uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{

	//0x21 is the I/O address of the master PIC
	//0xA1 is the I/O address of the slave PIC
	outb(0xFF, 0x21); //0xFF -> P[0x21] mask master interrupt
	outb(0xFF, 0xA1);	//0xFF -> P[0xA1] mask slave interrupt
/*
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW2_MASTER + 0, 0x21);
	outb(ICW3_MASTER, 0x21);
	
	outb(ICW1, SLAVE_8259_PORT);
	outb(ICW2_SLAVE, 0xA1);
	outb(ICW3_SLAVE, 0xA1);
	outb(ICW4 , 0xA1);

	outb(master_mask, 0x21);
	outb(slave_mask, 0xA1);
*/

	outb(0x11, 0x20);
	outb(0x20 + 0, 0x21);
	outb(0x04, 0x21);

	outb(0x11, 0xA0);
	outb(0x28, 0xA1);
	outb(0x02, 0xA1);
	outb(0x01, 0xA1);

	outb(master_mask, 0x21); // unmask
	outb(slave_mask, 0xA1);


}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
	unsigned int mask = ~(1 << irq_num);

	cached_irq_mask &= mask;
	if(irq_num & 8)
		outb(slave_mask, 0xA1);
	else
		outb(master_mask, 0x21);
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	unsigned int mask = 1 << irq_num;

	cached_irq_mask &= mask;
	if(irq_num & 8)
		outb(slave_mask, 0xA1);
	else
		outb(master_mask, 0x21);


}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	if(irq_num >= 8){
		outb((EOI | (irq_num - 8)), 0xA0);
		outb((EOI | 2), 0x20);
	} else{
		outb(EOI | irq_num, 0x20);	
	}
}

