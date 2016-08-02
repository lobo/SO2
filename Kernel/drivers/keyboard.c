/*Driver del teclado */

#include <keyboard.h>


KBLocks locks = {FALSE, FALSE, FALSE};
uint8_t shift_pressed = FALSE;

char readBuffer[BUFFER_SIZE] = {0};
//uint8_t READING = FALSE;
uint8_t EOF = FALSE;
uint16_t bufferPos = 0;

//deberia estar el typedef en el h y el resto aca.
KBCode code_table[256] = {
	{EMPTY, EMPTY}, // 0X00
	{EMPTY, EMPTY}, // 0x01 ESC
	{'1', '!'}, // 0x02
	{'2', '@'}, // 0x03
	{'3', '#'}, // 0x04
	{'4', '$'}, // 0x05
	{'5', '%'}, // 0x06
	{'6', '^'}, // 0x07
	{'7', '&'}, // 0x08
	{'8', '*'}, // 0x09
	{'9', '('}, // 0x0A
	{'0', ')'}, // 0x0B
	{'-', '_'}, // 0x0C
	{'=', '+'}, // 0x0D
	{EMPTY, EMPTY}, // 0x0E BACK
	{EMPTY, EMPTY}, // 0x0F TAB
	{'q', 'Q'}, // 0x10
	{'w', 'W'}, // 0x11
	{'e', 'E'}, // 0x12
	{'r', 'R'}, // 0x13
	{'t', 'T'}, // 0x14
	{'y', 'Y'}, // 0x15
	{'u', 'U'}, // 0x16
	{'i', 'I'}, // 0x17
	{'o', 'O'}, // 0x18
	{'p', 'P'}, // 0x19
	{'[', '{'}, // 0x1A
	{']', '}'}, // 0x1B
	{EMPTY, EMPTY}, // 0x1C ENTER
	{EMPTY, EMPTY}, // 0x1D CTRL IZQ
	{'a', 'A'}, // 0x1E
	{'s', 'S'}, // 0x1F
	{'d', 'D'}, // 0x20
	{'f', 'F'}, // 0x21
	{'g', 'G'}, // 0x22
	{'h', 'H'}, // 0x23
	{'j', 'J'}, // 0x24
	{'k', 'K'}, // 0x25
	{'l', 'L'}, // 0x26
	{';', ':'}, // 0x27
	{'\'', '"'}, // 0x28
	{'`', '~'}, // 0x29
	{EMPTY, EMPTY}, // 0x2A SHIFT IZQ
	{'\\', '|'}, // 0x2B
	{'z', 'Z'}, // 0x2C
	{'x', 'X'}, // 0x2D
	{'c', 'C'}, // 0x2E
	{'v', 'V'}, // 0x2F
	{'b', 'B'}, // 0x30
	{'n', 'N'}, // 0x31
	{'m', 'M'}, // 0x32
	{',', '<'}, // 0x33
	{'.', '>'}, // 0x34
	{'/', '?'}, // 0x35
	{EMPTY, EMPTY}, // 0x36 SHIFT DER
	{'*', '*'}, // 0x37 KEYPAD *
	{EMPTY, EMPTY}, // 0x38 ALT IZQ
	{' ', ' '}, // 0x39 quiza habria que ponerlo como null y tratarlo como una funcion especial?
	{EMPTY, EMPTY}, // 0x3A CAPSLOCK
	{EMPTY, EMPTY}, // 0x3B F1
	{EMPTY, EMPTY}, // 0x3C F2
	{EMPTY, EMPTY}, // 0x3D F3
	{EMPTY, EMPTY}, // 0x3E F4
	{EMPTY, EMPTY}, // 0x3F F5
	{EMPTY, EMPTY}, // 0x40 F6
	{EMPTY, EMPTY}, // 0x41 F7
	{EMPTY, EMPTY}, // 0x42 F8
	{EMPTY, EMPTY}, // 0x43 F9
	{EMPTY, EMPTY}, // 0x44 F10
	{EMPTY, EMPTY}, // 0x45 NUM LOCK
	{EMPTY, EMPTY}, // 0x46 SCROLL LOCK
	{EMPTY, EMPTY}, // 0x47 UP
	{EMPTY, EMPTY}, // 0x48
	{EMPTY, EMPTY}, // 0x49
	{EMPTY, EMPTY}, // 0x4A
	{EMPTY, EMPTY}, // 0x4B LEFT
	{EMPTY, EMPTY}, // 0x4C
	{EMPTY, EMPTY}, // 0x4D RIGHT
	{EMPTY, EMPTY}, // 0x4E
	{EMPTY, EMPTY}, // 0x4F
	{EMPTY, EMPTY}, // 0x50 DOWN
	{EMPTY, EMPTY}, // 0x51
	{EMPTY, EMPTY}, // 0x52
	{EMPTY, EMPTY}, // 0x53 DELETE
	{EMPTY, EMPTY}, // 0x54
	{'*', '*'}, // 0x55
	{'*', '*'}, // 0x56
	{EMPTY, EMPTY}, // 0x57 F11
	{EMPTY, EMPTY}, // 0x58 F12
};
	

void keyboard_handler(uint64_t sc){ //se podria hacer con punteros a funciones

	char c;

	if (sc == 170){
		shift_pressed = FALSE; 		//release del left shift
		return;
	}

	if (sc > 0x58) {				//salteamos los release
		return;
	}

	if (code_table[sc].ascii != EMPTY) {

		if (shift_pressed != locks.capsLock) {
			c = code_table[sc].spec_ascii;
		} else {
			c = code_table[sc].ascii;
		}

		readBuffer[bufferPos++] = c;
	}
	
	switch (sc) {

		case 0x0E: //backspace
			readBuffer[bufferPos++] = '\b';
			break;

		case 0x0F: //tab
			readBuffer[bufferPos++] = '\t';
			break;

		case 0x1C: //enter
			readBuffer[bufferPos++] = '\n';
			EOF = TRUE;
			break;

		case 0x2A: //left shift
			shift_pressed = TRUE;
			break;
		
		case 0x3A:
			locks.capsLock = !locks.capsLock;
			break;

		//seguir
	
	}
	return;	
}

int read_from_buffer(char* buf, size_t length) {
	bufferPos = 0;
	int i = 0;
	EOF = FALSE;
		
	while (bufferPos < length && !EOF);

	bufferPos >= BUFFER_SIZE ? (readBuffer[BUFFER_SIZE - 1] = '\0') : (readBuffer[++bufferPos] = '\0');

	while ((buf[i] = readBuffer[i]) != '\0')
		i++;
	
	return bufferPos;
}
