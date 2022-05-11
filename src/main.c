//-------| src/main.c |-------//
#include "main.h"


static off_t IEB_DOT[MAX_DOT] = {
	IEB_DOT1,
	IEB_DOT2,
	IEB_DOT3,
	IEB_DOT4,
	IEB_DOT5
};
static off_t IEB_FND[MAX_FND] = {
	IEB_FND0,
	IEB_FND1,
	IEB_FND2,
	IEB_FND3,
	IEB_FND4,
	IEB_FND5,
	IEB_FND6,
	IEB_FND7
};

static int fd;
static int map_counter = 0;
static void * map_data[100];
static seclection_t sel; // selection_t struct is in main.h

// This variables are state flag.
// Start the state that matching flag is 1
int init = 1;
int TI = 0;
int WS = 0;
int WD = 0;


int main(int argc, char* argv[]) {
	
	int i;
	short * led, * dot[MAX_DOT], * fnd[MAX_FND];
	short * clcd_cmd, * clcd_data/*, * keypad_out, * keypad_in*/; // in ximulator, keypad doesn't work

	int init_on[3] = {0, 0, 0};	// control flag of init, [0] : led, [1] : dot, [2] : fnd
	
	
	
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		fprintf(stderr, "Cannot open /dev/mem file");
		exit(EXIT_FAILURE);
	}
	
	// Mapping part
	led = mapper(IEB_LED, PROT_WRITE);
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = mapper(IEB_DOT[i], PROT_WRITE); // 2nd argument is protect arg. if PROT_WRITE : enable write, if PROT_READ : enable read
	}
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = mapper(IEB_FND[i], PROT_WRITE);
	}
	clcd_cmd  = mapper(IEB_CLCD_CMD, PROT_WRITE);
	clcd_data = mapper(IEB_CLCD_DATA, PROT_WRITE);
	//keypad_out  = mapper(IEB_KEY_W, PROT_WRITE);	// in ximulator, keypad doesn't work
	//keypad_in = mapper(IEB_KEY_R, PROT_READ);		// in ximulator, keypad doesn't work
	


	// Initialize part
	init_led(led);
	init_dot(dot);
	init_fnd(fnd);
	init_clcd(clcd_cmd, clcd_data);
	//init_keypad(keypad_out, keypad_in);			// in ximulator, keypad doesn't work
	
	sel.all = 0;

	if (checker(argc, argv, init_on) == ERROR){error();}
	else {while( logic(init_on) == TRUE ) {	}}
	
	unmapper();
	close(fd);
	return 0;
}

short * mapper(off_t offset, int prot) {
	map_data[map_counter] = mmap(NULL, sizeof(short), prot, MAP_SHARED, fd, offset);
	if ( map_data[map_counter] == MAP_FAILED ) {
		fprintf(stderr, "Cannot do mmap()");
		emergency_closer();
	}
	return (short *)map_data[map_counter++];
}

void unmapper() {
	int i;
	for( i=0; i<map_counter; i++) {
		munmap(map_data[i], sizeof(short));
	}
	map_counter = 0;
}

void emergency_closer() {
	unmapper();
	close(fd);
	exit(EXIT_FAILURE);
}

truth_t logic(int* init_on) {
	int exit_flag = 0;
	int input_time = -1;
	int digit_num = 1;

	// State 1
	if (init == 1){
		state_init(&exit_flag, init_on); 
		init = 0; TI = 1; 
		// return FALSE; 					// For debug
	}

	// State 2
	if (TI == 1){
		state_TI(&input_time, &digit_num);

		TI = 0; WS = 1;
		// return FALSE;							// For debug
	}										// Successfully accesible

	// State 3
	if (WS == 1){
		state_WS(input_time, digit_num); 
		WS = 0; WD = 1; 
		// return FALSE;						// For debug
	}

	// State 4
	if (WD == 1){
		state_WD(); 
		WD = 0; 
		return FALSE;
	}

	return TRUE;
}

void state_init(int* exit_flag, int* init_on){
	int counter = 2;

	//system("clear");						// clear terminal window
	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();

	s_init_clcd();							// static function
	s_init_fnd(init_on[2]);					// static function
	s_init_dot(init_on[1]);					// static function
	s_init_led(init_on[0]);					// dynamic function

	while(select_mode(&counter, exit_flag, init_on) == TRUE){ } // mode select loop


}


truth_t select_mode(int* counter, int* exit_flag, int* init_on) {
	int i;  char buf[100];
	char clcd_str[20] = "";
	

	printf("\n");
	printf("************ Select Mode ***********\n");
	printf("*   0 : Standard    1 : Blanket    *\n");
	printf("*   2 : Rinse       3 : Dehyderate *\n");
	printf("************************************\n");
	printf("*    press 'e' to exit program *** *\n");
	printf("* init_led_on : %d init_dot_on : %d  *\n", init_on[0], init_on[1]);
	printf("* init_fnd_on : %d                  *\n", init_on[2]);
	printf("************************************\n\n");

	if (*counter == 2){*counter = 0;}
	scanf("%s", buf);
	
	if ((*buf == '0') || (*buf == '1') || (*buf == '2') || (*buf == '3')){
		switch (*buf){
			case '0':
				s_init_clcd_select_mode("Standard");
				break;
			case '1':
				s_init_clcd_select_mode("Blanket");
				break;
			case '2':
				s_init_clcd_select_mode("Rinse");
				break;
			case '3':
				s_init_clcd_select_mode("Dehyderate");
				break;
		}
		sleep(2);
		return FALSE;
	}
	else if(*buf == 'e'){
		clcd_clear_display();
		clcd_write_string("Exit mode");

		dot_clear();
		led_clear();
		fnd_clear();

		emergency_closer();
		return FALSE;
	}
	else {printf("Wrong input. Try again\n"); (*counter) += 1; return TRUE;}
	
}


// This function is for checking argv  
error_t checker(int argc, char* argv[], int* init_on ){

	if (argc > 4){return ERROR;}
	else if (argc >= 2) {
		for (int i = 0; i < argc; i++){
			if(strcmp(argv[i], "led-on") == 0){init_on[0] = 1;}
			//if(strcmp(argv[i], "led-off") == 0){init_on[0] = 0;}  	// These are actually not needed
			else if(strcmp(argv[i], "dot-on") == 0){init_on[1] = 1;}
			//if(strcmp(argv[i], "dot-off") == 0){init_on[1] = 0;}
			else if(strcmp(argv[i], "fnd-on") == 0){init_on[2] = 1;}
			//if(strcmp(argv[i], "fnd-off") == 0){init_on[2] = 0;}
			else {return ERROR;}
		}
	}
	return SUCCESS;
} 

// if argv is not available
void error(){
	printf("Arguments are not available. Please Check the arguments.");
	clcd_write_string("ERROR");

	usleep(1000000);
}

void state_TI(int* input_time, int* digit_num){
	int key_num = -1;		
	int enter_flag = 0;	
	int loop_count = 0;

	key_num = 37;				// These 3 assignment is for test
	enter_flag = 1;
	loop_count = 2;

	s_TI_clcd();
	s_TI_fnd(key_num, loop_count);	// for test.
	// s_TI_led();				// s_TI_led and dot will combine with while loop below
	// s_TI_dot();
	while(enter_flag != 1){
		// enter_flag = s_TI_keypad(&key_num, &loop_count);
		// /*enter_flag =*/s_TI_fnd(key_num, loop_count);
		// sleep(1);			// sampling delay = 1s
	}
	*input_time = key_num;
	*digit_num = loop_count;
}


void state_WS(int input_time, int digit_num){
	s_WS_led(input_time);
	// s_WS_dot(input_time);
	// s_WS_fnd(input_time, digit_num);
	// s_WS_clcd(input_time);
}

void state_WD(){

}

