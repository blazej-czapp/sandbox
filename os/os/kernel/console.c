#include "console.h"
#include "../drivers/io.h"

#define VIDEO_MEMORY (unsigned char*)0xb8000
#define COLS 80
#define ROWS 25

// address into BIOS Data Area storing the base IO port for video 
static unsigned short* BDA_IO_vid_port = (unsigned short*)0x0463;

void memory_copy(char* source, char* dest, int no_bytes) {
	for (int i = 0; i < no_bytes; ++i) {
		*dest++ = *dest++;
	}
}

int string_length(const char* text) {
	int len = 0;

	while (*text != 0) {
		++len;
		++text;
	}

	return len;
}

char* vid_address(int row, int col) {
	return VIDEO_MEMORY + ((row * COLS) + col) * 2;
}

void put_char(char val, char* address, char color) {
	*address = val;
	*(address + 1) = color;
}

void clear_row(int row) {
	for (int j = 0; j < COLS; ++j) {
		put_char(*" ", vid_address(row, j), WHITE_ON_BLACK);
	}
}

void clear_screen() {
	for (int i = 0; i < ROWS; ++i) {
		clear_row(i);
	}
}

// scrolls screen one line up
void scroll_screen() {
	for (int i = 1; i < ROWS; ++i) {
		memory_copy(VIDEO_MEMORY + i * COLS * 2,
					VIDEO_MEMORY + (i - 1) * COLS * 2,
					COLS);
	}

	clear_row(ROWS - 1);
}

void print_at(const char *text, int row, int col, char color, int update_cursor) {
	int index = 0;
	int len = string_length(text);

	// <= rather than < so that we correctly insert the cursor after the text
	while (index <= len) {
		if (col >= COLS) {
			col = 0;
			++row;
		}

		if (row == ROWS) {
			//scroll_screen();
			row = 0;
		} // if row > ROWS we're in trouble

		if (index == len) {
			// done printing text, this is preparing for advancing the cursor, so break here
			break;
		}

		put_char(text[index], vid_address(row, col), color);

		++col;
		++index;
	}

	if (update_cursor) {
		move_cursor(row, col);
	}
}

void move_cursor(int row, int col) {
	unsigned short IO_vid_port = *BDA_IO_vid_port;

    unsigned short pos = row * COLS + col;
 
 	// select VGA internal register 15, i.e. the low byte of the position
    io_outb(IO_vid_port, 15);
    // write the low bits of the position
    io_outb(IO_vid_port + 1, (unsigned char)(pos & 0xff));
    // select VGA internal register 14, i.e. the high byte of the position
    io_outb(IO_vid_port, 14);
    // write the high bits of the position
    io_outb(IO_vid_port + 1, (unsigned char)((pos >> 8) & 0xff));
 }