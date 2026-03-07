#ifndef	_GLOBALS_H
#define	_GLOBALS_H

#include "headers.h"


// Main Process
volatile unsigned char	tmp;
unsigned char		DSbutton;
unsigned char		DSled;


// OS FSM
unsigned char 		OSstate;
unsigned char 		POSTst;


// Display
unsigned char		DSidx;
unsigned char		DSdot;
unsigned char		DSbuf[4];


// Timer
unsigned char 		OSALDIS;
unsigned char		OSALNUM;
unsigned char 		OSSTATE;
unsigned char 		OSALCTR;
unsigned char 		TMcnt;
volatile unsigned char	TMtimer[1];
volatile unsigned int 	TMtick;
volatile unsigned int 	TMdelay;
volatile unsigned char 	TMsec;

// Control
volatile unsigned char	sema;

#endif
