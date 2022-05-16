//-------| src/clcd.c |-------//
#include "clcd.h"



static short * clcd_cmd, * clcd_data;

void init_clcd(short * address_cmd, short * address_data) {
	int D=1, C=0, B=0;
	int DL=1, N=1, F=0;
	
	clcd_cmd  = address_cmd;
	clcd_data = address_data;
	
	clcd_clear_display();
	clcd_return_home();
	clcd_display_control(D, C, B);
	clcd_function_set(DL, N, F);
}

void clcd_write_cmd(int cmd) {
	*clcd_cmd = (short)cmd; usleep(CLCD_CMD_US);
}
void clcd_write_data(int data) {
	*clcd_data = (short)data; usleep(CLCD_DATA_US);
}

void clcd_clear_display() {
	clcd_write_cmd(0x01); usleep(CLCD_RETURN_US);
}
void clcd_return_home() {
	clcd_write_cmd(0x02); usleep(CLCD_RETURN_US);
}
void clcd_entry_mode_set(int ID, int S) {
	int cmd = 1 << 2;
	if( ID != 0 ) { cmd |= (1 << 1); }
	if( S  != 0 ) { cmd |= (1 << 0); }
	clcd_write_cmd(cmd);
}
void clcd_display_control(int D, int C, int B) {
	int cmd = 1 << 3;
	if( D != 0 ) { cmd |= (1 << 2); }
	if( C != 0 ) { cmd |= (1 << 1); }
	if( B != 0 ) { cmd |= (1 << 0); }
	clcd_write_cmd(cmd);
}
void clcd_shift(int SC, int RL) {
	int cmd = 1 << 4;
	if( SC != 0 ) { cmd |= (1 << 3); }
	if( RL != 0 ) { cmd |= (1 << 2); }
	clcd_write_cmd(cmd);
}
void clcd_function_set(int DL, int N, int F) {
	int cmd = 1 << 5;
	if( DL != 0 ) { cmd |= (1 << 4); }
	if( N  != 0 ) { cmd |= (1 << 3); }
	if( F  != 0 ) { cmd |= (1 << 2); }
	clcd_write_cmd(cmd);
}
void clcd_set_DDRAM(int address) {
	int cmd = 1 << 7;
	cmd |= address;
	clcd_write_cmd(cmd);
}

void clcd_write_string(char str[]) {
	int i;
	for( i=0; (str[i] != 0); i++) {
		clcd_write_data(str[i]);
	}
}

// clcd operation of 1st state.
void s_init_clcd(){
	// clear cursor and display
	clcd_clear_display();
	clcd_return_home();
	clcd_entry_mode_set(1, 0);		// write left->right, not mirrored

	// write in 1st line
	clcd_set_DDRAM(0); 				// set cursor : 0
	clcd_write_string("Initializing...");

	// write in 2nd line
	clcd_set_DDRAM(0x40);
	clcd_write_string("Insert Mode");	
}

// when select mode, print mode string
void s_init_clcd_select_mode(char* mode_name){
	clcd_clear_display();
	clcd_set_DDRAM(0);
	clcd_write_string(mode_name);
}

// when choose wrong
void wrong_select(){
	clcd_clear_display();
	clcd_write_string("Wrong input");
	clcd_set_DDRAM(0x40);
	clcd_write_string("Please Retry");
}

// clcd operation of 2nd state
void s_TI_clcd(){
	clcd_clear_display();
	clcd_write_string("Insert Time E:BS");
	clcd_set_DDRAM(0x40);
	clcd_write_string("0~9:NUM    F:ETR");
}

// clcd operation of 3rd state
void s_WS_clcd(int input_time){
	int i = 0;
	
	while(i!=input_time){
		clcd_clear_display();
		if(i%4 == 0) {clcd_write_string("Washing");}
		if(i%4 == 1) {clcd_write_string("Washing.");}
		if(i%4 == 2) {clcd_write_string("Washing..");}
		if(i%4 == 3) {clcd_write_string("Washing...");}
		i++;
		usleep(500000);
	}
	
}

// at WS state, s_WS_clcd converting to 1 processor multi-proccessing scheduled code
void s_WS_scheduling(int counter){
	clcd_clear_display();
	if(counter%4 == 0) {clcd_write_string("Washing");}
	if(counter%4 == 1) {clcd_write_string("Washing.");}
	if(counter%4 == 2) {clcd_write_string("Washing..");}
	if(counter%4 == 3) {clcd_write_string("Washing...");}
}

// clcd operation of 4th state
void s_WD_clcd(){
	clcd_clear_display();
	clcd_write_string("Washing Done!");
	clcd_set_DDRAM(0x40);
	clcd_write_string("Exit : B");
}

