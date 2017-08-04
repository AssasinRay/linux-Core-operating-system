#include "scheduler.h"


static uint32_t terminal_in_use_count = 1;
static uint32_t current_sched_process = 0;
static int time=0;
/*
*   sche_init()
*	Function: enable scheduling
*/
void init_sche()
{
/** initialized by init_terminal() **/
	// int i;
	// //initial the termianl structure
	// for(i=0;i<MAX_TERMINAL_NUMBER;i++)  
	// 	{
	// 		terminal_info[i].terminal_index = i;
	// 		terminal_info[i].pid = -1;
	// 		terminal_info[i].video_memory= NULL;
	// 		terminal_info[i].video_memory_base = NULL;
	// 		terminal_info[i].cursor_x = -1;
	// 		terminal_info[i].cursor_y = -1;
	// 	}
}

void scheduling()
{
	time++;
	if (time%100==0)
	{
		set_time();
		time=0;
	}
	//go to the next to-schedule task
	uint32_t esp,ebp;
	uint32_t next_process_to_schedule;
	asm volatile("movl %%esp, %0" : "=r" (esp));
	asm volatile("movl %%ebp, %0" : "=r" (ebp));
	// asm volatile(
	// 	"movl %%esp,%0    \n     \
	// 	movl %%ebp %1"
	// 	:"=r"(esp),"=r"(ebp)
	// 	:
	// 	:"memory"
	// 	);
	if(current_sched_process == terminal_in_use_count-1)	
		next_process_to_schedule = 0;
	else
		next_process_to_schedule = current_sched_process+1;
	//if there are some processes now in the current terminal
	if(terminal_info[current_sched_process].pid != -1 && terminal_info[next_process_to_schedule].pid !=-1 && current_sched_process!=next_process_to_schedule)
	{
		
		pcb_t* current_proc_pcb = get_pcb_info(terminal_info[current_sched_process].pid);
		pcb_t* next_proc_pcb = get_pcb_info(terminal_info[next_process_to_schedule].pid);
		 // printf("curr pid %d ,next process pid %d terminal in use %d\n", current_proc_pcb->pid,next_proc_pcb->pid,terminal_in_use_count);
		//  /*Save current esp ebp inside pcb*/
	    current_process->sche_esp = esp;
	    current_process->sche_ebp = ebp;


		current_process = next_proc_pcb;  //switch the current process

		esp = current_process->sche_esp;
	    ebp = current_process->sche_ebp;

		switch_pd(terminal_info[next_process_to_schedule].pid);//switch page table
		tss.esp0 = kernel_stack_address(current_process->pid);
        tss.ss0 = KERNEL_DS;
        asm volatile("movl %0, %%esp" : : "r" (esp));
	    asm volatile("movl %0, %%ebp" : : "r" (ebp));
		
	}

	current_sched_process = next_process_to_schedule;

	send_eoi(0);
	//    /*Restore next process esp ebp*/
	    
	// asm volatile(
	// 	"movl %0, %%esp \n    \
	// 	movl %1, %%ebp"
	// 	:
	// 	:"r"(esp),"r"(ebp)
	// 	:"memory"
	// 	);

	return;

}

int32_t add_termianl()
{
	if(terminal_in_use_count<=MAX_TERMINAL_NUMBER)
		{
			terminal_in_use_count++;
			return terminal_in_use_count;
		}
	else 
		return -1;
}

