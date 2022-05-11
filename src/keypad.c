//-------| src/keypad.c |-------//
#include "keypad.h"
#include "clcd.h"


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

int s_TI_keypad(int* key_num, int* loop_count){
	int temp_num;
	int cur_key_num = *key_num;
	keypad_read(&temp_num);

	if ((temp_num >= 10) || (temp_num <14)){		// if click wrong button
		void wrong_select();
	}

	else if (temp_num < 10){						// if click 0~9 button
		if(cur_key_num == -1){cur_key_num == 0;}
		*key_num = cur_key_num * 10 + temp_num;
		*loop_count += 1;	// This variable points number of digits
	}

	else if(temp_num >= 14){						// if click 14(e), 15(f) button
		if (temp_num == 14){						// if click 14(e). => erase
			*key_num = cur_key_num / 10;
			*loop_count -= 1;
		}
		else if(temp_num == 15){					//if click 15(f). => enter :: goto next state
			if(*key_num == -1) {
				void wrong_select();
				return 0;
			}
			return 1;
		}

	}
	return 0;
}