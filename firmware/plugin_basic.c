/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce basic plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "plugin_basic.h"
#include "plugin_mem.h"
#include "LettuceServer.h"
#include "LettuceClient.h"
#include "sralloc.h"

#ifdef LETTUCE_PLUGIN_BASIC

//-------------------------------------
// basic plugin instance data
//
typedef struct _BASIC_DATA {
	BYTE save;
	SPIMEM_HANDLE spimem;
} BASIC_DATA;


//===========================================================================
// Plugin event handler
//
void PluginEvent_basic(LETTUCE_PLUGIN* p, BYTE event)
{
	unsigned int i;
	BYTE b,t;
	BASIC_DATA* d;

	d = (BASIC_DATA*)p->mem;

	switch (event) {

		case EVENT_OPEN:

			// Disable interrupts
			SetPluginInt(p->dev,FALSE);

			// Set port tristates to input
			SetPortTristate(p->dev,0xFF);

			// Allocate buffer
			p->mem = SRAMalloc(sizeof(BASIC_DATA));
			p->timer = 0;
			d = ((BASIC_DATA*)p->mem);
			d->spimem = SPIMEM_BAD_HANDLE;

			// Fall through to perform initial scan...

		case EVENT_TICK:
			// Read port inputs
			b = GetPortByte(p->dev) & GetPortTristate(p->dev);
			if (b != d->save) {
				// An input has changed
//				BroadcastEvent("port input change");
				d->save = b;
			}
			break;

		case EVENT_CLOSE:
			// Clean up our buffer
			SRAMfree(p->mem);
			p->mem = 0;
			break;
	}
}

#define BIT_ACCEL_CS	0
#define BIT_MAG_CS		1
#define BIT_MAG_RESET	3
#define BIT_MAG_DRDY	5

#define SAMPLER_MAX 32768
static WORD sampler_addr = SAMPLER_MAX;
static SPIMEM_HANDLE sampler_mem = SPIMEM_BAD_HANDLE;

//===========================================================================
// Plugin request handler
//
BYTE PluginRequest_basic(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer)
{
	BYTE b,channel;
	BASIC_DATA* d;

	d = (BASIC_DATA*)p->mem;
	if (d == 0) {
		return LETTUCE_STATUS_BAD_DEVICE;
	}

	switch (addr) {
		case 0x0A00: // Magnetometer SPI read
			if (buffer->size < 3) {
				return LETTUCE_STATUS_BAD_ARG;
			}

			SetPortBit(p->dev,BIT_ACCEL_CS,1);
			SetPortBit(p->dev,BIT_MAG_CS,1);
			SetPortTristate(p->dev,0xF0);

			OpenSPI(SPI_FOSC_16 | BUS_MODE_00 | SMP_PHASE_MID);
			SelectPlugin(p->dev);
			SetPortBit(p->dev,BIT_MAG_CS,0);
			Delay1KTCYx(10);

			SetPortBit(p->dev,BIT_MAG_RESET,1);
			Delay1KTCYx(10);
			SetPortBit(p->dev,BIT_MAG_RESET,0);
			Delay1KTCYx(10);

			WriteSPI(*(buffer->data));

			while ( !(GetPortByte(p->dev) & (1<<BIT_MAG_DRDY) )) {};

			b = WriteSPI(*(buffer->data + 1));
			*(buffer->data) = b;
			b = WriteSPI(*(buffer->data + 2));
			*(buffer->data + 1) = b;
			buffer->size = 2;

			SetPortBit(p->dev,BIT_MAG_CS,1);
			SelectPlugin(LETTUCE_DEV_NONE);
			CloseSPI();
			return LETTUCE_STATUS_OK;

		case 0x0A01: // Accelerometer SPI read
			if (buffer->size < 2) {
				return LETTUCE_STATUS_BAD_ARG;
			}

			SetPortBit(p->dev,BIT_ACCEL_CS,1);
			SetPortBit(p->dev,BIT_MAG_CS,1);
			SetPortTristate(p->dev,0xF0);

			OpenSPI(SPI_FOSC_16 | BUS_MODE_11 | SMP_PHASE_MID);
			SelectPlugin(p->dev);
			SetPortBit(p->dev,BIT_ACCEL_CS,0);
			Delay1KTCYx(10);

			WriteSPI(*(buffer->data));

			b = WriteSPI(*(buffer->data + 1));
			*(buffer->data) = b;
			buffer->size = 1;

			SetPortBit(p->dev,BIT_ACCEL_CS,1);
			SelectPlugin(LETTUCE_DEV_NONE);
			CloseSPI();
			return LETTUCE_STATUS_OK;
			
		case 0x0B00: // Start sampler
			if (sampler_mem == SPIMEM_BAD_HANDLE) {
				sampler_mem = SPIMEMalloc();	
			}
			if (sampler_mem == SPIMEM_BAD_HANDLE) {
				return LETTUCE_STATUS_BAD_DEVICE;
			}

			ENTER_CRITICAL;
			b = GetPortByte(p->dev);
			while (GetPortByte(p->dev) == b) {
				ClrWdt();
			}	
			for (sampler_addr=0; sampler_addr<SAMPLER_MAX; ++sampler_addr) {
				ClrWdt();
				b = GetPortByte(p->dev);
				SPIMEMPutArray(sampler_mem,sampler_addr,&b,1);
			}	
			LEAVE_CRITICAL;
			
			return LETTUCE_STATUS_OK;	
	}

	// Channel read/write
	
	channel = (addr & 0x00FF);
	if (channel >= 8) {
		return LETTUCE_STATUS_BAD_ARG;
	}

	switch (cmd) {

		case LETTUCE_CMD_GET:
			SetBufferBool(buffer,!!(GetPortByte(p->dev) & (1<<channel)));
			return LETTUCE_STATUS_OK;

		case LETTUCE_CMD_SET:
			if (buffer->type != BUFFER_BOOL || buffer->size != 1) {
				return LETTUCE_STATUS_BAD_ARG;
			}
			SetPortBit(p->dev,channel,*buffer->data);
			return LETTUCE_STATUS_OK;

		case LETTUCE_CMD_DIR: // Set IO direction on port
			if (buffer->type != BUFFER_BOOL || buffer->size != 1) {
				return LETTUCE_STATUS_BAD_ARG;
			}

			b = GetPortTristate(p->dev);

			// Update tris with required bit
			if (*buffer->data) {
				b |= (1<<channel);
			} else {
				b &= ~(1<<channel);
			}

			SetPortTristate(p->dev,b);
			return LETTUCE_STATUS_OK;
	}

	return LETTUCE_STATUS_BAD_ADDR;
}

//===========================================================================
// Handle interrupts
//
void PluginISR_basic(LETTUCE_PLUGIN* p, LETTUCE_INT_TYPE type)
{

}	

#endif
