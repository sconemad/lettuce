/* Lettuce (http://www.sconemad.com/lettuce)
 
Plugin Controller

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "PluginController.h"
#include "LettuceCore.h"
#include "LettuceSystem.h"

// Plugins:
#include "plugin_basic.h"
#include "plugin_ioport.h"
#include "plugin_mem.h"
#include "plugin_rtc.h"
#include "plugin_mp3.h"


//-------------------------------------------------------------------
// Plugin class table
//
// Circuit diagram for plugin slot SID system:
//                                                          Vdd
// SID                                                 Rb    |
// ADC <-----+-----------+-----------+-----------+----####---+
//          _|_         _|_         _|_         _|_   
//          \ /         \ /         \ /         \ /
//          ---         ---         ---         ---
//           |           |           |           |
//           #           #           #           #
//           # Rp1       # Rp2       # Rp3       # Rp4
//           #           #           #           #
//           |           |           |           |
//          SS1         SS2         SS3         SS4
//
// A plugin slot's SSn pin is pulled to Gnd when measuring the SID
// value for the plugin. The expected ADC reading (SID_NOMINAL) can 
// be calculated by the following formula:
//
// SID_NOMINAL = (N/Vdd) * ( (Vdiode*Rb + Vdd*Rp) / (Rb+Rp) )
// 
// where:
//  N 			= 1024	Number of ADC steps
//  Vdd 		= 3.3		Supply voltage and ADC ref voltage
//  Vdiode 	= 0.6		Volts across diode (found empirically)
//  Rb			= 4700	SID pullup resistor value
//  Rp			= SID resistor value for the plugin
//
// SID_LOWER and SID_UPPER correspond to the values calculated when
// using Rp with errors of -20% and +20% respectively.
//
const rom LETTUCE_PLUGIN_CLASS plugin_classes[] = {
// TYPE, NAME, SPICON, SID_LOWER, SID_UPPER // SID_NOMINAL(20degC, 7degC) Rp
//-------------------------------------------------------------------
	{LETTUCE_PLUGIN_BASIC, (rom char*)"basic", SPI_FOSC_16 | BUS_MODE_00 | SMP_PHASE_MID, 0,189}, // 186 0R

#ifdef LETTUCE_PLUGIN_MP3
	{LETTUCE_PLUGIN_MP3, (rom char*)"mp3", SPI_FOSC_16 | BUS_MODE_00 | SMP_PHASE_MID, 190, 220}, // 204,215 100R 191
#endif
#ifdef LETTUCE_PLUGIN_RTC
	{LETTUCE_PLUGIN_RTC, (rom char*)"rtc", SPI_FOSC_16 | BUS_MODE_11 | SMP_PHASE_MID, 226,294}, // 262,271 470R 247
#endif
#ifdef LETTUCE_PLUGIN_MEM
	{LETTUCE_PLUGIN_MEM, (rom char*)"mem", SPI_FOSC_4 | BUS_MODE_00 | SMP_PHASE_MID, 300,435}, // 333,339 1K 319
#endif
#ifdef LETTUCE_PLUGIN_IOPORT
	{LETTUCE_PLUGIN_IOPORT, (rom char*)"ioport", SPI_FOSC_4 | BUS_MODE_00 | SMP_PHASE_MID, 620,864}, // 756,754 10K 745
#endif

// --- Add new plugins above here, the following must be last in the list ---
	{LETTUCE_DEV_NONE, (rom char*)"none", SPI_FOSC_16 | BUS_MODE_00 | SMP_PHASE_MID, 0,0} // 1023 (inf)
};
//-------------------------------------------------------------------

// Number of consistent SID reads required when determining whether a 
// plugin has changed
#define LETTUCE_PLUGIN_CHANGE_TRIALS 10

// Interval (in seconds) at which to look for plugin changes
#define LETTUCE_PLUGIN_DETECT_INTERVAL (10*TICK_SECOND)

#define TIMER3_FREQ			1800ul	// Hz 555us

#define STATE_RESET					0
#define STATE_START_DET			1
#define STATE_REPEAT_DET		2
#define STATE_END_DET				3
#define STATE_CLOSE_PLUGIN	4
#define STATE_OPEN_PLUGIN		5
#define STATE_WAIT					6

// Plugin array
static volatile LETTUCE_PLUGIN plugins[LETTUCE_NUM_SLOTS];

// Plugin controller state
static BYTE state;

BYTE servo1;

//===========================================================================
// Init plugin controller
//
void PluginControllerInit(void)
{
	BYTE i;
	LETTUCE_PLUGIN* p;

	// Set all plugins to none
	for (i=0; i<LETTUCE_NUM_SLOTS; ++i) {
		p = &plugins[i];
		p->dev = i+1;
		p->cls = LookupClass(1024);
		p->mem = 0;
		p->timer = 0;
		p->flags.intf = 0;
	}

	// Set up Timer 3 for use with plugins
	T3CON = 0xB1;	// 16-bit mode, 1:8 prescale, internal clock, timer enabled

	TMR3H = 0xFF; // Set timer to interrupt immediately, then it will be reset
	TMR3L = 0xFF; // with the correct value.

	IPR2bits.TMR3IP = 0;	// Low priority interrupt
	PIR2bits.TMR3IF = 0;	// Clear interrupt flag
	PIE2bits.TMR3IE = 1;	// Enable the interrupt

	// Set up PSP interrupt
	IPR1bits.PSPIP = 0;
	PIR1bits.PSPIF = 0;
	PIE1bits.PSPIE = 1;
	
	// Run the task a few times to detect plugins
	state = STATE_RESET;
	do {
		PluginControllerTask();		
	}	while (state != STATE_WAIT);
}

//===========================================================================
// Update plugins
//
void PluginControllerTask(void)
{
	BYTE i;
	LETTUCE_PLUGIN* p;
	static TICK timeout = 0;
	static BYTE det_current = 0;
	static rom LETTUCE_PLUGIN_CLASS* det_cls;
	static BYTE det_repeats;

	p = &plugins[det_current];

	switch (state) {	
		
		case STATE_RESET:
			det_current = 0;
			state = STATE_START_DET;
			break;

		case STATE_START_DET:
			// Check for SID changes, to detect plugging-in or unplugging
			det_cls = LookupClass(GetPluginID(p->dev));
			if (det_cls->type != p->cls->type) {
				det_repeats = 0;
				state = STATE_REPEAT_DET;
			} else {
				state = STATE_END_DET;
			}
			break;
		
		case STATE_REPEAT_DET:
			// Check for SID changes, to detect plugging-in or unplugging
			if (det_cls->type != LookupClass(GetPluginID(p->dev))->type) {
				// Inconsistent results
				state = STATE_END_DET;
			}	else if (det_repeats++ >= LETTUCE_PLUGIN_CHANGE_TRIALS) {
				// Plugin SID has changed, switch over
				state = STATE_CLOSE_PLUGIN;
			}
			break;
			
		case STATE_CLOSE_PLUGIN:	
			Log(p->cls->type,det_current,(rom char*)"plugin close");
			PluginEvent(p,EVENT_CLOSE);
			p->cls = LookupClass(1024);
			state = STATE_OPEN_PLUGIN;
			break;
			
		case STATE_OPEN_PLUGIN:
			Log(p->cls->type,det_current,(rom char*)"plugin open");
			p->cls = det_cls;
			PluginEvent(p,EVENT_OPEN);
			Log(p->cls->type,det_current,(rom char*)"plugin ready");
			state = STATE_END_DET;
			break;

		case STATE_END_DET:
			if (++det_current < LETTUCE_NUM_SLOTS) {
				state = STATE_START_DET;
			} else {	
				timeout = TickGet() + LETTUCE_PLUGIN_DETECT_INTERVAL;
				state = STATE_WAIT;
			}	
			break;
		
		case STATE_WAIT:
		  if (TickGet() >= timeout) {
			  state = STATE_RESET;
			}
			break;
	}
			
	// Loop over each plugin slot
	for (i=0; i<LETTUCE_NUM_SLOTS; ++i) {
		p = &plugins[i];

		// Run plugin tick update
		PluginEvent(p,EVENT_TICK);
	}
}


//===========================================================================
// Send state event to all plugins
//
void PluginControllerStateEvent(void)
{
	BYTE i;
	LETTUCE_PLUGIN* p;

	// Loop over each plugin slot
	for (i=0; i<LETTUCE_NUM_SLOTS; ++i) {
		p = &plugins[i];

		// Send plugin state event
		PluginEvent(p,EVENT_STATE);
	}
}


//===========================================================================
// Dispatch interrupts
//
void PluginControllerHighISR(void)
{
	static BYTE i;
	static LETTUCE_PLUGIN* p;

	// Loop over each plugin slot and check for peripheral interrupt
	for (i=0; i<LETTUCE_NUM_SLOTS; ++i) {
		p = &plugins[i];
		if (GetPluginInt(p->dev)) {
			// Clear interrupt flag
//			SetPluginInt(p->dev,FALSE);
			ClearPluginInt(p->dev);

			// Set the plugin int flag
			p->flags.intf = 1;

			// We want to handle all peripheral interrupts as low priority. 
			// In order to work around the fact that the INT0 interrupt has no 
			// priority selection (it is always high priority), a low priority PSP 
			// interrupt is faked, which is handled in the LowISR as if it were a 
			// peripheral interrupt. (PSP functionality is not used by Lettuce).
			PIR1bits.PSPIF = 1;
		}
	}
}

//===========================================================================
// Dispatch interrupts
//
void PluginControllerLowISR(void)
{
	static BYTE i;
	static LETTUCE_PLUGIN* p;

	if (PIR2bits.TMR3IF) {
		// TIMER3 - Plugin timers

		// Loop over each plugin slot and dispatch timer interrupts
		for (i=0; i<LETTUCE_NUM_SLOTS; ++i) {
			p = &plugins[i];
			if (p->timer && (--(p->timer) == 0)) {
				PluginISR(p,INT_TYPE_TIMER3);
			}
		}

		// Clear interrupt flag
		PIR2bits.TMR3IF = 0;

		// Reset timer (to -724)
		TMR3H = HIGHBYTE(-((GetPeripheralClock()/8 + TIMER3_FREQ/2)/TIMER3_FREQ));
		TMR3L = LOWBYTE(-((GetPeripheralClock()/8 + TIMER3_FREQ/2)/TIMER3_FREQ));
	}
	
	if (PIR1bits.PSPIF) {
		// PSP - Fake low priority peripheral interrupt (see note above)

		// Clear interrupt flag
		PIR1bits.PSPIF = 0;

		// Loop over each plugin slot and check for interrupt flag
		for (i=0; i<LETTUCE_NUM_SLOTS; ++i) {
			p = &plugins[i];
			if (p->flags.intf) {
				// Handle peripheral interupt as low priority
				PluginISR(p,INT_TYPE_PERIPHERAL);

//				SetPluginInt(p->dev,TRUE);

				// Reset the plugin int flag
				p->flags.intf = 0;
			}
		}
	}
}

//===========================================================================
// Get plugin by device
//
LETTUCE_PLUGIN* GetPlugin(BYTE dev, BYTE index)
{
	BYTE i;
	LETTUCE_PLUGIN* p;

	if (dev >= LETTUCE_DEV_SPI_SLOT_1 && 
			dev < LETTUCE_DEV_SPI_SLOT_1 + LETTUCE_NUM_SLOTS) {
		// dev is an absolute (slot) ID.
		return &plugins[dev-LETTUCE_DEV_SPI_SLOT_1];
	}

	if (dev >= LETTUCE_DEV_PLUGIN) {
		// dev is a plugin type, search for that plugin
		for (i=0; i<LETTUCE_NUM_SLOTS; ++i) {
			p = &plugins[i];
			if (p->cls->type == dev) {
				if (index-- == 0) {
					return p;
				}
			}
		}
	}

	return 0;
}

//===========================================================================
// Lookup index of plugin from slot ID
//
BYTE LookupIndex(LETTUCE_PLUGIN* p)
{
	BYTE i;
	BYTE index=0;

	if (p && 
			p->dev >= LETTUCE_DEV_SPI_SLOT_1 && 
			p->dev < LETTUCE_DEV_SPI_SLOT_1 + LETTUCE_NUM_SLOTS) {
		// dev is an absolute (slot) ID.
		for (i=0; i<(p->dev-1); ++i) {
			if (plugins[i].cls->type == p->cls->type) {
				++index;
			}
		}		
	}

	return index;
}

//===========================================================================
// Lookup plugin class by SID
//
rom LETTUCE_PLUGIN_CLASS* LookupClass(WORD sid)
{
	rom LETTUCE_PLUGIN_CLASS* cls;
	for (cls = &plugin_classes[0]; 
			 cls->type != LETTUCE_DEV_NONE;
			 ++cls) {
		if (sid >= cls->sidl && sid <= cls->sidu) {
			break;
		}
	}
	// NOTE: if not found, then the NONE class will be returned (end of list)
	return cls;
}

//===========================================================================
// Send an event to a lettuce plugin
//
void PluginEvent(LETTUCE_PLUGIN* p, BYTE event)
{
	switch (p->cls->type) {

#ifdef LETTUCE_PLUGIN_BASIC
		case LETTUCE_PLUGIN_BASIC:
			PluginEvent_basic(p,event);
			break;
#endif

#ifdef LETTUCE_PLUGIN_IOPORT
		case LETTUCE_PLUGIN_IOPORT:
			PluginEvent_ioport(p,event);
			break;
#endif

#ifdef LETTUCE_PLUGIN_MEM
		case LETTUCE_PLUGIN_MEM:
			PluginEvent_mem(p,event);
			break;
#endif

#ifdef LETTUCE_PLUGIN_RTC
		case LETTUCE_PLUGIN_RTC:
			PluginEvent_rtc(p,event);
			break;
#endif

#ifdef LETTUCE_PLUGIN_MP3
		case LETTUCE_PLUGIN_MP3:
			PluginEvent_mp3(p,event);
			break;
#endif

		default: // Unknown or no plugin
			if (event == EVENT_OPEN) {
				// Disable interrupts
				SetPluginInt(p->dev,FALSE);
	
				// Set port tristates to input for safety
				SetPortTristate(p->dev,0xFF);

				// Disable timer
				p->timer = 0;
			}
			break;
	}
}

//===========================================================================
// Send a request to a lettuce plugin
// See LettuceServer
//
BYTE PluginRequest(BYTE cmd, BYTE devtype, BYTE devloc, WORD addr, BUFFER* buffer)
{
	LETTUCE_PLUGIN* p = GetPlugin(devtype,devloc);
	BYTE i;
	BYTE b;

	if (p == 0) {
		return LETTUCE_STATUS_BAD_DEVICE;
	}

	// Handle standard addresses
	switch (addr) {
		case 0xFF00: // Plugin name
			SetBufferRomString(buffer, p->cls->name);
			return LETTUCE_STATUS_OK;

		case 0xFF01: // Plugin raw ID reading
			SetBufferWord(buffer,BUFFER_UINT,GetPluginID(p->dev));
			return LETTUCE_STATUS_OK;

		case 0xFF02: // Plugin interrupt state
			SetBufferBool(buffer,GetPluginInt(p->dev));
			return LETTUCE_STATUS_OK;

		case 0xFF10: // SPI transfer
			OpenSPI(p->cls->spicon);
			SelectPlugin(p->dev);
			for (i=0; i<buffer->size; ++i)
			{
				b = WriteSPI(*(buffer->data + i));
				*(buffer->data + i) = b;
			}
			SelectPlugin(LETTUCE_DEV_NONE);
			CloseSPI();
			return LETTUCE_STATUS_OK;

		case 0xFF11: // SPI transfer with conf
			OpenSPI(*buffer->data);
			SelectPlugin(p->dev);
			for (i=1; i<buffer->size; ++i)
			{
				b = WriteSPI(*(buffer->data + i));
				*(buffer->data + i) = b;
			}
			SelectPlugin(LETTUCE_DEV_NONE);
			CloseSPI();
			return LETTUCE_STATUS_OK;

/*
		case CMD_PORT_TRIS: // Set port tristate: BYTE tris
			TCPGet(socket,&bval);
			SetPortTristate(dev,bval);
			break;

		case CMD_PORT_INPUT: // Get port input: return BYTE value
			TCPPut(socket,GetPortByte(dev));
			break;

		case CMD_PORT_OUTPUT: // Set port output: BYTE value
			TCPGet(socket,&bval);
			SetPortByte(dev,bval);
			break;

		case CMD_PORT_ANALOG: // Get port analog input: BYTE pin, return WORD value
			TCPGet(socket,&bval);
			wval = GetPortAnalog(dev,bval);
			TCPPutArray(socket,(BYTE*)&wval,2);
			break;
*/
	}

	// Pass to plugin code to handle anything else
	switch (p->cls->type) {

#ifdef LETTUCE_PLUGIN_BASIC
		case LETTUCE_PLUGIN_BASIC:
			return PluginRequest_basic(p,cmd,addr,buffer);
#endif

#ifdef LETTUCE_PLUGIN_IOPORT
		case LETTUCE_PLUGIN_IOPORT:
			return PluginRequest_ioport(p,cmd,addr,buffer);
#endif

#ifdef LETTUCE_PLUGIN_MEM
		case LETTUCE_PLUGIN_MEM:
			return PluginRequest_mem(p,cmd,addr,buffer);
#endif

#ifdef LETTUCE_PLUGIN_RTC
		case LETTUCE_PLUGIN_RTC:
			return PluginRequest_rtc(p,cmd,addr,buffer);
#endif

#ifdef LETTUCE_PLUGIN_MP3
		case LETTUCE_PLUGIN_MP3:
			return PluginRequest_mp3(p,cmd,addr,buffer);
#endif

	}

	return LETTUCE_STATUS_BAD_DEVICE;
}

 
//===========================================================================
// Send interrupt to a lettuce plugin
//
void PluginISR(LETTUCE_PLUGIN* p, LETTUCE_INT_TYPE type)
{
	switch (p->cls->type) {

#ifdef LETTUCE_PLUGIN_IOPORT
		case LETTUCE_PLUGIN_IOPORT:
			PluginISR_ioport(p,type);
			break;
#endif

	// mem doesn't use interupts

#ifdef LETTUCE_PLUGIN_RTC
		case LETTUCE_PLUGIN_RTC:
//			PluginISR_rtc(p,type);
			break;
#endif

#ifdef LETTUCE_PLUGIN_MP3
		case LETTUCE_PLUGIN_MP3:
			PluginISR_mp3(p,type);
			break;
#endif

#ifdef LETTUCE_PLUGIN_BASIC
		case LETTUCE_PLUGIN_BASIC:
			PluginISR_basic(p,type);
			break;
#endif
	}
}
