/*
 * Driver de Video
 */
 
#include <video.h>

 uint8_t curr_col = 0;
 uint8_t curr_row = 0;
 static uint8_t curr_color = SET_COLOR(GREEN, BLACK);

 cursor written_space = {0, 0}; //Marca hasta donde se puede borrar (inclusive)

 char buffer[64] = {0};
 
 //Funciones para manejo de colores
 
 /**
  * Cambio el color a usar a partir de ahora para imprimir
  */
 void color_set(uint8_t font_c, uint8_t background_c){
	 curr_color = SET_COLOR(font_c, background_c);
 }
 
 /**
  * Cambio el color al default
  */
 void color_set_default(){
	 color_set(GREEN, BLACK);
 }
 
 /**
  * Pinto toda la pantalla del color que elija
  */
 void color_change(uint8_t font_c, uint8_t background_c){
	 int curr_pos = 0;
	 uint16_t c;
	 while (curr_pos < VID_HEIGHT * VID_WIDTH){
		 c = VID_START[curr_pos] & 0xFF00;			//pongo en 0 los 8 primeros bits (del color)
		 VID_START[curr_pos++] = c | (SET_COLOR(font_c, background_c) << 8);
	 }
	 color_set(font_c, background_c);
 }
 
 /**
  * Pinto toda la pantalla del color default
  */
 void color_change_default(){
	 color_change(WHITE, BLACK);
 }
 
 //Funciones para imprimir
 
 /**
  * Imprimo un caracter en la posicion actual de la pantalla
  */
 void print_char(char c){

	switch (c){
		case '\n':						//new line
			print_line();
			break;
		case '\t':						//tab
			print_string("     ");
			break;
		case '\b':						//backspace
			delete_char();
			break;
		default:
			draw_char(c, curr_col*10, curr_row*15, 1, 0, 255, 0);
			//print_char_at(c, curr_row, curr_col);
			curr_col++;

			if (curr_col == VID_WIDTH){
				curr_col = 0;
				curr_row++;
				
				if (curr_row == VID_HEIGHT)
					scroll();
			}
			break;
	}
	 
 }
 
 /**
  * Imprimo un caracter en la posicion indicada en pantalla
  */
 void print_char_at(char c, uint8_t row, uint8_t col){
	 VID_START[row * VID_WIDTH + col] = (uint16_t) (c | (curr_color << 8));
 }

/**
  * Borro el char en la posicion row, col
  */
void delete_char_at(uint8_t row, uint8_t col){

	vesa_delete_char(col * 10, row * 15, 1);
	//print_char_at(' ',row, col);
}

/**
  * Borro el char que se encuentra detras del cursor y lo muevo
  */
void delete_char(){
	
	if (curr_col == 0){
		if(written_space.row == curr_row-1){
			beep();
			return;
		}

		curr_col = VID_WIDTH - 1;
		if (curr_row == 0){
			curr_row = 0;
			curr_col = 0;
		}
		else
			curr_row--;
	}
	else
		curr_col--;
	
	delete_char_at(curr_row, curr_col);
}
 
 /**
  * Imprimo un string en la posicion actual
  */
 void print_string(const char* str){

	 int i = 0;
	 while (str[i]){
		 print_char(str[i]);
		 i++;
	 }

 }
 
 /**
  * Imprimo un \n
  */
 void print_line(){

	while (curr_col < VID_WIDTH)
		print_char_at(' ',curr_row, curr_col++);
	 
	curr_row++;
	curr_col = 0;
	if (curr_row == VID_HEIGHT)
		scroll();
 }

/**
 * Imprimo el numero pasado en decimal
 * Ref: naiveConsole.c (Catedra Arqui)
 */
void print_num_dec(uint64_t num){
	print_num_base(num, 10);
}

/**
 * Imprimo el numero pasado en hexa
 * Ref: naiveConsole.c (Catedra Arqui)
 */
void print_num_hex(uint64_t num){
	print_num_base(num, 16);
}

/**
 * Imprimo el numero pasado en binario
 * Ref: naiveConsole.c (Catedra Arqui)
 */
void print_num_bin(uint64_t num){
	print_num_base(num, 2);
}


static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base){
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}


/**
 * Imprimo el numero pasado en la base deseada
 * Ref: naiveConsole.c (Catedra Arqui)
 */
void print_num_base(uint64_t num, uint32_t base){

	uintToBase(num, buffer, base);
	print_string(buffer);
}


 //Funciones de manejo de pantalla

/**
 * Manejo del ENTER
 */
void print_enter(){

	written_space.row = curr_row;
	print_line();
}

/**
 * Actualizo hasta donde puedo borrar
 */
void update_written_space(){
	
	written_space.col = curr_col;
	written_space.row = curr_row;
}

 /**
  * Subo todas las lineas 1 fila
  */
 void scroll(){

	//int curr_pos = 0;

	vesa_scroll();

	/*while (curr_pos < (VID_HEIGHT-1)*VID_WIDTH){
		VID_START[curr_pos] = VID_START[curr_pos + VID_WIDTH];
		curr_pos++;
	}*/
	written_space.row--;
	curr_row--;
	curr_col = 0;
	
	/*for(curr_pos = 0; curr_pos < VID_WIDTH; curr_pos++){
		print_char_at(' ', VID_HEIGHT-1, curr_pos);
	}*/
 }

 /**
  * Limpio la pantalla
  */
 void clear_screen(){
	
 	clear_vesa_screen();

	/*for (uint8_t r = 0; r < VID_HEIGHT; r++)
		for (uint8_t c = 0; c < VID_WIDTH; c++)
			print_char_at(' ', r, c);*/
			
	curr_col = 0;
	curr_row = 1;
	update_written_space();
 }
