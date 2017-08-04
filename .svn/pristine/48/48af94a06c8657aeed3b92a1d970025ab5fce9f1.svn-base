#ifndef SOUNDBLASTER16_H
#define SOUNDBLASTER16_H
#include "types.h"
uint16_t sb_inb(uint16_t port);
void sb_outb(uint8_t data, uint16_t port);
int Reset_DSP();
void Write_DSP(uint8_t data);
uint16_t Read_DSP();
int write_pageport(int channel, uint16_t linear_addr);
int DMA_transfer(int channel, uint8_t modebit, uint16_t tranfer_len);
int tranferlength(int channel,uint16_t tranfer_len);


/*Writing to the DSP
Reading from the DSP
Programming the DMA Controller
Setting the sampling rate
Digitized sound I/O
DSP commands
Auto-initialized DMA*/
#endif