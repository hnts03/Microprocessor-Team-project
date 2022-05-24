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
static seclection_t sel; // selection_t struct is in main.h, This variable actually not use in this program

// This variables are state flag.
// Start the state that matching flag is 1
int init = 1;
int TI = 0;
int WS = 0;
int WD = 0;


int main(int argc, char* argv[]) {
	
	int i;
	short * led, * dot[MAX_DOT], * fnd[MAX_FND];
	short * clcd_cmd, * clcd_data, * keypad_out, * keypad_in; // in ximulator, keypad doesn't work

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
	keypad_out  = mapper(IEB_KEY_W, PROT_WRITE);		// in ximulator, keypad doesn't work
	keypad_in = mapper(IEB_KEY_R, PROT_READ);			// in ximulator, keypad doesn't work
	


	// Initialize part
	init_led(led);
	init_dot(dot);
	init_fnd(fnd);
	init_clcd(clcd_cmd, clcd_data);
	init_keypad(keypad_out, keypad_in);					// in ximulator, keypad doesn't work
	
	sel.all = 0;										// actually, not use in this program


	// Checking part
	if (checker(argc, argv, init_on) == ERROR){error();}
	else {while( logic(init_on) == TRUE ) {	}}			// logic() looping in while loop
	

	// If WD state over, program closing part
	unmapper();
	term_EXIT(3);						// Unmapping Done msg | Terminal
	sleep(1);

	close(fd);
	term_EXIT(4);						// Closing FD Done msg | Terminal
	sleep(1);
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
	}										// Successfully accessible

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

// Init state
void state_init(int* exit_flag, int* init_on){
	int counter = 2;

	//system("clear");						// clear terminal window

	// all parts clear
	led_clear();
	dot_clear();
	fnd_clear();
	clcd_clear_display();

	s_init_clcd();							// static function | CLCD
	s_init_fnd(init_on[2]);					// static function | FND
	s_init_dot(init_on[1]);					// static function | DOT
	s_init_led(init_on[0]);					// static function | LED

	while(select_mode(&counter, exit_flag, init_on) == TRUE){ } // mode select loop


}


// Mode select function.
// Using keyboard input(scanf() function), selects mode.
// In real, selecting not makes changes.
// It just shows possibility for branching new states. :: Update probability
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
	int i;

	if (argc > 4){return ERROR;}
	else if (argc >= 2) {
		for (i = 1; i < argc; i++){
			if(strcmp(argv[i], "led-on") == 0){init_on[0] = 1;}
			else if(strcmp(argv[i], "led-off") == 0){init_on[0] = 0;}  	// These are actually not needed
			else if(strcmp(argv[i], "dot-on") == 0){init_on[1] = 1;}
			else if(strcmp(argv[i], "dot-off") == 0){init_on[1] = 0;}
			else if(strcmp(argv[i], "fnd-on") == 0){init_on[2] = 1;}
			else if(strcmp(argv[i], "fnd-off") == 0){init_on[2] = 0;}
			else {return ERROR;}
		}
	}
	return SUCCESS;
} 

// if argv is not available
void error(){
	printf("Arguments are not available. Please Check the arguments.\n");
	clcd_write_string("ERROR 1");

	usleep(1000000);
}


// TI state
void state_TI(int* input_time, int* digit_num){
	int key_num = -1;		
	int enter_flag = 0;	
	int loop_count = 0;
	int input_key = -1;
	int i = 0;
	int j = 0;

	//key_num = 11;				// These 3 assignment is for test
	//enter_flag = 1;
	//loop_count = 2;

	s_TI_clcd();														// static function | CLCD		
	// s_TI_fnd(key_num, loop_count);	// for test.

	while(enter_flag != 1){
		if (i%4 == 1) {s_TI_clcd();}
		enter_flag = s_TI_keypad(&key_num, &loop_count, &input_key);	// dynamic function | KEYPAD
		s_TI_fnd(key_num, loop_count);									// dynamic function | FND
		s_TI_scheduling(i);												// dynamic function | LED
		s_TI_dot(i);													// dynamic function | DOT

		term_TI(key_num, input_key);									// Terminal window
		i++;
		usleep(1000000);			// Sampling frequency : 1Hz

	}
	sleep(2);					
	*input_time = key_num;
	*digit_num = loop_count;
}


void state_WS(int input_time, int digit_num){
	// s_WS_led(input_time);
	// s_WS_dot(input_time);
	// s_WS_fnd(input_time, digit_num);
	// s_WS_clcd(input_time);

	// Below codes are combined function
	int i = 0;
	int j = 0;
	while(1){
		if (i == input_time+1){break;}

		//LED
		s_WS_stack(i%9);

		// Dot matrix
		for(j = 0; j<MAX_DOT; j++){
			s_WS_tornado(i%6, j);
		}

		// FND
		fnd_deca_number(input_time-i, digit_num);

		// CLCD
		s_WS_scheduling(i);
		
		// Terminal
		term_WS(input_time - i);

		i++;
		usleep(500000);		// Delay for 0.5s. This line control operation time of WS state
	}

}

void state_WD(){
	int i = 0;
	int WD_exit_flag = 0;

	s_WD_clcd();						// static function | clcd
	s_WD_fnd();							// static function | fnd

	term_WD();							// terminal

	while(WD_exit_flag != 1){

		s_WD_scheduling(i);				// dynamic function | led	
		s_WD_dot(i);					// dynamic function | dot
		WD_exit_flag = s_WD_keypad();	// dynamic function |keypad

		i++;
		usleep(1000000);				// Sampling frequency : 1Hz
	}
	// s_WD_fnd();
	// s_WD_clcd();
	// s_WD_led();
	// s_WD_dot();


	clcd_clear_display();
	clcd_write_string("All Operation");
	clcd_set_DDRAM(0x40);
	clcd_write_string("Done!");
	term_EXIT(1);			// Exitting start msg | Terminal
	sleep(2);

	clcd_clear_display();		// clear parts
	fnd_clear();
	dot_clear();
	led_clear();
	term_EXIT(2);			// Parts Initialize msg |Terminal
}


// below terminal functions are for cmd window.
// this window will help when debugging or updating program later.
void term_TI(int key_num, int input_key){
	system("clear");
	printf("\n");
	printf("************ [TI state] ************\n");
	printf("* Current input number : %-8d  *\n", key_num);
	printf("* Pressed key number : %-2d          *\n",input_key);
	printf("************************************\n");
	printf("* Sampling Freq : 1Hz              *\n");
	printf("************************************\n\n");
}

void term_WS(int left_time){
	system("clear");
	printf("\n");
	printf("************ [WS state] ************\n");
	printf("*                                  *\n");
	printf("*            Wasing Now...         *\n");
	printf("*				   *\n");
	printf("*         Time left(s) : %-8d *\n", left_time);
	printf("************************************\n\n");
}

void term_WD(int left_time){
	system("clear");
	printf("\n");
	printf("************ [WD state] ************\n");
	printf("*                                  *\n");
	printf("*            Wasing Done!          *\n");
	printf("*				   *\n");
	printf("*          Press Button [B]!       *\n");
	printf("*				   *\n");
	printf("************************************\n\n");
}

void term_EXIT(int step){
	if (step == 1){
		system("clear");
		printf("\n");
		printf("************** [EXIT] **************\n");
		printf("*                                  *\n");
		printf("*           EXIT PROGRAM           *\n");
		printf("*				   *\n");
		printf("************************************\n\n");
	}
	else if (step == 2){
		system("clear");
		printf("\n");
		printf("************** [EXIT] **************\n");
		printf("*                                  *\n");
		printf("*           EXIT PROGRAM           *\n");
		printf("*				   *\n");
		printf("*   Initializing All Parts DONE    *\n");
		printf("*				   *\n");
		printf("************************************\n\n");
	}

	else if (step == 3){
		system("clear");
		printf("\n");
		printf("************** [EXIT] **************\n");
		printf("*                                  *\n");
		printf("*           EXIT PROGRAM           *\n");
		printf("*				   *\n");
		printf("*   Initializing All Parts DONE    *\n");
		printf("*				   *\n");
		printf("*       Memory Unmapping  DONE     *\n");
		printf("*				   *\n");
		printf("************************************\n\n");
	}

	else if (step == 4){
		system("clear");
		printf("\n");
		printf("************** [EXIT] **************\n");
		printf("*                                  *\n");
		printf("*           EXIT PROGRAM           *\n");
		printf("*				   *\n");
		printf("*   Initializing All Parts DONE    *\n");
		printf("*				   *\n");
		printf("*       Memory Unmapping  DONE     *\n");
		printf("*				   *\n");
		printf("*          Closing FD Done         *\n");
		printf("*				   *\n");
		printf("************************************\n\n");
	}
}

