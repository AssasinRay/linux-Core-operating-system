/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#define MASTER_IRQ_NUM 8
/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask = 0xff; /* IRQs 0-7 */
uint8_t slave_mask = 0xff; /* IRQs 8-15 */

 
/* Initialize the 8259 PIC */
void
i8259_init(void)
{
    /*master_mask = 0xFF;
    slave_mask = 0xFF;*/
	outb(master_mask,MASTER_8259_PORT_2);
	outb(slave_mask,SLAVE_8259_PORT_2);

	/*
	outb Initial a wide range of PC hardware
	*/
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW2_MASTER, MASTER_8259_PORT_2);
	outb(ICW3_MASTER, MASTER_8259_PORT_2);
	outb(ICW4, MASTER_8259_PORT_2);

	outb(ICW1, SLAVE_8259_PORT);
	outb(ICW2_SLAVE, SLAVE_8259_PORT_2);
	outb(ICW3_SLAVE, SLAVE_8259_PORT_2);
	outb(ICW4, SLAVE_8259_PORT_2);


	/*restore master irq mask*/
	/*outb(master_mask, MASTER_8259_PORT_2);
	outb(slave_mask, SLAVE_8259_PORT_2);*/

	printf("the PIC initialized!!\n");
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{

	uint16_t mask;
	if (irq_num < MASTER_IRQ_NUM)		/*if irq is in the master*/
    {
        mask = ~(1 << irq_num);
    	master_mask &= mask;
    	outb(master_mask, MASTER_8259_PORT_2);
    }
    else  					/*if irq is in the slave*/
    {
        mask = ~(1 << (irq_num % MASTER_IRQ_NUM));
    	slave_mask &= mask;
    	outb(slave_mask, SLAVE_8259_PORT_2);
    }         


}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{

	uint16_t mask;

	if (irq_num < MASTER_IRQ_NUM)		//if irq is in the master
    {
        mask = 1 << irq_num;
    	master_mask |= mask;
    	outb(master_mask, MASTER_8259_PORT_2);
    }
    else  					//if irq is in the slave
    {
        mask = 1 << ((irq_num) % MASTER_IRQ_NUM);
    	slave_mask |= mask;
    	outb(slave_mask, SLAVE_8259_PORT_2);
    }         
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{

	if (irq_num >= MASTER_IRQ_NUM)		/*if irq is in the slave*/
    {
    	outb((EOI | (irq_num % MASTER_IRQ_NUM)), SLAVE_8259_PORT);
        outb((EOI | 2), MASTER_8259_PORT);
    }
    else  					/*if irq is in the master*/
    {
    	outb(EOI | irq_num, MASTER_8259_PORT);	
    }   

}

