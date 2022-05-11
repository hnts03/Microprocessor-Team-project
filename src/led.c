//-------| src/led.c |-------//
#include "led.h"

static short * led;

void init_led(short * address) {
	led = address;
}

void led_down_shift() {
	
	int i;
	for( i=1; i<=16 ; i++ ) {
		*led = (short)~( ((unsigned long)0xFF << i) >> 8 );
		usleep(50000);
	}
}

void led_up_shift() {
	
	int i;
	for( i=1; i<=16 ; i++ ) {
		*led = (short)~((unsigned long)0xFF00 >> i);
		usleep(50000);
	}
}

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

	if (init_led_on == 1){
		while(/* is_key_clicked */){
			i++;
			if (i < 8){ *led = (short) ~((*unsigned long)0xFF00 >> i); usleep(100000);}
			else if (i >= 8) {*led = (short) ~((*unsigned long)0xFF00 >> (i - 8)); usleep(100000);}
			if (i == 16) {i = 0;}
		}
	}
	else {*led = 0;}
}
