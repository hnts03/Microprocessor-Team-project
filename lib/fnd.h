//-------| lib/fnd.h |-------//
#ifndef	FND_H
#define	FND_H 


#include <unistd.h> // usleep()

#define MAX_FND 8



void init_fnd(short * address[]);
void fnd_clear();
void fnd_all();
void fnd_write(int hexadecimal, int fnd_num);
void fnd_hexa_number(unsigned long number);
void fnd_0();
void s_init_fnd(int init_fnd_on);
int s_TI_fnd(int key_num, int loop_counter);


#endif
