//-------| src/keypad.c |-------//
#include "keypad.h"


static short * keypad_out, * keypad_in;

void init_keypad(short * address_out, short * address_in) {
	keypad_out  = address_out;
	keypad_in = address_in;
}

int keypad_read(int * key_value) {
	int col, row, key_count = 0;
	short key_temp;
	
	for( col=0; col<MAX_KEY_COL; col++ ) {
		*keypad_out = (short)(0x08 >> col);
		key_temp = * keypad_in;
		
		for( row=0; row<MAX_KEY_ROW; row++ ) {
			if( ( (key_temp >> row) & 1 ) == 1 ) {
				*key_value = (row * 4) + col;
				key_count++;
			}
		}
	}
	return key_count;
}


int s_init_keypad(){
	int temp = 10;
	keypad_read(&temp);
	if ((temp == 0) || (temp == 1) || (temp == 2) || (temp == 3)) {return 0;}
	return 1;
}