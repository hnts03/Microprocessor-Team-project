//-------| lib/dot.h |-------//
#ifndef	DOT_H
#define	DOT_H 


#include <unistd.h> // usleep()


#define MAX_DOT 5


void init_dot(short * address[]);
void dot_clear();
void dot_write(int number) ;
void dot_on();
void s_init_dot(int init_dot_on);
void s_TI_dot();


#endif
