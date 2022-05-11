//-------| lib/keypad.h |-------//
#ifndef	KEYPAD_H
#define	KEYPAD_H 


#define MAX_KEY_COL	4
#define MAX_KEY_ROW	4
#define ERROR_VAL 100000000


void init_keypad(short * address_out, short * address_in) ;
int keypad_read(int * key_value);
int s_init_keypad();
int s_TI_keypad(int* key_num, int* loop_count);



#endif
