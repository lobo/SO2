#ifndef DRIVER_VIDEO_H
#define DRIVER_VIDEO_H

#include <stdint.h>
#include <audio.h>
#include <vesa_video.h>

#define VID_START ((uint16_t*)0xB8000)
#define VID_WIDTH 102
#define VID_HEIGHT 51

#define SET_COLOR(c, background) (uint8_t)((background << 4) | c)

/*
 * VGA Colors
 * Ref: http://wiki.osdev.org/Text_UI
 */
typedef enum {
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LIGHT_GRAY = 7,
	DARK_GREY = 8,
	LIGHT_BLUE = 9,
	LIGHT_GREEN = 10,
	LIGHT_CYAN = 11,
	LIGHT_RED = 12,
	LIGHT_MAGENTA = 13,
	LIGHT_BROWN = 14,
	WHITE = 15
} colors;

typedef struct{
	uint8_t row;
	uint8_t col;
} cursor;

void update_written_space();
void color_set(uint8_t font_c, uint8_t background_c);
void color_set_default();
void color_change(uint8_t font_c, uint8_t background_c);
void color_change_default();
void print_char(char c);
void print_char_at(char c, uint8_t row, uint8_t col);
void print_string(const char* str);
void print_line();
void print_enter();
void scroll();
void clear_screen();
void delete_char();
void print_num_dec(uint64_t num);
void print_num_hex(uint64_t num);
void print_num_bin(uint64_t num);
void print_num_base(uint64_t num, uint32_t base);
//static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

#endif
