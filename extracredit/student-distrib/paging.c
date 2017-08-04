#include "paging.h"

#include "x86_desc.h"
#include "process.h"
#include "lib.h"
#include "terminal.h"

/** @brief Kernel starts at physical address 4MB. */
#define KERNEL_BASE_ADDR 0x400000

/** @brief User program starts at physical address 8MB. */
#define USER_BASE_ADDR 0x800000

/** @brief Bitmask to clear first 20 bits of CR3 */
#define CR3_MASK 0x00000FFF

/** Number of page-directory entries in the page directory (PD) */
#define NUM_PDE 1024

/** 
 * @brief      Bitmask of 4MB supervisor page directory entry.
 * 
 * \n Bit 0 : 1 - present flag
 * \n Bit 1 : 1 - read/write flag
 * \n Bit 2 : 0 - user/supervisor flag
 * \n Bit 7 : 1 - page size flag
 */
#define PDE_4MB_SUPERVISOR_MASK 0x83

/** 
 * @brief      Bitmask of 4MB user page directory entry.
 * 
 * \n Bit 0 : 1 - present flag
 * \n Bit 1 : 1 - read/write flag
 * \n Bit 2 : 1 - user/supervisor flag
 * \n Bit 7 : 1 - page size flag
 */
#define PDE_4MB_USER_MASK 0x87

/**
 * @brief      Bitmask of 4KB supervisor page directory entry.
 * 
 * \n Bit 0 : 1 - present flag
 * \n Bit 1 : 1 - read/write flag
 * \n Bit 2 : 0 - user/supervisor fla
 * g
 * \n Bit 7 : 0 - page size flag
 */
#define PDE_4KB_SUPERVISOR_MASK 0x3

/**
 * @brief      Bitmask of 4KB supervisor page table entry.
 * 
 * \n Bit 0 : 1 - present flag
 * \n Bit 1 : 1 - read/write flag
 * \n Bit 2 : 0 - user/supervisor flag
 */
#define PTE_4KB_SUPERVISOR_MASK 0x3
 /**
 * @brief      Bitmask of 4KB user page directory entry.
 * 
 * \n Bit 0 : 1 - present flag
 * \n Bit 1 : 1 - read/write flag
 * \n Bit 2 : 1 - user/supervisor flag
 * \n Bit 7 : 0 - page size flag
 */
#define PDE_4KB_USER_MASK 0x7

/**
 * @brief      Bitmask of 4KB user page table entry.
 * 
 * \n Bit 0 : 1 - present flag
 * \n Bit 1 : 1 - read/write flag
 * \n Bit 2 : 1 - user/supervisor flag
 */
#define PTE_4KB_USER_MASK 0x7


static uint32_t pd0[NUM_PDE] __attribute__((aligned(4096)));
static uint32_t pd1[NUM_PDE] __attribute__((aligned(4096)));
static uint32_t pd2[NUM_PDE] __attribute__((aligned(4096)));
static uint32_t pd3[NUM_PDE] __attribute__((aligned(4096)));
static uint32_t pd4[NUM_PDE] __attribute__((aligned(4096)));
static uint32_t pd5[NUM_PDE] __attribute__((aligned(4096)));

static uint32_t video_pt0[NUM_PTE] __attribute__((aligned(4096)));
static uint32_t video_pt1[NUM_PTE] __attribute__((aligned(4096)));
static uint32_t video_pt2[NUM_PTE] __attribute__((aligned(4096)));
static uint32_t video_pt3[NUM_PTE] __attribute__((aligned(4096)));
static uint32_t video_pt4[NUM_PTE] __attribute__((aligned(4096)));
static uint32_t video_pt5[NUM_PTE] __attribute__((aligned(4096)));

static uint32_t* pd[6];

static uint32_t* video_pt[6];

void init_paging() {
	uint32_t i, j;
	uint32_t cr0val, cr3val, cr4val;

	pd[0] = pd0;
	pd[1] = pd1;
	pd[2] = pd2;
	pd[3] = pd3;
	pd[4] = pd4;
	pd[5] = pd5;

	video_pt[0] = video_pt0;
	video_pt[1] = video_pt1;
	video_pt[2] = video_pt2;
	video_pt[3] = video_pt3;
	video_pt[4] = video_pt4;
	video_pt[5] = video_pt5;

	for (i = 0; i < 6; i++) {
		for (j = 0; j < NUM_PDE; j++) {
			pd[i][j] = 0x0;
		}
	}

	// set PTE for physical video memory
	pt[VIDEO_MEM_BASE_ADDR / 4096] = VIDEO_MEM_BASE_ADDR | PTE_4KB_SUPERVISOR_MASK;

	// set PTE for terminal buffer
	pt[VIDEO_MEM_BASE_ADDR / 4096 + 1] = (VIDEO_MEM_BASE_ADDR + 4096 * 1) | PTE_4KB_SUPERVISOR_MASK;
	pt[VIDEO_MEM_BASE_ADDR / 4096 + 2] = (VIDEO_MEM_BASE_ADDR + 4096 * 2) | PTE_4KB_SUPERVISOR_MASK;
	pt[VIDEO_MEM_BASE_ADDR / 4096 + 3] = (VIDEO_MEM_BASE_ADDR + 4096 * 3) | PTE_4KB_SUPERVISOR_MASK;

	for (i = 0; i < 6; i++) {
		// set PDE for physical video memory
		pd[i][0] = (uint32_t)pt | PDE_4KB_SUPERVISOR_MASK;

		// set PDE for kernel
		pd[i][1] = KERNEL_BASE_ADDR | PDE_4MB_SUPERVISOR_MASK;

		// set PDE for user program
		pd[i][32] = (USER_BASE_ADDR + i * USER_PAGE_SIZE) | PDE_4MB_USER_MASK;

		// set PTE for virtual video memory
		video_pt[i][0] = VIDEO_MEM_BASE_ADDR | PTE_4KB_USER_MASK;

		// set PDE for virtual video memory
		pd[i][VIRTUAL_VIDEO_MEM_BASE_ADDR / USER_PAGE_SIZE] = (uint32_t)video_pt[i] | PDE_4KB_USER_MASK;
	}

	/* enable paging */

	// set CR3 to be page directory base register
	asm volatile("movl %%cr3, %0" : "=r" (cr3val));
	cr3val = (cr3val & CR3_MASK) | (uint32_t)pd[0];
	asm volatile("movl %0, %%cr3" : /* no outputs */ : "r" (cr3val));

	// set PSE flag in CR4, bit 4
	asm volatile("movl %%cr4, %0" : "=r" (cr4val));
	cr4val |= (1 << 4);
	asm volatile("movl %0, %%cr4" : /* no outputs */ : "r" (cr4val));

	// set PG flag in CR0, bit 31
	asm volatile("movl %%cr0, %0" : "=r" (cr0val));
	cr0val |= (1 << 31);
	asm volatile("movl %0, %%cr0" : /* no outputs */ : "r" (cr0val));
}

int32_t switch_pd(uint32_t pid) {
	uint32_t cr3val;

	if (pid > MAX_PROCESS) {
		return -1;
	}

	cli();

	// set CR3 to be page directory base register
	asm volatile("movl %%cr3, %0" : "=r" (cr3val));
	cr3val = (cr3val & CR3_MASK) | (uint32_t)pd[pid];
	asm volatile("movl %0, %%cr3" : /* no outputs */ : "r" (cr3val));

	sti();

	return 0;
}

int32_t switch_pt(uint8_t old_tid, uint8_t new_tid) {
	if (old_tid != new_tid) {
		video_pt[old_tid][0] = (uint32_t)terminal_buffer(old_tid) | PTE_4KB_USER_MASK;
		video_pt[new_tid][0] = VIDEO_MEM_BASE_ADDR | PTE_4KB_USER_MASK;

		return 0;		
	} else {
		return -1;
	}

}
