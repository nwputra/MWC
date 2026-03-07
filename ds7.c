// 7 Segment Display 
// YB1NWP (c) 2026
// version 1.0
//
//

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "headers.h"
#include "ds7.h"



/********* DSinit ***********
;
; Subroutine	: DSinit
; Author	: N. Putra
; Created	: 04/29/03
;
; Description   	: This routine will initialize Display subsystem
;			: 
; Register Affected     : Flags set accordingly
; Returns 		: Non zero in case of error
;
*/
void DSinit( void )
{
	DSidx	= 0;
}



/********** DSrefr ***********
;
; Subroutine	: DSrefr
; Author	: N. Putra
; Created	: 03/01/2026
;
; Description   	: This routine will refresh 7segment
;			: periodically (called every 5mS)
;			:
;
; Register Affected     : Flags set accordingly
*/
void DSrefr( void )
{
	// Disable display
	PORTC = 0x3F;

	if( DSidx == 4 )
	{
		// Dots
		DSidx = 0;

		// Disable all segments but dots
		PORTD = DDOT;

		// Check which dot is active
		if( DSdot & 0x01 )
		{
			// Upper dot (bit 1)
			PORTC = PORTC & 0xFD;
		}

		if( DSdot & 0x02 )
		{
			// Lower dot (bit 0)
			PORTC = PORTC & 0xFE;
		}
	}
	else
	{
		// Disable display
		PORTC = 0x3F;

		// Update Data bits
		PORTD = DSbuf[ DSidx ];

		// Enable display
		// Display is active LOW
		//
		PORTC = ~(1 << DSidx);

		// Next digit
		DSidx++;
	}
}



void DSwr( unsigned char pos, unsigned char dt )
{
#ifdef HWREWIRE
	// Hardware rewired so that digit index is logical
	if( pos < 4 )
	{
		DSbuf[pos] = dt;
	}

#else
	// Hardware index is not logical
	//
	switch( pos )
	{
		case 0:	// First digit - on 1st buffer
			DSbuf[0] = dt;
			break;

		case 1:	// Second digit - on 3rd buffer
			DSbuf[2] = dt;
			break;

		case 2:	// Third digit - on 4th buffer
			DSbuf[3] = dt;
			break;

		case 3:	// Fourth digit - on 2nd buffer
			DSbuf[1] = dt;
			break;

		default:
			// Ignore
			break;
	}
#endif
}
