#include "pit.h"
#include "i8259.h"
#include "syscall.h"
#include "lib.h"
#define  base_frq      1193180
#define  channel_zero  0x40
#define  mask_8        0xFF
#define  eight         8
#define  command_port  0x43
#define  command_byte  0x36
#define  sound_port    0x61
#define  channel_two   0x42
#define  sound_mask    0xFC;
#define  command_mask  0xb6;



/**
 * @brief      initlize pit
 *
 * @param[in]  frequency   frequency of pit
 *
 * @return     nothing.
 */
void init_pit(int32_t frequency)  //source:http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html
{
	enable_irq(0);
	int divisor = base_frq / frequency;

   	outb(command_byte, command_port);

  
   uint8_t l = (uint8_t)(divisor & mask_8);
   uint8_t h = (uint8_t)((divisor>>eight) & mask_8 );

  
   outb(l, channel_zero);
   outb(h, channel_zero);

}
void pit_interrput()
{
	init_pit(100);
}
//Play sound using built in speaker 
//source:http://wiki.osdev.org/PC_Speaker
/**
 * @brief      play sound
 *
 * @param[in]  frequency   frequency of sound
 *
 * @return     nothing.
 */

void play_sound(uint32_t nFrequence)
 { 
 /*	uint32_t Div;
 	uint8_t tmp;
=======
// void play_sound(uint32_t nFrequence)
//  { 
//  	uint32_t Div;
//  	uint8_t tmp;
>>>>>>> .r18473
 
//         //Set the PIT to the desired frequency
//  	Div = base_frq / nFrequence;
//  	//outb(0x43, 0xb6);
//  	outb(command_mask,command_port);
//  	//outb(0x42, (uint8_t) (Div) );
//  	outb((uint8_t) (Div),channel_two );
//  	//outb(0x42, (uint8_t) (Div >> 8));
//  	outb((uint8_t) (Div >> 8),channel_two );
 
<<<<<<< .mine
        //And play the sound using the PC speaker
 	tmp = inb(sound_port);
  	if (tmp != (tmp | 3)) {
 		outb(tmp | 3,sound_port);
 	}*/
 }

//         //And play the sound using the PC speaker
//  	tmp = inb(sound_port);
//   	if (tmp != (tmp | 3)) {
//  		outb(tmp | 3,sound_port);
//  	}
//  }

 
 /**
 * @brief      stop sound
 *
 * @param[in]  none
 *
 * @return     nothing.
 */
void nosound() {
 	uint8_t tmp = inb(sound_port) & sound_mask;
 
 	outb(tmp,sound_port);
 }
 
