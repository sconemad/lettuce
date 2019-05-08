/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce Core devices

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "LettuceCore.h"
#include "LettuceServer.h"
#include "LettuceClient.h"
#include "LettuceRegistry.h"

//===========================================================================
// EEPROM device
//
#ifdef LETTUCE_DEV_EEPROM

#define MAX_EEPROM_BUFFER 100u

//===========================================================================
BYTE ModuleRequest_eeprom(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer)
{
	BYTE* p;
	DWORD len;
	DWORD eaddr = addr;
	eaddr |= (devloc << 16);

	if (cmd == LETTUCE_CMD_SET) {
		len = buffer->size;

		if (len == 0u || len > MAX_EEPROM_BUFFER) {
			return LETTUCE_STATUS_BAD_ARG;
		}

		XEEBeginWrite(eaddr);
		p = buffer->data;
		while (len-- > 0u) {
			XEEWrite(*p++);	
		}
		XEEEndWrite();
		DeleteBuffer(buffer); // Don't bother sending the buffer back
	}

	if (cmd == LETTUCE_CMD_GET) {
		if (!GetBufferDWord(buffer,BUFFER_UINT,&len)) {
			return LETTUCE_STATUS_BAD_ARG;
		}

		if (len > MAX_EEPROM_BUFFER) {
			return LETTUCE_STATUS_BAD_ARG;
		}

		AllocBuffer(buffer,len);
		p = buffer->data;

		XEEBeginRead(eaddr);
		while (len-- > 0u) {
			*p++ = XEERead();	
		}
		XEEEndRead();
	}

	return LETTUCE_STATUS_OK;
}

#endif

//===========================================================================
// AUX device
//
#ifdef LETTUCE_DEV_AUX

//===========================================================================
BYTE ModuleRequest_aux(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer)
{
	BYTE b;
	WORD w;
	float f;

	switch (addr) {

		case AUX_LED:
			if (cmd == LETTUCE_CMD_SET) {
				if (!GetBufferBool(buffer,&b)) {
					return LETTUCE_STATUS_BAD_ARG;
				}
				SetPortBit(LETTUCE_DEV_AUX,5,b);
			}
			SetBufferBool(buffer,GetPortBit(LETTUCE_DEV_AUX,5));
			return LETTUCE_STATUS_OK;
	
		case AUX_TEMPERATURE:
			w = GetPortAnalog(LETTUCE_DEV_AUX,0);
			f = ((float)w - 155.15151515) / 3.1030303;
			SetBufferDWord(buffer,BUFFER_REAL,*((DWORD*)&f));
			return LETTUCE_STATUS_OK;
	}	

	return LETTUCE_STATUS_BAD_ADDR;
}

#endif

//===========================================================================
// PWM device
//
#ifdef LETTUCE_DEV_PWM

static BYTE duty1;
static DWORD period1;

//===========================================================================
void ModuleInit_pwm(void)
{
	// Initialise pwm
	duty1 = 0;
	period1 = (20 * TICK_SECOND/1000);

	// Setup timer2
	IPR1bits.TMR2IP = 1;	// High priority interrupt
	T2CON = 0xFB;
}

//===========================================================================
void ModuleTask_pwm(void)
{
	static TICK t = 0;

	// Get tick count
  if (TickGet() >= t) {
		t = TickGet() + period1;

		if (duty1 != 0u) {
			AUX_2_LAT = 1;
			AUX_4_LAT = 1;
			TMR2 = 0;
			PR2 = duty1;
	
			PIR1bits.TMR2IF = 0;	// Clear interrupt flag
			PIE1bits.TMR2IE = 1;
	
			T2CONbits.TMR2ON = 1;	
		}
	}
}

//===========================================================================
BYTE ModuleRequest_pwm(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer)
{
	switch (addr) {

		case PWM_DUTY1:
			if (cmd == LETTUCE_CMD_SET) {
				if (!GetBufferByte(buffer,BUFFER_UINT,&duty1)) {
					return LETTUCE_STATUS_BAD_ARG;
				}
			}		
			SetBufferByte(buffer,BUFFER_UINT,duty1);
			return LETTUCE_STATUS_OK;

		case PWM_PERIOD1:
			if (cmd == LETTUCE_CMD_SET) {
				if (buffer->type != BUFFER_UINT || buffer->size != 1u) {
					return LETTUCE_STATUS_BAD_ARG;
				}
				period1 = (*buffer->data) * TICK_SECOND/1000;
			}		
			SetBufferByte(buffer,BUFFER_UINT,period1);
			return LETTUCE_STATUS_OK;
	}

	return LETTUCE_STATUS_BAD_ADDR;
}

//===========================================================================
void ModuleISR_pwm(void)
{
	if (PIR1bits.TMR2IF) {
		// TIMER2 - PWM
		AUX_2_LAT = 0;
		AUX_4_LAT = 0;
		T2CONbits.TMR2ON = 0;	
		PIR1bits.TMR2IF = 0;
	}	
}

#endif

//===========================================================================
// COUNTER device
//
#ifdef LETTUCE_DEV_COUNTER

//===========================================================================
BYTE ModuleRequest_counter(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer)
{
	T1CON = 0x02;
	TMR1H = 0;
	TMR1L = 0;
	IPR1bits.TMR1IP = 0;	// Low priority interrupt
	PIR1bits.TMR1IF = 0;	// Clear interrupt flag
	PIE1bits.TMR1IE = 0;	// Disable the interrupt
	
	T1CONbits.TMR1ON = 1;
	DelayMs(*buffer->data);
	T1CONbits.TMR1ON = 0;
	
	if (PIR1bits.TMR1IF) {
		return LETTUCE_STATUS_BAD_ARG;
	} else {
		SetBufferWord(buffer,BUFFER_UINT,TMR1L | (TMR1H << 8));
		return LETTUCE_STATUS_OK;
	}

	return LETTUCE_STATUS_BAD_ADDR;
}

#endif

//-------------------------------------
// BUTTONS device
//
#ifdef LETTUCE_DEV_BUTTONS

// Saves the button states so changes can be detected
static BYTE button_states;

//===========================================================================
void ModuleTask_buttons(void)
{
	unsigned int j;
	BYTE new_states;
	DWORD addr;

	// Get current button states
	new_states = (BUTTON0_IO * 1 +
								BUTTON1_IO * 2 + 
								BUTTON2_IO * 4 + 
								BUTTON3_IO * 8 + 
								BUTTON4_IO * 16 + 
								BUTTON5_IO * 32);

	if (button_states != new_states) {
		// One or more button states have changed

		for (j=0u; j<6u; ++j) {
			if ( (new_states & (1<<j)) != (button_states & (1<<j)) ) {
				addr = ((DWORD)LETTUCE_DEV_BUTTONS << 24) + j;
				BroadcastState(addr,!!(new_states & (1<<j)));
			}
		}
		button_states = new_states;
	}
}

//===========================================================================
BYTE ModuleRequest_buttons(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer)
{
	switch (addr) {

		case BUTTONS_0:
			SetBufferBool(buffer,BUTTON0_IO);
			return LETTUCE_STATUS_OK;

		case BUTTONS_1:
			SetBufferBool(buffer,BUTTON1_IO);
			return LETTUCE_STATUS_OK;

		case BUTTONS_2:
			SetBufferBool(buffer,BUTTON2_IO);
			return LETTUCE_STATUS_OK;

		case BUTTONS_3:
			SetBufferBool(buffer,BUTTON3_IO);
			return LETTUCE_STATUS_OK;

		case BUTTONS_4:
			SetBufferBool(buffer,BUTTON4_IO);
			return LETTUCE_STATUS_OK;

		case BUTTONS_5:
			SetBufferBool(buffer,BUTTON5_IO);
			return LETTUCE_STATUS_OK;
	}	

	return LETTUCE_STATUS_BAD_ADDR;
}

#endif

//===========================================================================
// LINBUS device
//
#ifdef LETTUCE_DEV_LINBUS

//===========================================================================
BYTE ModuleRequest_linbus(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer)
{
	if (devloc != 0) {
		return LETTUCE_STATUS_BAD_ADDR;
	}	

	if (addr = 0xFFFF) {
	}	

	return LETTUCE_STATUS_BAD_ADDR;
}

#endif
