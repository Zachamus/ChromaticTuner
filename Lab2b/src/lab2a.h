/*****************************************************************************
* lab2a.h for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#ifndef lab2a_h
#define lab2a_h

enum Lab2ASignals {
	ENCODER_UP = Q_USER_SIG,
	ENCODER_DOWN,
	ENCODER_CLICK,
	TICK,
	MODE1,
	MODE2,
	MODE3,
	MODE4,
	MODE5,
	MODE,
	HIDE
};


extern struct Lab2ATag AO_Lab2A;


void LCD_ctor(void);
void GpioHandler(void *CallbackRef);
void TwistHandler(void *CallbackRef);
void TimerHandler(void *CallbackRef);

#endif  
