//-------| lib/clcd.h |-------//
// #ifndef STRING_H 

// #define STRING_H
// #include <string.h>
// #endif

#ifndef	CLCD_H
#define	CLCD_H 


#include <unistd.h> // usleep()


#define CLCD_CMD_US		40
#define CLCD_DATA_US	(CLCD_CMD_US + 4)
#define CLCD_RETURN_US	(1520 - CLCD_CMD_US)



void init_clcd(short * address_cmd, short * address_data);
void clcd_write_cmd(int cmd);
void clcd_write_data(int data);
void clcd_clear_display();
void clcd_return_home();
void clcd_entry_mode_set(int ID, int S);
void clcd_display_control(int D, int C, int B);
void clcd_shift(int SC, int RL);
void clcd_function_set(int DL, int N, int F);
void clcd_set_DDRAM(int address);
void clcd_write_string(char str[]);
void s_init_clcd();
void s_init_clcd_select_mode(char* mode_name);
void wrong_select();
void s_TI_clcd();
void s_WS_clcd(int input_time);
void s_WS_scheduling(int counter);
void s_WD_clcd();


#endif
