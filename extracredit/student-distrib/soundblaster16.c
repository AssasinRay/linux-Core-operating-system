#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "multiboot.h"
#include "x86_desc.h"
#include "debug.h"
#include "pit.h"
#include "syscall.h"
#define reset_port 0x6 
#define status_port 0xE
#define read_port  0xA
#define write_port 0xC
#define ack_port   0xF
#define base_port  0x220

#define DMA1_maskR 0x0A
#define DMA2_maskR 0xD4
#define DMA1_clear 0x0C
#define DMA2_clear 0xD8
#define DMA1_mode  0x0B
#define DMA2_mode  0xD6
#define DMA1_baseaddr  0x00
#define DMA2_baseaddr  0xC0
#define DMA1_count  0x00
#define DMA2_count  0xC0

#define pagechannel_0  0x87
#define pagechannel_1  0x83
#define pagechannel_2  0x81
#define pagechannel_3  0x82
#define pagechannel_5  0x8B
#define pagechannel_6  0x89
#define pagechannel_7  0x8A

#define countchannel_0  0x01
#define countchannel_1  0x03
#define countchannel_2  0x05
#define countchannel_3  0x07
#define countchannel_4  0xC2
#define countchannel_5  0xC6
#define countchannel_6  0xCA
#define countchannel_7  0xCE
/*2xAh - DSP Read
2xCh - DSP Write (Command/Data), DSP write-buffer status (Bit 7)
2xEh - DSP Read-buffer status (Bit 7), DSP interrupt acknowledge
2xFh - DSP 16-bit interrupt acknowledge*/

void sb_outb(uint8_t data, uint16_t port)
{
	outb(data,port+base_port);
	return;
}
uint16_t sb_inb(uint16_t port)
{
	inb(base_port + port);
	return;
}


int Reset_DSP()
{
	int r=0;
	
	outb(1,reset_port);
	int i =10000;
	while(i>0)
	{
		i--;
	}
	outb(0,reset_port);
	
	while(1)
	{
		r = sb_inb(status_port);
		if(r == 0x7)  break;
	}
	while(1)
	{
		r = sb_inb(read_port);
		if(r== 0xAA)  break;
	}
	return 0;

}
void Write_DSP(uint8_t data)
{
	uint16_t r;
	while(1)
	{
		r= sb_inb(write_port);
		if(r==0x7) break;
	}
	sb_outb(data,write_port);
}
uint16_t Read_DSP()
{
	int r;
	while(1)
	{
		r= sb_inb(status_port);
		if(r==0x7) break;
	}
	return sb_inb(read_port);
}
int DMA_transfer(int channel, uint8_t modebit, uint16_t tranfer_len)
{
	/*int DMA = 0;
	uint16_t linear_addr = 0;
	if(channel>7 || channel <0) return -1;
	if(channel>=0 && channel <=3 ) DMA = 1;
	else                           DMA = 2;
	if(DMA ==1)
	{
		// get linear addr?
		
	uint8_t  maskport_bit1  =0;          // disable sound card
	maskport_bit1 = 4 + channel % 4;
	sb_outb(maskport_bit1, DMA1_maskR);

	sb_outb(0,DMA1_clear);               // clear port
	//uint8_t  mode_bit1 = 0;
	//mode_bit1 = mode_bit1 | 0x40;       //single mode
	//mode_bit1 = mode_bit1 | 0x20;       // address increment
	//mode_bit1 = mode_bit1 | 0x10;      // auto-initialization bit /
	modebit = modebit + channel % 4;  // write DMA mode
	sb_outb(modebit,DMA1_mode);

	uint16_t bufferoffset8 = 0;      //write offset
	bufferoffset8 = linear_addr % 65536; 
	uint8_t lobuff1 = bufferoffset8 & 0x00FF;
	uint8_t hibuff1 = (bufferoffset8 & 0xFF00)>>8;
	sb_outb(lobuff1,DMA1_baseaddr);
	sb_outb(hibuff1,DMA1_baseaddr);

	if(transferlength(channel,tranfer_len) == -1) return -1;//transfer len

	if(write_pageport(channel,linear_addr)==-1) return -1; //pageport

	maskport_bit1 = channel % 4; 
	sb_outb(maskport_bit1, DMA1_maskR);//enable
	}
	if(DMA==2)
	{
	uint8_t  maskport_bit2  =0;
	maskport_bit2 = 4 + channel % 4;
	sb_outb(maskport_bit2, DMA2_maskR); // disable sound card

	sb_outb(0,DMA1_clear);               // clear port 
	//uint8_t  mode_bit2 = 0;
	//mode_bit2 = mode_bit2 | 0x40;        //single mode
	//mode_bit2 = mode_bit2 | 0x20;			//address increment
	//mode_bit2 = mode_bit2 | 0x10;          //auto-initialization bit /
	//modebit = modebit + channel %4 ;     //// write DMA mode
	sb_outb(modebit, DMA2_mode);


	uint16_t bufferoffset16 = 0;      ////write offset
	bufferoffset16 = (linear_addr / 2 )% 65536 ; 
	uint8_t lobuff2 = bufferoffset16 & 0x00FF;
	uint8_t hibuff2 = (bufferoffset16 & 0xFF00)>>8;
	sb_outb(lobuff2,DMA2_baseaddr);
	sb_outb(hibuff2,DMA2_baseaddr);

	if(transferlength(channel,tranfer_len) == -1) return -1; //transfer len

	if(write_pageport(channel,linear_addr)==-1) return -1;//pageport

	maskport_bit2 = channel % 4; 
	sb_outb(maskport_bit2, DMA1_maskR);//enable
	}
	*/
	return 0;
}

int write_pageport(int channel, uint16_t linear_addr)
{
	linear_addr = linear_addr / 65536;
	switch(channel){
	case 0:
	{
		sb_outb(linear_addr,pagechannel_0);
		return 0;
	}
	case 1:
	{
		sb_outb(linear_addr,pagechannel_1);
		return 0;
	}
	case 2:
	{
		sb_outb(linear_addr,pagechannel_2);
		return 0;
	}
	case 3:
	{
		sb_outb(linear_addr,pagechannel_3);
		return 0;
	}
	case 5:
	{
		sb_outb(linear_addr,pagechannel_5);
		return 0;
	}
	case 6:
	{
		sb_outb(linear_addr,pagechannel_6);
		return 0;
	}
	case 7:
	{
		sb_outb(linear_addr,pagechannel_7);
		return 0;
	}
	default:
	{
		return -1;
	}
 }
}
int tranferlength(int channel,uint16_t tranfer_len)
{
	tranfer_len --;
	uint8_t lo_len = (tranfer_len & 0x00FF);
	uint8_t hi_len = (tranfer_len & 0xFF00)>>8;
	switch(channel){
	case 0:
	
		sb_outb(lo_len,countchannel_0);
		sb_outb(hi_len,countchannel_0);
		return 0;
	
	case 1:
	
		sb_outb(lo_len,countchannel_1);
		sb_outb(hi_len,countchannel_1);
		return 0;
	
	case 2:
	
		sb_outb(lo_len,countchannel_2);
		sb_outb(hi_len,countchannel_2);
		return 0;
	
	case 3:
	
		sb_outb(lo_len,countchannel_3);
		sb_outb(hi_len,countchannel_3);
		return 0;
	
	case 4:
	
		sb_outb(lo_len,countchannel_4);
		sb_outb(hi_len,countchannel_4);
		return 0;
	
	case 5:
	
		sb_outb(lo_len,countchannel_5);
		sb_outb(hi_len,countchannel_5);
		return 0;
	
	case 6:
	
		sb_outb(lo_len,countchannel_6);
		sb_outb(hi_len,countchannel_6);
		return 0;
	
	case 7:
	
		sb_outb(lo_len,countchannel_7);
		sb_outb(hi_len,countchannel_7);
		return 0;
	
	default:return -1;}
	
		
	
}



