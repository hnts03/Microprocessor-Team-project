//-------| src/fnd.c |-------//
#include "fnd.h"

static unsigned short fnd_hexadecimal[16] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x67, // 9
	0x77, // A
	0x7C, // b
	0x39, // C
	0x5E, // d
	0x79, // E
	0x71  // F
};

static short * fnd[MAX_FND];

void init_fnd(short * address[]) {
	int i;
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = address[i];
	}
}

void fnd_clear() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0;
	}
}

void fnd_all() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0xFF;
	}
}

void fnd_write(int hexadecimal, int fnd_num) {
	*fnd[fnd_num] = fnd_hexadecimal[hexadecimal];
}

void fnd_hexa_number(unsigned long number) {
	int i;
	for(i=0; i<MAX_FND; i++){
		fnd_write( (short)(number & 0xF), i);
		number = number >> 4;
	}
}

// for s_WS_fnd
void fnd_deca_number(unsigned long number, int digit_num){
	int i;
	int temp = number;
	for (i = 0; i<digit_num; i++){
		fnd_write((short)(temp%10), i);
		temp = temp / 10;
		usleep(1);
	}
}



// for s_init_fnd
void fnd_0(){
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0x3F;
	}
}

// FND operation of 1st state.
// if init_fnd_on == 1, turn all fnds on with 0
// else, turn all fnds off
void s_init_fnd(int init_fnd_on){
	if (init_fnd_on == 1){fnd_0();}
	else {fnd_clear();}
}


// FND operation of 2nd state
int s_TI_fnd(int key_num, int loop_counter){
	int temp_num;
	int calc_step = key_num;
	int i;

	if (loop_counter == 0){return 0;}			// if loop_counter == 0, This function not work
	if (loop_counter >= 8){return 0;}
	if ((loop_counter >= 0) && (loop_counter <= 7)) {*fnd[loop_counter] = 0;}			// if loop_counter over 8, it can't display on FND
	for (i = 0; i < loop_counter; i++){
		temp_num = calc_step % 10;
		calc_step = calc_step / 10;

		fnd_write(temp_num, i);
	}
	return 1;
}

// FND operation of 3rd state
void s_WS_fnd(int input_time, int digit_num){
	int time;
	for (time = input_time; time >= 0; time--){
		fnd_deca_number(time, digit_num);
		usleep(500000);		// for 0.5s
	}
}

//FND operation of 4th state
void s_WD_fnd(){
	int index;
	for (index = 1; index < MAX_FND; index++){
		*fnd[index] = 0;
	}
	*fnd[0] = 0x3F;
}
