
/* ====================================
	coded by Mikolaj Feliks aka Majuma
	mail: majuma@wp.pl
	web: http://www.majuma.xt.pl
	all rights reserved
   ==================================== */


#ifndef _timer_h
#define _timer_h

#define Wait(a) { GlobalCounter = 0; while (GlobalCounter < a); }



extern int GlobalCounter, GlobalSec;

//------------------------------------------------------------

void SetTimer(void);
void RestoreTimer(void);


#endif
