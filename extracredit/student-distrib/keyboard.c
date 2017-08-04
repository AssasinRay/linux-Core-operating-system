#include "keyboard.h"


#define Q 0x10
#define P 0x19
#define A 0x1e
#define L 0x26
#define Z 0x2c
#define M 0x32

static uint8_t shift;
static uint8_t caps;
static uint8_t ctrl;
static uint8_t alt;

/*The font data for mapping scancode to symbols*/
static uint8_t font_data_num[14] = {'\0', '\0', '1', '2', '3','4','5','6','7','8','9','0', '-', '='};
static uint8_t font_data_letter[42] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\r', '\0',
										 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\',
										 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' '};
static uint8_t font_data_num_shift[14] = {'\0', '\0', '!', '@', '#','$','%','^','&','*','(',')', '_', '+'};
static uint8_t font_data_letter_shift[42] = {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\r', '\0',
										 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', '\0', '|',
										 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '\?', '\0', '*', '\0', ' '};
void keyboard_int_handler()
{
	/*Variables for take in and output the keyboard signal*/
	uint8_t scancode;
	uint8_t output;

	/*send_eoi(1);*/
	/*Read in the scancode from the port*/
	scancode = inb(KEYBORD_PORT);

	switch(scancode)
	{
		case LALT:
			alt = pressed;
			break;
		case LALT_re:
			alt = released;
			break;
		case LSHIFT:
		case RSHIFT:
			shift = pressed;
			break;
		case LSHIFT_re:
		case RSHIFT_re:
			shift = released;
			break;
		case CAPS:
			caps ^= 1;
			break;
		case CTRL:
			ctrl = pressed;
			break;
		case CTRL_re:
			ctrl = released;
			break;
		case BACKSPACE:
			input_char_terminal(BACKSPACE_ascii);
			break;
		case ENTER:
			input_char_terminal(ENTER_ascii);
			break;
		default:
			if (scancode <= NUM_LIM)
			{
				if (!shift && !caps && !ctrl && !alt)
				{
					output = font_data_num[scancode];
					input_char_terminal(output);
					break;
				}	
				else if (shift && !caps && !ctrl && !alt)
				{
					output = font_data_num_shift[scancode];
					input_char_terminal(output);
					break;
				} 
				else if (!shift && caps && !ctrl && !alt)
				{
					output = font_data_num[scancode];
					input_char_terminal(output);
					break;
				}
				else if (shift && caps && !ctrl && !alt)
				{
					output = font_data_num_shift[scancode];
					input_char_terminal(output);
					break;
				}
			}
			else if (scancode <= LETTER_UP && scancode >= LETTER_DOWN)
			{
				if ((scancode == L) && (ctrl))
				{
					clear_terminal();
					break;
				}
				else if (!shift && !caps && !ctrl && !alt)
				{
					output = font_data_letter[scancode - LETTER_OFF];
					input_char_terminal(output);
					break;
				}	
				else if (shift && !caps && !ctrl && !alt)
				{
					output = font_data_letter_shift[scancode - LETTER_OFF];
					input_char_terminal(output);
					break;
				}
				else if (!shift && caps && !ctrl && !alt)
				{
					if ((scancode >= Q && scancode <= P) || 
						(scancode >= A && scancode <= L) ||
						(scancode >= Z && scancode <= M))
					{
						output = font_data_letter_shift[scancode - LETTER_OFF];
						input_char_terminal(output);
						break;
					}
					else
					{
						output = font_data_letter[scancode - LETTER_OFF];
						input_char_terminal(output);
						break;
					}					
				}
				else if (shift && caps && !ctrl && !alt)
				{
					if ((scancode >= Q && scancode <= P) || 
						(scancode >= A && scancode <= L) ||
						(scancode >= Z && scancode <= M))
					{
						output = font_data_letter[scancode - LETTER_OFF];
						input_char_terminal(output);
						break;
					}
					else
					{
						output = font_data_letter_shift[scancode - LETTER_OFF];
						input_char_terminal(output);
						break;
					}		
				}
			}
			else if (scancode >= LETTER_UP)
			{
				switch(scancode)
				{
					case F1:
						if (alt && !shift && !ctrl && !caps)
						{
							switch_terminal(0);
							break;
						}
					case F2:
						if (alt && !shift && !ctrl && !caps)
						{
							switch_terminal(1);
							break;
						}
					case F3:
						if (alt && !shift && !ctrl && !caps)
						{
							switch_terminal(2);
							break;
						}
				}
			}

	}
	/*if (scancode == LSHIFT || scancode == RSHIFT)
		shift = pressed;
	else if (scancode == LSHIFT_re || scancode == RSHIFT_re)
		shift = released;
	else if (scancode == CAPS){
		caps ^= 1;
		printf("CAPS: %d\n",caps);
	}

	else if (scancode == CTRL)
		ctrl = pressed;
	else if (scancode == CTRL_re)
		ctrl = released;
	else if (scancode == BACKSPACE)
		input_char_terminal(BACKSPACE);
	else if (scancode == ENTER)
		input_char_terminal(ENTER);
	else if (scancode <= NUM_LIM
			&& scancode != ESC
			&& scancode != LSHIFT
			&& scancode != RSHIFT
			&& scancode != CTRL
			&& scancode != BACKSPACE
			&& scancode != CAPS)
			{
				if (!shift && !caps && !ctrl)
				{
					output = font_data_num[scancode];
					input_char_terminal(output);
				}	
				else if (shift && !caps && !ctrl)
				{
					output = font_data_num_shift[scancode];
					input_char_terminal(output);
				} 
				else if (!shift && caps && !ctrl)
				{
					output = font_data_num[scancode];
					input_char_terminal(output);
				}
				else if (shift && caps && !ctrl)
				{
					output = font_data_num_shift[scancode];
					input_char_terminal(output);
				}
			}
	else if (scancode <= LETTER_UP && scancode >= LETTER_DOWN
			&& scancode != ESC
			&& scancode != LSHIFT
			&& scancode != RSHIFT
			&& scancode != CTRL
			&& scancode != BACKSPACE
			&& scancode != CAPS)
			{
				if ((scancode == L) && (ctrl))
				{
					clear_terminal();
				}
				else if (!shift && !caps && !ctrl)
				{
					output = font_data_letter[scancode - LETTER_OFF];
					input_char_terminal(output);
				}	
				else if (shift && !caps && !ctrl)
				{
					output = font_data_letter_shift[scancode - LETTER_OFF];
					input_char_terminal(output);
				}
				else if (!shift && caps && !ctrl)
				{
					if ((scancode >= Q && scancode <= P) || 
						(scancode >= A && scancode <= L) ||
						(scancode >= Z && scancode <= M))
					{
						output = font_data_letter_shift[scancode - LETTER_OFF];
						input_char_terminal(output);
					}
					else
					{
						output = font_data_letter[scancode - LETTER_OFF];
						input_char_terminal(output);
					}					
				}
				else if (shift && caps && !ctrl)
				{
					output = font_data_letter_shift[scancode - LETTER_OFF];
					input_char_terminal(output);
				}
			}
	else
	{
		send_eoi(KEY_IRQ);
		return;
	}*/
	

	send_eoi(KEY_IRQ);

	return;
}








