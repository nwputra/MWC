#include <avr/pgmspace.h>
#include "headers.h"
#include "globals.h"
#include "keyboard.h"
#include "keybreak.h"



void F1_B( void )
{
}



void F2_B( void )
{
}



void F3_B( void )
{
}



void execBreak()
{
	/* key has been released */

	if( KBrels & (1 << F1) )
	{
		F1_B();				// F1 released
	}

	if( KBrels & (1 << F2) )
	{
		F2_B();				// F2 released
	}

	if( KBrels & (1 << F3) )
	{
		F3_B();				// F3 released
	}
}



