/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce memory plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "plugin_mem.h"
#include "LettuceServer.h"
#include "sralloc.h"

#ifdef LETTUCE_PLUGIN_MEM

//-------------------------------------
// AMI N256SO8xxHDA commands:
//
#define CMD_READ	0x03
#define CMD_WRITE	0x02
#define CMD_RDSR	0x05
#define CMD_WRSR	0x01

// Memory types
#define MEM_NONE				0
#define MEM_RAM					1
#define MEM_EEPROM			2

// Maximum number of chips
#define MEM_NUM_CHIPS 	8

#define PAGE 						32
#define MODIFY 					0xCE

static BYTE mem_spicon = 0;

//-----------------------------------
// SPI memory chip
//
typedef struct _MEM_CHIP {
	BYTE 		type;
	DWORD 	size;
	struct {
		unsigned char used : 1;
	} flags;
} MEM_CHIP;

//-------------------------------------
// mem plugin instance data
//
typedef struct _MEM_DATA {
	MEM_CHIP chips[MEM_NUM_CHIPS];
} MEM_DATA;


//===========================================================================
// Plugin event handler
//
void PluginEvent_mem(LETTUCE_PLUGIN* p, BYTE event)
{
	unsigned int i;
	WORD a;
	BYTE b,bs;
	MEM_CHIP* chip;
	MEM_DATA* d;

	d = (MEM_DATA*)p->mem;

	switch (event) {

		case EVENT_OPEN:
			// Disable interrupts as we use polling for now
			SetPluginInt(p->dev,FALSE);

			// Make a local copy of the spicon for later use
			mem_spicon = p->cls->spicon;

		  // Set up SPI module
			OpenSPI(p->cls->spicon);
		
			// Set all outputs high, unselecting all chips
			SetPortByte(p->dev,0xFF);
		
			// Set port to all outputs
			SetPortTristate(p->dev,0x00);

			// Select plugin
			SelectPlugin(p->dev);
		
			// Allocate buffer
			p->mem = SRAMalloc(sizeof(MEM_DATA));
			d = ((MEM_DATA*)p->mem);

			for (i=0; i<MEM_NUM_CHIPS; ++i) {
				chip = &d->chips[i];
				chip->type = MEM_NONE;
				chip->size = 0;
				chip->flags.used = 0;

				// Attempt a read
				SetPortBit(p->dev,i,0);
				WriteSPI(CMD_READ);
				WriteSPI(0); // Address
				WriteSPI(0);
				bs = WriteSPI(0);
				SetPortBit(p->dev,i,1);

				Nop(); Nop();
		
				// Attempt to write back modified byte
				SetPortBit(p->dev,i,0);
				WriteSPI(CMD_WRITE);
				WriteSPI(0); // Address
				WriteSPI(0);
				WriteSPI(bs + MODIFY);
				SetPortBit(p->dev,i,1);
		
				Nop(); Nop();

				// Attempt to read back modified byte
				SetPortBit(p->dev,i,0);
				WriteSPI(CMD_READ);
				WriteSPI(0); // Address
				WriteSPI(0);
				b = WriteSPI(0);
				SetPortBit(p->dev,i,1);

				Nop(); Nop();

				// Attempt to write back original byte
				SetPortBit(p->dev,i,0);
				WriteSPI(CMD_WRITE);
				WriteSPI(0); // Address
				WriteSPI(0);
				WriteSPI(bs);
				SetPortBit(p->dev,i,1);
		
				// See if we read back what we originally wrote
				if (b == (bs + MODIFY)) {
					// Success, found something
					chip->type = MEM_RAM;
					chip->size = 0x8000; // probably one of those 32KB SRAM chips
		
					SetPortBit(p->dev,i,0);
					WriteSPI(CMD_WRSR);
					WriteSPI(0x40); // Enable burst mode
					SetPortBit(p->dev,i,1);
				}
			}
		
			// Unselect plugin
			SelectPlugin(LETTUCE_DEV_NONE);
			CloseSPI();
			break;

		case EVENT_CLOSE:
			// Clean up our buffer
			SRAMfree(p->mem);
			p->mem = 0;
			break;
	}
}

#define REQ_BLOCK_SIZE 64

//===========================================================================
// Plugin request handler
//
BYTE PluginRequest_mem(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer)
{
	WORD mem_addr;
	SPIMEM_HANDLE handle;

	if (cmd != LETTUCE_CMD_GET) {
		return LETTUCE_STATUS_BAD_CMD;
	}	

	if (addr >= MEM_NUM_CHIPS) {
		return LETTUCE_STATUS_BAD_ADDR;
	}
		
	// Determine the handle (addr gives the chip number)
	handle = (addr << 4) | p->dev;
	
	if (!GetBufferWord(buffer,BUFFER_UINT,&mem_addr)) {
		return LETTUCE_STATUS_BAD_ARG;
	}

	AllocBuffer(buffer,REQ_BLOCK_SIZE);
	SPIMEMGetArray(handle,mem_addr,buffer->data,REQ_BLOCK_SIZE);
	
	return LETTUCE_STATUS_OK;
}

//===========================================================================
// Allocate from SPIMEM
//
SPIMEM_HANDLE SPIMEMalloc(void)
{
	LETTUCE_PLUGIN* p;
	BYTE ip, ic;
	MEM_CHIP* c;
	MEM_DATA* d;

	for (ip=0; ip<LETTUCE_NUM_SLOTS; ++ip) {
		p = GetPlugin(LETTUCE_PLUGIN_MEM,ip);
		if (!p) {
			break;
		}

		d = (MEM_DATA*)p->mem;
		if (!d) {
			break;
		}

		for (ic=0; ic<MEM_NUM_CHIPS; ++ic) {
			c = &d->chips[ic];
			// Check if its a RAM chip, and not in use
			if (c->type == MEM_RAM && !c->flags.used) {
				// Mark chip as in use and return handle to it
				c->flags.used = 1;
				return ((ic << 4) | p->dev);
			}
		}
	}

	// Failed to allocate
	return SPIMEM_BAD_HANDLE;
}

//===========================================================================
// Free previously allocated SPIMEM
//
void SPIMEMfree(SPIMEM_HANDLE handle)
{
	LETTUCE_PLUGIN* p;
	MEM_CHIP* c;
	MEM_DATA* d;

	p = GetPlugin(handle & 0x0F,0);
	if (!p) {
		return; // Bad handle (plugin number)
	}

	d = (MEM_DATA*)p->mem;
	if (!d) {
		return; // Plugin data not present
	}

	// Shift handle to extract the chip number
	handle &= 0xF0;
	handle = handle >> 4;

	// Check the chip number is in range
	if (handle >= MEM_NUM_CHIPS) {
		return; // Bad handle (chip number)
	}

	// Mark the chip as not in use
	c = &d->chips[handle];
	c->flags.used = 0;
}

//===========================================================================
// Read array from SPIMEM
//
void SPIMEMGetArray(SPIMEM_HANDLE handle, WORD addr, BYTE *data, WORD n)
{
	// Ignore operations when the destination is NULL or nothing to read
	if (data == 0 || n == 0 || handle == SPIMEM_BAD_HANDLE) {
		return;
	}

	OpenSPI(mem_spicon);

	SelectPlugin(handle & 0x0F);
	SetPortBit(handle & 0x0F,(handle & 0xF0) >> 4,0);

	WriteSPI(CMD_READ);
	WriteSPI(((BYTE*)&addr)[1]);
	WriteSPI(((BYTE*)&addr)[0]);

	while (n--) {
		*data++ = WriteSPI(0);
	};
	
	SetPortBit(handle & 0x0F,(handle & 0xF0) >> 4,1);
	SelectPlugin(LETTUCE_DEV_NONE);

	CloseSPI();
}

//===========================================================================
// Read array from SPIMEM
//
void SPIMEMGetArrayISR(SPIMEM_HANDLE handle, WORD addr, BYTE *data, WORD n)
{
	// Ignore operations when the destination is NULL or nothing to read
	if (data == 0 || n == 0 || handle == SPIMEM_BAD_HANDLE) {
		return;
	}

	OpenSPI(mem_spicon);

	SelectPlugin(handle & 0x0F);
	SetPortBit(handle & 0x0F,(handle & 0xF0) >> 4,0);

	WriteSPI(CMD_READ);
	WriteSPI(((BYTE*)&addr)[1]);
	WriteSPI(((BYTE*)&addr)[0]);

	while (n--) {
		*data++ = WriteSPI(0);
	};
	
	SetPortBit(handle & 0x0F,(handle & 0xF0) >> 4,1);
	SelectPlugin(LETTUCE_DEV_NONE);

	CloseSPI();
}

//===========================================================================
// Write array to SPIMEM
//
void SPIMEMPutArray(SPIMEM_HANDLE handle, WORD addr, BYTE *data, WORD n)
{
	// Ignore operations when the destination is NULL or nothing to read
	if(data == NULL || n == 0 || handle == SPIMEM_BAD_HANDLE) {
		return;
	}

	OpenSPI(mem_spicon);

	SelectPlugin(handle & 0x0F);
	SetPortBit(handle & 0x0F,(handle & 0xF0) >> 4,0);

	WriteSPI(CMD_WRITE);
	WriteSPI(((BYTE*)&addr)[1]);
	WriteSPI(((BYTE*)&addr)[0]);

	while (n--) {
		WriteSPI(*data++);
	};
	
	SetPortBit(handle & 0x0F,(handle & 0xF0) >> 4,1);
	SelectPlugin(LETTUCE_DEV_NONE);

	CloseSPI();
}

//===========================================================================
// Write ROM array to SPIMEM
//
void SPIMEMPutROMArray(SPIMEM_HANDLE handle, WORD addr,ROM BYTE *data, WORD n)
{
	// Ignore operations when the destination is NULL or nothing to read
	if(data == NULL || n == 0 || handle == SPIMEM_BAD_HANDLE) {
		return;
	}

	OpenSPI(mem_spicon);

	SelectPlugin(handle & 0x0F);
	SetPortBit(handle & 0x0F,(handle & 0xF0) >> 4,0);

	WriteSPI(CMD_WRITE);
	WriteSPI(((BYTE*)&addr)[1]);
	WriteSPI(((BYTE*)&addr)[0]);

	while (n--) {
		WriteSPI(*data++);
	};
	
	SetPortBit(handle & 0x0F,(handle & 0xF0) >> 4,1);
	SelectPlugin(LETTUCE_DEV_NONE);

	CloseSPI();
}

#endif
