/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce RF433 plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/SNTP.h"
#include "plugin_rf433.h"
#include "LettuceServer.h"
#include "sralloc.h"

#ifdef LETTUCE_PLUGIN_RF433

//-------------------------------------
// RF12B commands
//
#define CMD_READ						0x0000
#define CMD_WRITE						0x8000

//-------------------------------------
// RF12B registers
//
#define ADDR_CONFIG 				0x0000
#define ADDR_POWER					0x0200
#define ADDR_FREQ						0x2000
#define ADDR_DATARATE				0x4600
#define ADDR_RXCON					0x1000
#define ADDR_DATAFILTER			0x4200
#define ADDR_FIFORESET			0x4A00
#define ADDR_SYNCHRON				0x4E00
#define ADDR_RECV						0x3000
#define ADDR_AFC						0x4400
#define ADDR_TXCON					0x1800
#define ADDR_PLLSET					0x4C12
#define ADDR_SEND						0x3800
#define ADDR_WAKEUP					0x6000
#define ADDR_LOWDUTY				0x4800
#define ADDR_LOWBATT				0x4000

//-------------------------------------
// rf433 plugin instance data
//
typedef struct _RF433_DATA {
	BYTE dummy;
} RF433_DATA;

//===========================================================================
// Plugin event handler
//
void PluginEvent_rf433(LETTUCE_PLUGIN* p, BYTE event)
{
	RF433_DATA* d;

	d = (RF433_DATA*)p->mem;

	switch (event) {

		case EVENT_OPEN:
			// Disable interrupts during initialisation
			SetPluginInt(p->dev,FALSE);

			// Set associated port to output all zeros as it isn't used
			SetPortByte(p->dev,0x00);
			SetPortTristate(p->dev,0x00);

			p->mem = SRAMalloc(sizeof(RF433_DATA));
			d = ((RF433_DATA*)p->mem);
			break;

		case EVENT_CLOSE:
			// Clean up our buffer
			SRAMfree(p->mem);
			p->mem = 0;
			break;
	}
}

//===========================================================================
// Plugin request handler
//
BYTE PluginRequest_rf433(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer)
{
	switch (addr) {
		
		
		
	}	

	return LETTUCE_STATUS_BAD_ADDR;
}

#endif
