#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "x86_desc.h"
#include "types.h"
#include "i8259.h"
#include "multiboot.h"
#include "debug.h"
#include "terminal.h"

#define KEYBORD_PORT 0x60
#define	NUM_LIM 0xd
#define LETTER_DOWN 0x10
#define LETTER_UP 0x39
#define LETTER_OFF 0x10
#define KEY_IRQ 1

#define ESC 0x1
#define ESC_re 0x81

#define BACKSPACE 0xe
#define BACKSPACE_ascii 0x8

#define LSHIFT 0x2a
#define LSHIFT_re 0xaa
#define RSHIFT 0x36
#define RSHIFT_re 0xb6

#define ENTER 0x1c
#define ENTER_ascii 0xa
#define ENTER_re 0x9c

#define CTRL 0x1d
#define CTRL_re 0x9d

#define CAPS 0x3a
#define CAPS_re 0xba

#define UARROW 0x48
#define UARROW_re 0xc8
#define LARROW 0x4b
#define LARROW_re 0xcb
#define RARROW 0x4d
#define RARROW_re 0xcd
#define DARROW 0x50
#define DARROW_re 0xd0

#define LALT 0x38
#define LALT_re 0xb8

#define F1 0x3b
#define F1_re 0xbb
#define F2 0x3c
#define F2_re 0xbc
#define F3 0x3d
#define F3_re 0xbd

#define pressed 1
#define released 0
/**
 * @brief      { keyboard interrupt handler }
 *
 *
 * @return     { None }
 */
extern void keyboard_int_handler();

#endif





