//-------| src/led.c |-------//
#include "led.h" 

static short * led;

// basic function
void init_led(short * address) {
	led = address;
}

// basic function
void led_down_shift() {
	
	int i;
	for( i=1; i<=16 ; i++ ) {
		*led = (short)~( ((unsigned long)0xFF << i) >> 8 );
		usleep(50000);
	}
}

// basic function
void led_up_shift() {
	
	int i;
	for( i=1; i<=16 ; i++ ) {
		*led = (short)~((unsigned long)0xFF00 >> i);
		usleep(50000);
	}
}

// basic function
void led_blink_all() {
	int i;
	for( i=1; i<=16 ; i++ ) {
		if ( i%2 == 1 ) {
			*led = (short)~0xFF;
		}
		else {
			*led = (short)~0x00;
		}
		usleep(100000);
	}
}

void led_clear() {
	*led = (short)~0;
}

void led_all() {
	*led = (short)~0xFF;
}

void led_bit(int number) {
	*led = (short)~(number << 4);
}




// 1st state of FSM
// make operation of each part and then combine them to 1 function
// if init_led_on == 1, led up-down operation.
// else, led not working
void s_init_led(int init_led_on){
	int i = 0;

	if (init_led_on == 1){							// For caring bug of Ximulator, Added 2 lines in if statement.
		*led = (short)(~(unsigned long)0xFF); 
		usleep(5);
		*led = (short)(~(unsigned long)0xFF);
	}
	else {*led = ~0;}
}

// led operation of 2nd state
// Sequential blink of odd and even LEDs
void s_TI_led(){
	int i = 0;

	*led = (short) ~(0xAA);							// odd blink
	usleep(500000);
	while(/* keypad_read() == 16 */i != 5){			// This condition is temperary condition.
		if (*led != 0x55) {*led = (short)0x55; usleep(500000);}
		else {*led = (short)0xAA; usleep(500000);}
		i++;
	}

}

// This function is actual LED function in TI state
void s_TI_scheduling(int count){
	if (count%2 == 1) {*led = (short) ~(0xAA);}
	else {*led = (short) ~(0x55);}
}


// led operation of 3rd state
// this function is same with s_init_led().
void s_WS_led(int input_time){
	int i = 0;

	while(i<=10/*i != key_num */){
		i++;
		if (i < 8){ *led = (short) ~((unsigned long)0x0100 >> i); usleep(50000);}
		else if (i >= 8) {*led = (short) ~((unsigned long)(0x01 << i) >> 8 ); usleep(50000);}
		if (i == 16) {i = 0;}
	}
}

// This function is actually using in WS state.
// stack LEDs downside to upside
void s_WS_stack(int counter){
	*led = (short)~((unsigned long)0xFF00 >> counter);
}


// led operation of 4th sate
// this function is same with s_TI_led(). but stop trigger is key_num
void s_WD_led(){
	int i=0;
	while(1){
		if(i==10){break;}			// for debugging

		if(i%2 == 1) {*led = (short)0x55;}
		else {*led = (short)0xAA;}
		usleep(500000);
		i++;
	}
}

// This function is actually using in WD state
// even / odd LEDs blinking.
// same with TI state
void s_WD_scheduling(int count){
	if (count%2 == 1) {*led = (short) ~(0xAA);}
	else {*led = (short) ~(0x55);}
}
