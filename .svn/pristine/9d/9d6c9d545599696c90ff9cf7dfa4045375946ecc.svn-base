#include "terminal.h"
#include "keyboard.h"
#include "paging.h"
#include "scheduler.h"
#include "ece391syscall.h"
#include "process.h"

/**
 * @brief Video memory starting address
 * 
 * Copied from lib.c
 */
#define VIDEO 0xB8000

/**
 * @brief Number of terminal columns
 * 
 * Copied from lib.c
 */
#define NUM_COLS 80

/**
 * @brief Number of terminal rows
 * 
 * Copied from lib.c
 */
#define NUM_ROWS 25

/**
 * @brief Video memory attribute
 * 
 * Copied from lib.c
 */
#define ATTRIB 0x7

/**
 * @brief Pointer to video memory
 * 
 * Copied from lib.c
 */
//static char* video_mem = (char*)VIRTUAL_VIDEO_MEM_BASE_ADDR; //(char *)VIDEO;

// static uint32_t terminal_buffer[MAX_TERMINAL_NUMBER][VIDEO_PAGE_SIZE];

/** @brief Terminal input buffer */
//static uint8_t input_buffer[TERMINAL_INPUT_BUFFER_SIZE];

/** @brief Cursor x coordinate */
//int32_t cursor_x = 0;

/** @brief Cursor y coordinate */
//int32_t cursor_y = 0;

/** @brief Top of input buffer */
//uint32_t buffer_top = 0;

/**
 * @brief Flag to indicate wether enter is pressed.
 * 
 * 1 if pressed, 0 if not pressed. 
 * Used to sync between read_termianl() and input_char_terminal().
 */
//volatile int32_t enter_pressed = 0;

/**
 * @brief Flag to indicate wether terminal is being read.
 * 
 * 1 if being read, 0 if not.
 * Used to sync between read_termianl() and input_char_terminal().
 */
//volatile int32_t reading_terminal = 0;


/* Function declarations */
/**
 * @brief      Get pointer to video memory
 *
 * @param[in]  tid   Which termianl to get
 *
 * @return     A pointer to video memory
 */
char* video_mem(uint8_t tid);

/**
 * @brief      Push a character onto input buffer.
 *
 * @param[in]  character  The character to be pushed
 *
 * @return     0 on success, -1 on buffer overflow.
 */
int32_t buffer_push(uint8_t character, uint8_t tid);

/**
 * @brief      Pop a character from input buffer.
 *
 * @return     The character being poped. NUL (0x0) on buffer underflow.
 */
uint8_t buffer_pop(uint8_t tid);

/**
 * @brief      Scroll the screen.
 *
 * @param[in]  tid   The terminal to scroll
 */
void scroll(uint8_t tid);

/**
 * @brief      Print a character on screen.
 *
 * @param[in]  character  Character to be print
 * @param[in]  tid        The terminal to print
 *
 * @return     0 on success, -1 on failure
 */
int32_t print_char(uint8_t character, uint8_t tid);

/**
 * @brief      Draw a character at specified location.
 *
 * @param[in]  character  The character to be drawn
 * @param[in]  pos_x      X coordinate of the character
 * @param[in]  pos_y      Y coordinate of the character
 * @param[in]  tid        The terminal to draw
 *
 * @see        putc()
 */
void draw_char(uint8_t character, uint32_t pos_x, uint32_t pos_y, uint8_t tid);


/**
 * @brief      Move cursor to specified position.
 *
 *             Modified from http://wiki.osdev.org/Text_Mode_Cursor
 *
 * @param[in]  row   New y coordinate of cursor
 * @param[in]  col   New y coordinate of cursor
 * @param[in]  tid   The terminal to update
 */
void update_cursor(uint32_t row, uint32_t col, uint8_t tid);

/* Function definitions */


void init_terminal() {
    int i;

    current_tid = 0;
    for (i = 0; i < MAX_TERMINAL_NUMBER; i++) {
        terminal_info[i].pid = -1;
        terminal_info[i].cursor_x = 0;
        terminal_info[i].cursor_y = 0;
        terminal_info[i].buffer_top = 0;
        terminal_info[i].enter_pressed = 0;
        terminal_info[i].reading_terminal = 0;
    }
    clear_terminal();
    
    // //initial the termianl structure
    // for(i=0;i<MAX_TERMINAL_NUMBER;i++)  
    //     {
    //         terminal_info[i].terminal_index = i;
    //         terminal_info[i].pid = -1;
    //         terminal_info[i].video_memory= NULL;
    //         terminal_info[i].video_memory_base = NULL;
    //         terminal_info[i].cursor_x = -1;
    //         terminal_info[i].cursor_y = -1;
    //     }
}

void switch_terminal(uint8_t tid) {
    
    uint32_t* video_page = (uint32_t*)VIDEO;
    uint32_t i;

    if (tid != current_tid) {
        for (i = 0; i < 1024; i++) {
            // copy current video memory to buffer
            terminal_buffer(current_tid)[i] = video_page[i];
            // copy new buffer to video memory
            video_page[i] = terminal_buffer(tid)[i];
        }

        // switch video page table
        switch_pt(current_tid, tid);
        
        // update current_tid
        current_tid = tid;

        // update cursor
        update_cursor(terminal_info[current_tid].cursor_y, terminal_info[current_tid].cursor_x, current_tid);

        // start a new shell if current terminal is empty
/*        if (terminal_info[current_tid].pid < 0) {
            //add_termianl();
            current_process = NULL;
            ece391_execute((const uint8_t*)"shell");
        }*/
    } 
    return;
}

int32_t read_terminal(int32_t fd, void* buf, int32_t nbytes) {
    uint32_t i;
    int32_t bytes_to_read;
    terminal_t* current_terminal = &terminal_info[current_process->tid];

    // we need to disable interrupts whenever we update sync variables
    cli();

    current_terminal->enter_pressed = 0;
    current_terminal->reading_terminal = 1;

    sti();
    
    // wait until enter is pressed by user
    while (current_terminal->enter_pressed != 1) {}

    cli();

    current_terminal->enter_pressed = 0;
    current_terminal->reading_terminal = 0;

    bytes_to_read = (nbytes < TERMINAL_INPUT_BUFFER_SIZE) ? nbytes : TERMINAL_INPUT_BUFFER_SIZE;
    bytes_to_read = (bytes_to_read < current_terminal->buffer_top) ? bytes_to_read : current_terminal->buffer_top;

    for (i = 0; i < bytes_to_read; i++) {
        ((uint8_t*)buf)[i] = current_terminal->input_buffer[i];
    }

    // add LF to the end
    ((uint8_t*)buf)[bytes_to_read] = ENTER_ascii;

    // terminate string if nbytes > 
    if (bytes_to_read < nbytes) {
        ((uint8_t*)buf)[bytes_to_read + 1] = '\0';
    }

    current_terminal->buffer_top = 0;

    sti();

    return bytes_to_read + 1;
}

int32_t write_terminal(int32_t fd, const void* buf, int32_t nbytes) {
    int32_t bytes_written = 0;

    cli();

    while (bytes_written < nbytes) {
        if (((uint8_t*)buf)[bytes_written] == 0xFF) {
            sti();
            return bytes_written;
        } else {
            print_char(((uint8_t*)buf)[bytes_written], current_process->tid);
            bytes_written++;
        }
    }

    // should never reach here
    sti();
    return bytes_written;
}

int32_t open_terminal(const uint8_t* filename) {   
    return 0;
}

int32_t close_terminal(int32_t fd) {
    return 0;
}

void clear_terminal() {
    terminal_t* current_terminal = &terminal_info[current_tid];

    clear();
    current_terminal->cursor_x = 0;
    current_terminal->cursor_y = 0;
    current_terminal->buffer_top = 0;
    current_terminal->enter_pressed = 0;
    current_terminal->reading_terminal = 0;
    update_cursor(0, 0, current_tid);
}

int32_t input_char_terminal(uint8_t character) {
    terminal_t* current_terminal = &terminal_info[current_tid];

    switch (character) {
        case BACKSPACE_ascii:
            if (current_terminal->buffer_top == 0) {
                return -1;
            }

            current_terminal->cursor_x--;
            if (current_terminal->cursor_x < 0) {
                current_terminal->cursor_x = NUM_COLS - 1;
                current_terminal->cursor_y--;
                if (current_terminal->cursor_y < 0) {
                    current_terminal->cursor_x = 0;
                    current_terminal->cursor_y = 0;
                }
            }
            buffer_pop(current_tid);
            draw_char(' ', current_terminal->cursor_x, current_terminal->cursor_y, current_tid);
            update_cursor(current_terminal->cursor_y, current_terminal->cursor_x, current_tid);
            return 0;

        case ENTER_ascii:
            current_terminal->cursor_x = 0;
            current_terminal->cursor_y++;
            if (current_terminal->cursor_y >= NUM_ROWS) {
                scroll(current_tid);
                current_terminal->cursor_y = NUM_ROWS - 1;
            }

            if (current_terminal->reading_terminal == 1) {
                current_terminal->enter_pressed = 1;
            } else {
                current_terminal->buffer_top = 0;
            }

            update_cursor(current_terminal->cursor_y, current_terminal->cursor_x, current_tid);

            return 0;

        default:
            if (buffer_push(character, current_tid) == -1) {
                return -1;
            }
            return print_char(current_terminal->input_buffer[current_terminal->buffer_top - 1], current_tid);
    }
    
}

int32_t print_char(uint8_t character, uint8_t tid) {
    terminal_t* current_terminal = &terminal_info[tid];

    switch (character) {
        case '\n':
        case '\r':
            current_terminal->cursor_x = 0;
            current_terminal->cursor_y++;
            if (current_terminal->cursor_y >= NUM_ROWS) {
                scroll(tid);
                current_terminal->cursor_y = NUM_ROWS - 1;
            }

            update_cursor(current_terminal->cursor_y, current_terminal->cursor_x, tid);

            break;
            
        default:
            if ((current_terminal->cursor_y == NUM_ROWS - 1) && (current_terminal->cursor_x == NUM_COLS - 1)) {
                draw_char(character, current_terminal->cursor_x, current_terminal->cursor_y, tid);
                scroll(tid);
                current_terminal->cursor_x = 0;
            } else {
                if (current_terminal->cursor_x >= NUM_COLS) {
                    current_terminal->cursor_x = 0;
                    current_terminal->cursor_y++;
                    if (current_terminal->cursor_y >= NUM_ROWS) {
                        scroll(tid);
                        current_terminal->cursor_y = NUM_ROWS - 1;
                    }
                }
                draw_char(character, current_terminal->cursor_x, current_terminal->cursor_y, tid);
                current_terminal->cursor_x++;
            }
            update_cursor(current_terminal->cursor_y, current_terminal->cursor_x, tid);
    }

    return 0;
}

void scroll(uint8_t tid) {
    uint32_t i;

    for (i = 0; i < 2 * (NUM_ROWS - 1) * NUM_COLS; i++) {
        *(uint8_t *)(video_mem(tid) + i) = *(uint8_t *)(video_mem(tid) + i + 2 * NUM_COLS);
    }
    for (i = 0; i < NUM_COLS; i++) {
        draw_char(' ', i, NUM_ROWS - 1, tid);
    }
}

int32_t buffer_push(uint8_t character, uint8_t tid) {
    terminal_t* current_terminal = &terminal_info[tid];

    if (current_terminal->buffer_top >= TERMINAL_INPUT_BUFFER_SIZE) {
        return -1; // buffer overflow
    } else {
        // push char onto input buffer
        current_terminal->input_buffer[current_terminal->buffer_top] = character;
        current_terminal->buffer_top++;
        return 0;
    }
}

uint8_t buffer_pop(uint8_t tid) {
    terminal_t* current_terminal = &terminal_info[tid];

    if (current_terminal->buffer_top == 0) {
        return 0; // buffer underflow
    } else {
        current_terminal->buffer_top--;
        return current_terminal->input_buffer[current_terminal->buffer_top];
    }
}

void draw_char(uint8_t character, uint32_t pos_x, uint32_t pos_y, uint8_t tid) {
    // copied from putc()
    *(uint8_t *)(video_mem(tid) + ((NUM_COLS*pos_y + pos_x) << 1)) = character;
    *(uint8_t *)(video_mem(tid) + ((NUM_COLS*pos_y + pos_x) << 1) + 1) = ATTRIB;
}

void update_cursor(uint32_t row, uint32_t col, uint8_t tid) {
    if (tid == current_tid) {
        if ((row > NUM_ROWS) || (col > NUM_COLS)) {
            return;
        }

        uint16_t position = (row * 80) + col;
     
        // cursor LOW port to vga INDEX register
        outb(0x0F, 0x3D4);
        outb((uint8_t)(position & 0xFF), 0x3D5);
        // cursor HIGH port to vga INDEX register
        outb(0x0E, 0x3D4);
        outb((uint8_t)((position >> 8) & 0xFF), 0x3D5);        
    }
}

uint32_t* terminal_buffer(uint8_t tid) {
    return (uint32_t*)(VIDEO + (tid + 1) * VIDEO_PAGE_SIZE);
}

char* video_mem(uint8_t tid) {
    return (char*)(tid == current_tid ? VIDEO : (uint32_t)terminal_buffer(tid));
}
