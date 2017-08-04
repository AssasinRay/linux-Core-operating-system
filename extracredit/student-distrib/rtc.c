#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "multiboot.h"
#include "x86_desc.h"
#include "debug.h"

#define RTC_PORT_REG 0x70
#define RTC_PORT_INFO 0x71
#define REG_A 0x8A
#define REG_B 0x8B
#define REG_C 0x8C
#define rtc_irq 8
#define NMI_mask 0x40
#define initial_freq 2
#define freq_upper 1024
#define set_mask 0xF0
#define max_freq 16
volatile  int INT = 0; 
    // time[0] hour time[1] min  time[3] sec
//DEBUG
//static int32_t curr_freq;

/**
 * @brief     initiliazaion rtc function
 */
void initial_rtc()
{
	int temp = inb(RTC_PORT_INFO);	      
	temp = temp | NMI_mask;           //mask all the NMI
	outb(REG_B,RTC_PORT_REG);		
	outb(temp , RTC_PORT_INFO);
	/*Set the frequency to 2Hz*/
	int32_t value = rtc_change_freq(2);  //change to inital freq = 2
	value -- ;
	enable_irq(rtc_irq);       
}


/**
 * @brief      rtc interrupt handler function 
 */
void rtc_int_handle()
{	
	/*For debug and demo*/
	//test_interrupts();
	outb(REG_C,RTC_PORT_REG);        // first write REG_C to portreg 
	inb(RTC_PORT_INFO);				// read the portinfo 
	INT = 1 ;						//set the INT =1
	/*Send end of int signal*/
	send_eoi(rtc_irq);
}

/**
 * @brief       rtc open function.  open the rtc and call initial_rtc
 * 
 *
 * @return      0 
 */
int32_t rtc_open()
{
	initial_rtc();
	return 0;
}

/**
 * @brief       rtc close function just return 0
 *
 * @return      0
 */
int32_t rtc_close()
{
	return 0;
}

/**
 * @brief       rtc_wait function which will be called by rtc read.
 * 
 * this function will wait fot the interrupt
 *
 * @return     0
 */
int32_t rtc_wait()
{
	INT = 0;
	sti();
	/*Wait for next interrupt*/
	while(1)
	{
		if(INT == 1) break;
	}
	cli();
	return 0;
}

/**
 * @brief      system call  rtc read
 *
 * @param[in]  fd      file_descriptor (ignored)
 * @param[in]  buf     buffer          (ignored)
 * @param[in]  nbytes  nbytes          (ignored)
 *
 * @return     0
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
	rtc_wait();
	return 0;
}

/**
 * @brief      change the frequency of rtc
 *
 * @param[in]  freq   the frequency we need
 *
 * @return     0 if succeed     -1 if failed
 */
int32_t rtc_change_freq(int32_t freq)
{
	if(freq > freq_upper || freq < initial_freq)  //check the frequency range
	{
		printf(" freq is out of range" );
		return -1;
	}
	if(check_power(freq) == 0)                   //check if is power of 2
	{
		printf(" freq is not power of 2");
		return -1;
	}
	
	int32_t rate = transfer_to_rate(freq);      //transfer the frequency to rate
	 
	cli();
	int temp = inb(RTC_PORT_INFO);             //set ports
	temp = (temp & set_mask) | rate;
	outb(REG_A,RTC_PORT_REG);
	outb(temp,RTC_PORT_INFO);

//DEBUG
//curr_freq = freq;

	sti();
	
	return 0;

}

/**
 * @brief      system call rtc_write
 *
 * @param[in]  fd      file_descriptor (ignored)
 * @param[in]  buf     buffer          (ignored)
 * @param[in]  nbytes  nbytes          (ignored)
 *
 * @return     the rate of rtc
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
	if(buf==NULL)  return -1;
	int32_t freq = *((int32_t*)buf);          //get the freq and call rtc_change_freq
	
	int32_t value = rtc_change_freq(freq);
	value--;
	return  0;

}


/**
 * @brief     transfer the frequency to the rate
 *
 * @param[in]  freq  frequency we need 
 *
 * @return     rate 
 */
int32_t transfer_to_rate(int32_t freq)
{
	int32_t temp = 0;                       //log2 of freq
	while(freq != 1)
	{
		freq = freq / 2;
		temp++;
	}
	return max_freq - temp ;
}

/**
 * @brief      check if it is power of 2 
 *
 * @param[in]  x    frequency
 *
 * @return     1 it is power of 2  0 it is not
 */
int check_power(int32_t x)              
{
	 return ((x != 0) && !(x & (x - 1)));  
}


void set_time()
{
	time[2]++;
	if(time[2] == 60)
	{
		time[2] = 0;
		time[1]++;
		if(time[1] == 60)
		{
			time[1] = 0;
			time[0]++;
			if(time[0] == 24)
			{
				time[0]=0;
				time[1]=0;
				time[2]=0;
			}
		}
	}
	int h1=time[0]/10;
	int h2=time[0]%10;
	int m1=time[1]/10;
	int m2=time[1]%10;
	int s1=time[2]/10;
	int s2=time[2]%10;
	// printf("%d%d:%d%d:%d%d",h1,h2,m1,m2,s1,s2);
}
void set_timesyscall(uint8_t *buff)
{

}
