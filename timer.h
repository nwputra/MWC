#ifndef _OStimer
#define _OStimer

void TMtmr2init( void );

extern unsigned char	OSALDIS;
extern unsigned char	OSALNUM;
extern unsigned char	OSSTATE;
extern unsigned char	TMcnt;

extern volatile unsigned int	TMtick;
extern volatile unsigned int	TMdelay;
extern volatile unsigned char	TMpmh;
extern volatile unsigned char	TMsec;
extern volatile unsigned char	TMdtc;

#endif
