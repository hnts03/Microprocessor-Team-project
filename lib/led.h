//-------| lib/led.h |-------//
#ifndef	LED_H
#define	LED_H 


#include <unistd.h> // usleep()


void init_led(short * address);
void led_down_shift();
void led_up_shift();
void led_blink_all();
void led_clear();
void led_all();
void s_init_led(int init_led_on);
void s_TI_led();
void s_WS_led(int input_time);
void s_WD_led();
void s_WS_stack(int counter);
void s_TI_scheduling(int count);
void s_WD_scheduling(int count);
#endif
