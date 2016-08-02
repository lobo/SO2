/* 
 * extraido de BareMetal-OS
 * Orig. written by ohnx.
*/

#ifndef VESA_VIDEO_H
#define VESA_VIDEO_H

#include <lib.h>
#include <memmanager.h>

void vesa_set_video_mode();
void clear_vesa_screen();
void vesa_scroll();

void clear_buffer();
void flush_buffer();

void put_pixel(int x, int y, char red, char green, char blue);
void draw_rect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b);
void draw_frect(unsigned int x, unsigned int y, unsigned int l, unsigned int w, unsigned int r, unsigned int g, unsigned int b);
void draw_fcircle(unsigned int x, unsigned int y, int radius, unsigned int r, unsigned int g, unsigned int b);

void draw_char_buffer(char to, unsigned int x, unsigned int y, int size, unsigned int r, unsigned int g, unsigned int b);
void draw_char(char to, unsigned int x, unsigned int y, int size, unsigned int r, unsigned int g, unsigned int b);
void vesa_delete_char(unsigned int xd, unsigned int yd, int size);

#endif