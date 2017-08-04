#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "terminal.h"
#include "types.h"
#include "syscall.h"
#include "paging.h"
#include "process.h"
#include "i8259.h"

void init_sche();
void scheduling();
int32_t add_termianl();

#endif







