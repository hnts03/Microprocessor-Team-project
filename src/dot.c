//-------| src/dot.c |-------//
#include "dot.h"


static unsigned short dot_hexadecimal[16][MAX_DOT] = {
	{0x7F, 0x41, 0x41, 0x41, 0x7F}, // 0
	{0x00, 0x00, 0x7F, 0x00, 0x00}, // 1
	{0x4F, 0x49, 0x49, 0x49, 0x79}, // 2
	{0x49, 0x49, 0x49, 0x49, 0x7F}, // 3
	{0x78, 0x08, 0x08, 0x7F, 0x08}, // 4
	{0x79, 0x49, 0x49, 0x49, 0x4F}, // 5
	{0x7F, 0x49, 0x49, 0x49, 0x4F}, // 6
	{0x40, 0x40, 0x40, 0x40, 0x7F}, // 7
	{0x7F, 0x49, 0x49, 0x49, 0x7F}, // 8
	{0x78, 0x48, 0x48, 0x48, 0x7F}, // 9
	{0x1F, 0x28, 0x48, 0x28, 0x1F}, // A
	{0x7F, 0x49, 0x49, 0x49, 0x36}, // B
	{0x3E, 0x41, 0x41, 0x41, 0x22}, // C
	{0x7F, 0x41, 0x41, 0x41, 0x3E}, // D
	{0x7F, 0x49, 0x49, 0x49, 0x49}, // E
	{0x7F, 0x48, 0x48, 0x48, 0x48}  // F
};

static unsigned short dot_TIME[4][MAX_DOT] = {
	{0x60, 0x60, 0x7F, 0x60, 0x60},	// T
	//{0x00, 0x60, 0x6F, 0x60, 0x00}, // i
	{0x41, 0x41, 0x7F, 0x41, 0x41}, // I
	{0x7F, 0x70, 0x0C, 0x70, 0x7F}, // M
	{0x7F, 0x49, 0x49, 0x49, 0x49}  // E
};

static unsigned short dot_tornado[6][MAX_DOT] = {
	{0x70, 0x70, 0x77, 0x07, 0x07},
	{0x00, 0x77, 0x77, 0x77, 0x00},
	{0x07, 0x07, 0x77, 0x70, 0x70},
	{0x0E, 0x0E, 0x36, 0x38, 0x38},
	{0x1C, 0x1C, 0x00, 0x1C, 0x1C},
	{0x38, 0x38, 0x36, 0x0E, 0x0E}
};

/*		Tornado bitmap
11100     01110     00111      00000     00000     00000
11100     01110     00111      00111     00000     11100
11100     01110     00111      00111     11011     11100
00000     00000     00000      11011     11011     11011
00111     01110     11100      11100     11011     00111
00111     01110     11100      11100     00000     00111
00111     01110     11100      00000     00000     00000
*/


static short * dot[MAX_DOT];

void init_dot(short * address[]) {
	int i;
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = address[i];
	}
}

void dot_clear() {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = 0;
	}
	usleep(0); // for Ximulator
}

void dot_write(int number) {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = dot_hexadecimal[number][i];
	}
	usleep(0); // for Ximulator
}

// for s_init_dot
// dot matrix on
void dot_on(){
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = 0xFF;
	}
	usleep(0); // for Ximulator
}

// Operation of dot in 1st state.
// if init_dot_on == 1, all matrix turn on
// else, all matrix turn off
void s_init_dot(int init_dot_on){
	if (init_dot_on == 1) {dot_on();}
	else {dot_clear();}
}


// Operation of dot in 2nd state
void s_TI_dot(){
	int i = 0;
	int j;
	int counter = 0;
	while(/* keypad_read() == 'e' */ counter != 3){	// temporary condition	
		if (i == 4){i = 0; counter++;}

		// for control the function char by char
		for(j = 0; j<MAX_DOT; j++){
			*dot[j] = dot_TIME[i][j];
		}
		usleep(50000);
		i++;
	}
}

// Operation of dot in 3rd state
void s_WS_dot(int input_time){
	int i = 0;
	int j;
	int counter = 0;
	while(/* keypad_read() == 'e' */ counter != 3){	// temporary condition	
		if (i == 6){i = 0; counter++;}

		// for control the function char by char
		for(j = 0; j<MAX_DOT; j++){
			*dot[j] = dot_tornado[i][j];
		}
		usleep(50000);
		i++;
	}
}