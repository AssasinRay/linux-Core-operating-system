#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

/** @brief User program starts at virtual address 128MB. */
#define USER_PROGRAM_START 0x08048000

/** @brief User program lives in a 4MB page. */
#define USER_PAGE_SIZE 0x400000

/** @brief User stack starts at end of page. */
#define USER_STACK_START 0x08400000

/** @brief Video memory physical address */
#define VIDEO_MEM_BASE_ADDR 0xB8000

/** @brief Video memory lives in a 4KB page. */
#define VIDEO_PAGE_SIZE 0x1000

/** @brief Virtual video memory physical address */
#define VIRTUAL_VIDEO_MEM_BASE_ADDR 0x0A000000

/**
 * @brief      Initialize paging.
 */
void init_paging();

/**
 * @brief      Switch to specified process' page directory.
 *
 * @param[in]  pid   Process to switch to
 *
 * @return     0 on sucess, -1 if pid is invalid.
 */
int32_t switch_pd(uint32_t pid);

/**
 * @brief      Switch to specified process' terminal.
 *
 * @param[in]  old_tid  Terminal to switch from
 * @param[in]  new_tid  Terminal to switch to
 *
 * @return     0 on sucess, -1 if pid is invalid.
 */
int32_t switch_pt(uint8_t old_tid, uint8_t new_tid);

#endif /* _PAGING_H */
