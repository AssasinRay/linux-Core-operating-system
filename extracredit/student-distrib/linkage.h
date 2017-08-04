#ifndef _LINKAGE_H
#define _LINKAGE_H
#include "rtc.h"
#include "keyboard.h"

#include "mouse.h"

#include "scheduler.h"


/*The linkage function for the keyboard*/
extern void keyboard_link();
/*The linkage function for the rtc*/
extern void rtc_link();

/*The linkage function for the mouse*/
extern void mouse_link();

/*The linkage function for scheduler*/
extern void sched_link();


#endif



