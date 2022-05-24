//-------| src/keypad.c |-------//
//#include "keypad.h"
//#include "clcd.h"
//#include "fnd.h"
#include "main.h" 	// for using stdio.h and other headers

static short * keypad_out, * keypad_in;
static int pressed_key_num;

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

// init state keypad function.
// This function not used.
int s_init_keypad(){
	int temp = 10;
	keypad_read(&temp);
	if ((temp == 0) || (temp == 1) || (temp == 2) || (temp == 3)) {return 0;}
	return 1;
}

// TI state
// This function uses only 0~9, e, f button.
// using for time insertion with decimal number.
int s_TI_keypad(int* key_num, int* loop_count, int* input_key){
	int temp_num;
	int cur_key_num = *key_num;
	pressed_key_num = keypad_read(&temp_num);
	if (pressed_key_num != 0){
		if (temp_num == -1) {return 0;}
		if ((temp_num >= 10) && (temp_num <14)){		// if click wrong button
			wrong_select();
			usleep(500000);					// 0.5s for err message
			clcd_clear_display();
			//system("clear");
			//printf("%d is temp_num\n", temp_num);

			sleep(2);
		}

		else if (temp_num < 10){						// if click 0~9 button
			if(cur_key_num == -1){cur_key_num = 0;}
			*key_num = cur_key_num * 10 + temp_num;
			*input_key = temp_num;
			//system("clear");
			
			//printf("%d is temp_num\n", temp_num);
			//printf("%d is *key_num\n", *key_num);
			
	
			*loop_count += 1;	// This variable points number of digits
		}

		else if(temp_num >= 14){						// if click 14(e), 15(f) button
			if (temp_num == 14){						// if click 14(e). => erase
				*key_num = cur_key_num / 10;
				*loop_count -= 1;
				if (*loop_count <= 0) {*loop_count = 0; fnd_clear();}

				//system("clear");
				//printf("%dpressed_key_num is temp_num\n", temp_num);
				//printf("%d is *key_num\n", *key_num);

			}
			else if(temp_num == 15){					//if click 15(f). => enter :: goto next state
				if(*key_num == -1) {
					wrong_select();
					usleep(500000);					// 0.5s for err message
					clcd_clear_display();
					return 0;
				}
				clcd_clear_display();
				clcd_write_string("ENTER PRESSED!");
				
				system("clear");
				printf("\n");
				printf("************ [TI state] ************\n");
				printf("* Current input number : %-8d  *\n", *key_num);
				printf("* Pressed key number : %-2d          *\n",temp_num);
				printf("************************************\n");
				printf("* Sampling Freq : 1Hz              *\n");
				printf("************************************\n");
				printf("* Enter Pressed!                   *\n");
				printf("************************************\n\n");
				sleep(1);				

				return 1;
			}

		}
		return 0;
	}
	else {return 0;}
	
	
}

// WD state
// if machine done, press b button for extting program.
int s_WD_keypad(){
	int num = -1;
	pressed_key_num = keypad_read(&num);
	if (pressed_key_num != 0) {
		if (num == 11){
			clcd_clear_display();
			clcd_write_string("EXIT PRESSED!");

			system("clear");
			printf("\n");
			printf("************ [WD state] ************\n");
			printf("*                                  *\n");
			printf("*            Wasing Done!          *\n");
			printf("*				   *\n");
			printf("*          Press Button [B]!       *\n");
			printf("*				   *\n");
			printf("*         Button [B] Pressed!      *\n");
			printf("*				   *\n");
			printf("************************************\n\n");

			sleep(1);
			return 1;
		}
		else {
			wrong_select();			// wrong input, retry
			sleep(1);
			s_WD_clcd();			// Change to WD state clcd operation
		}
	}
	
	return 0;
}

