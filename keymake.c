#include "keyboard.h"
#include "keymake.h"
#include "headers.h"
#include "globals.h"




void F1_M( void )
{
}



void F2_M( void )
{
}



void F3_M( void )
{
}



void execMake()
{

	if( KBpres & (1 << F1) )
	{
		F1_M();				// F1 pressed
	}

	if( KBpres & (1 << F2) )
	{
		F2_M();				// F2 pressed
	}

	if( KBpres & (1 << F3) )
	{
		F3_M();				// F3 pressed
	}
}



