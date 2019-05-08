/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce I/O port plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "plugin_ioport.h"
#include "LettuceServer.h"
#include "LettuceClient.h"
#include "LettuceRegistry.h"
#include "sralloc.h"

#ifdef LETTUCE_PLUGIN_IOPORT

//-------------------------------------
// MCP23S17 memory map (using BANK 0 mode):
//
#define	ADDR_IODIRA			0x00
#define ADDR_IODIRB			0x01
#define ADDR_IPOLA			0x02
#define ADDR_IPOLB			0x03
#define ADDR_GPINTENA		0x04
#define ADDR_GPINTENB		0x05
#define ADDR_DEFVALA		0x06
#define ADDR_DEFVALB		0x07
#define ADDR_INTCONA		0x08
#define ADDR_INTCONB		0x09
#define ADDR_IOCON			0x0A
//      ADDR_IOCON			0X0B (duplicate)
#define ADDR_GPPUA			0x0C
#define ADDR_GPPUB			0x0D
#define ADDR_INTFA			0x0E
#define ADDR_INTFB			0x0F
#define ADDR_INTCAPA		0x10
#define ADDR_INTCAPB		0x11
#define ADDR_GPIOA			0x12
#define ADDR_GPIOB			0x13
#define ADDR_OLATA			0x14
#define ADDR_OLATB			0x15

//-------------------------------------
// MCP23S17 commands:
//
#define CMD_WRITE(dev)	(0x40 | (dev << 1))
#define CMD_READ(dev)		(0x41 | (dev << 1))

// Maximum number of devices that can be chained from a single lettuce SPI 
// slot. Each device must be assigned a unique 3-bit address.
#define MAX_DEVICES			8

// How often we scan for new devices in the chain
#define BUS_SCAN_INTERVAL		(10*TICK_SECOND)

// Configuration to use for the devices:
// BANK=0, MIRROR=1, SEQOP=0, DISSLW=0, HAEN=1, ODR=1
#define DEVICE_IOCON		0x4C

#define IOPORT_ADDRESS(p,dev,line) ( ((DWORD)LETTUCE_PLUGIN_IOPORT << 24) + ((DWORD)LookupIndex(p) << 16) + ((DWORD)dev << 8) + (DWORD)line )


//-------------------------------------
// Device types:
//
#define IOPORT_NONE			0
#define IOPORT_16BIT		1

//-----------------------------------
// I/O port device data for 16bit devices
//
typedef struct _IOPORT_16BIT_DEVICE_DATA {
	WORD values[16];
} IOPORT_16BIT_DEVICE_DATA;

//-----------------------------------
// I/O port device
//
typedef struct _IOPORT_DEVICE {
	BYTE type;
	void* data;
} IOPORT_DEVICE;

//-------------------------------------
// ioport plugin instance data
//
typedef struct _IOPORT_DATA {
	IOPORT_DEVICE devices[MAX_DEVICES];
	TICK scan_timer;
	struct {
		unsigned char interrupt : 1;
	} flags;
} IOPORT_DATA;


//===========================================================================
// Plugin event handler
//
void PluginEvent_ioport(LETTUCE_PLUGIN* p, BYTE event)
{
	unsigned int i,j;
	BYTE b;
	BYTE intf_a, intf_b;
	BYTE intcap_a, intcap_b;
	DWORD addr;
	IOPORT_16BIT_DEVICE_DATA* d16bit;
	IOPORT_DEVICE* device;
	IOPORT_DATA* d;
	BUFFER buffer;

	d = (IOPORT_DATA*)p->mem;
	InitBuffer(&buffer);

	switch (event) {

		case EVENT_OPEN:
			// Disable interrupts during initialisation
			SetPluginInt(p->dev,FALSE);
			SetupPluginInt(p->dev,INT_FALLING_EDGE);

			// Set associated port to output all zeros as it isn't used
			SetPortByte(p->dev,0x00);
			SetPortTristate(p->dev,0x00);

			// Allocate buffer
			p->mem = SRAMalloc(sizeof(IOPORT_DATA));
			p->timer = 0;
			d = ((IOPORT_DATA*)p->mem);

			// Clear devices array
			for (i=0; i<MAX_DEVICES; ++i) {
				device = &d->devices[i];
				device->type = IOPORT_NONE;
				device->data = 0;
			}
			d->scan_timer = 0;
			d->flags.interrupt = 0;

			// Enable interrupts
			SetPluginInt(p->dev,TRUE);
			ClearPluginInt(p->dev);

			// Fall through to perform initial bus scan...

		case EVENT_TICK:

			if (d->flags.interrupt) {
				// An interrupt occurred, find which input(s) caused it

				// Reset interrupt flag
				d->flags.interrupt = 0;

			  // Read all INTCAP registers
				for (i=0; i<MAX_DEVICES; ++i) {
					device = &d->devices[i];
					if (device->type == IOPORT_16BIT) {
						d16bit = ((IOPORT_16BIT_DEVICE_DATA*)device->data);
						OpenSPI(p->cls->spicon);
						SelectPlugin(p->dev);

						WriteSPI(CMD_READ(i));
						WriteSPI(ADDR_INTFA);
						intf_a = WriteSPI(0); 	// ADDR_INTFA
						intf_b = WriteSPI(0);		// ADDR_INTFB
						intcap_a = WriteSPI(0);	// ADDR_INTCAPA
						intcap_b = WriteSPI(0);	// ADDR_INTCAPB

						SelectPlugin(LETTUCE_DEV_NONE);
						CloseSPI();

						for (j=0; j<8; ++j) {
							if ((1<<j) & intf_a) {
								b = !!(intcap_a & (1<<j));
								if (GetRegEntry(REG_IOTYPE,IOPORT_ADDRESS(p,i,j),&buffer) && *buffer.data == 2) {
									// Time latched input
									if (b) {
										if (GetRegEntry(REG_TIME,IOPORT_ADDRESS(p,i,j),&buffer) && GetBufferWord(&buffer,BUFFER_UINT,&(d16bit->values[j]))) {
											BroadcastState(IOPORT_ADDRESS(p,i,j),b);
										}
									}
								} else {
									BroadcastState(IOPORT_ADDRESS(p,i,j),b);
								}
							}
						}

						for (j=0; j<8; ++j) {
							if ((1<<j) & intf_b) {
								b = !!(intcap_b & (1<<j));
								if (GetRegEntry(REG_IOTYPE,IOPORT_ADDRESS(p,i,8+j),&buffer) && *buffer.data == 2) {
									// Time latched input
									if (b) {
										if (GetRegEntry(REG_TIME,IOPORT_ADDRESS(p,i,8+j),&buffer) && GetBufferWord(&buffer,BUFFER_UINT,&(d16bit->values[8+j]))) {
											BroadcastState(IOPORT_ADDRESS(p,i,8+j),b);
										}
									}
								} else {
									BroadcastState(IOPORT_ADDRESS(p,i,8+j),b);
								}
							}
						}

					} // (device loop)
				}
			}

			// Handle time latched inputs
			for (i=0; i<MAX_DEVICES; ++i) {
				device = &d->devices[i];
				if (device->type == IOPORT_16BIT) {
					d16bit = ((IOPORT_16BIT_DEVICE_DATA*)device->data);

					for (j=0; j<16; ++j) {
						if (d16bit->values[j]) {
							--(d16bit->values[j]);
							if (d16bit->values[j] == 0) {
								BroadcastState(IOPORT_ADDRESS(p,i,j),0);
							}
						}
					}

				}
			}

			// Return if scan timeout not reached	
		  if (TickGet() < d->scan_timer) {
				DeleteBuffer(&buffer);
				return;
			}
			d->scan_timer = TickGet() + BUS_SCAN_INTERVAL;

		  // Set up SPI module
			OpenSPI(p->cls->spicon);

			// Broadcast IOCON for all devices on the bus. After this we should be
			// able to select devices individually using hardware addresses.
			for (i=0; i<MAX_DEVICES; ++i) {
				SelectPlugin(p->dev);
				WriteSPI(CMD_WRITE(i));
				WriteSPI(ADDR_IOCON);
				WriteSPI(DEVICE_IOCON);
				SelectPlugin(LETTUCE_DEV_NONE);
			}
			CloseSPI();

			// Probe each device address
			for (i=0; i<MAX_DEVICES; ++i) {
				device = &d->devices[i];
		
				// Attempt to read back the device's IOCON register
				OpenSPI(p->cls->spicon);
				SelectPlugin(p->dev);
				WriteSPI(CMD_READ(i));
				WriteSPI(ADDR_IOCON);
				b = WriteSPI(0);
				SelectPlugin(LETTUCE_DEV_NONE);
				CloseSPI();
			
				if (b == DEVICE_IOCON) {
					if (device->type != IOPORT_16BIT) {

						// Found a new device on the bus
						device->type = IOPORT_16BIT;

						if (device->data) {
							SRAMfree(device->data);
						}
						device->data = SRAMalloc(sizeof(IOPORT_16BIT_DEVICE_DATA));
						d16bit = ((IOPORT_16BIT_DEVICE_DATA*)device->data);

						for (j=0; j<16; ++j) {
							d16bit->values[j] = 0;
						}

						// Set tristates using settings from registry

						// Port A
						b = 0xFF; // Default to all input
						for (j=0; j<8; ++j) {
							if (GetRegEntry(REG_IOTYPE,IOPORT_ADDRESS(p,i,j),&buffer) && *buffer.data == 0) {
								b &= ~(1 << j);
							}
						}

						OpenSPI(p->cls->spicon);
						SelectPlugin(p->dev);
						WriteSPI(CMD_WRITE(i));
						WriteSPI(ADDR_IODIRA);
						WriteSPI(b);
						SelectPlugin(LETTUCE_DEV_NONE);
						CloseSPI();

						// Port B
						b = 0xFF; // Default to all input
						for (j=0; j<8; ++j) {
							if (GetRegEntry(REG_IOTYPE,IOPORT_ADDRESS(p,i,8+j),&buffer) && *buffer.data == 0) {
								b &= ~(1 << j);
							}
						}

						OpenSPI(p->cls->spicon);
						SelectPlugin(p->dev); 
						WriteSPI(CMD_WRITE(i));
						WriteSPI(ADDR_IODIRB);
						WriteSPI(b);
						SelectPlugin(LETTUCE_DEV_NONE);
						CloseSPI();

						// Enable interrupts

						OpenSPI(p->cls->spicon);

						SelectPlugin(p->dev);
						WriteSPI(CMD_WRITE(i));
						WriteSPI(ADDR_GPINTENA);
						WriteSPI(0xFF);
						SelectPlugin(LETTUCE_DEV_NONE);

						SelectPlugin(p->dev);
						WriteSPI(CMD_WRITE(i));
						WriteSPI(ADDR_GPINTENB);
						WriteSPI(0xFF);
						SelectPlugin(LETTUCE_DEV_NONE);
			
						CloseSPI();
					}
				} else {
					device->type = IOPORT_NONE;
					if (device->data) {
						SRAMfree(device->data);
						device->data = 0;
					}
				}
			}
			break;

		case EVENT_STATE:
			// Re-evaluate output states

			for (i=0; i<MAX_DEVICES; ++i) {
				device = &d->devices[i];
				if (device->type == IOPORT_16BIT) {
					d16bit = ((IOPORT_16BIT_DEVICE_DATA*)device->data);

					// Read value from port latch
					OpenSPI(p->cls->spicon);
					SelectPlugin(p->dev); // Send to device
					WriteSPI(CMD_READ(i));
					WriteSPI(ADDR_OLATA);
					b = WriteSPI(0);
					SelectPlugin(LETTUCE_DEV_NONE);
					CloseSPI();
	
					for (j=0; j<8; ++j) {
						if (GetRegEntry(REG_CONDITION,IOPORT_ADDRESS(p,i,j),&buffer)) {
							if (EvaluateExpr(buffer.data)) {
								b |= (1<<j);
							} else {
								b &= ~(1<<j);
							}
						}
					}
	
					// Write value to port latch
					OpenSPI(p->cls->spicon);
					SelectPlugin(p->dev); // Send to device
					WriteSPI(CMD_WRITE(i));
					WriteSPI(ADDR_OLATA);
					WriteSPI(b);
					SelectPlugin(LETTUCE_DEV_NONE);
					CloseSPI();
	
					// Read value from port latch
					OpenSPI(p->cls->spicon);
					SelectPlugin(p->dev); // Send to device
					WriteSPI(CMD_READ(i));
					WriteSPI(ADDR_OLATB);
					b = WriteSPI(0);
					SelectPlugin(LETTUCE_DEV_NONE);
					CloseSPI();
	
					for (j=0; j<8; ++j) {
						if (GetRegEntry(REG_CONDITION,IOPORT_ADDRESS(p,i,8+j),&buffer)) {
							if (EvaluateExpr(buffer.data)) {
								b |= (1<<j);
							} else {
								b &= ~(1<<j);
							}
						}
					}
	
					// Write value to port latch
					OpenSPI(p->cls->spicon);
					SelectPlugin(p->dev); // Send to device
					WriteSPI(CMD_WRITE(i));
					WriteSPI(ADDR_OLATB);
					WriteSPI(b);
					SelectPlugin(LETTUCE_DEV_NONE);
					CloseSPI();
				}
			}	
			break;

		case EVENT_CLOSE:
			// Clean up our buffer
			SRAMfree(p->mem);
			p->mem = 0;
			break;
	}

	DeleteBuffer(&buffer);
}

//===========================================================================
// Plugin request handler
//
BYTE PluginRequest_ioport(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer)
{
	BYTE b,device,port,channel;
	IOPORT_16BIT_DEVICE_DATA* d16bit;
	IOPORT_DATA* d;

	d = (IOPORT_DATA*)p->mem;
	if (d == 0) {
		return LETTUCE_STATUS_BAD_DEVICE;
	}

	device = ((addr & 0xFF00) >> 8);
	if (device >= MAX_DEVICES || d->devices[device].type == IOPORT_NONE) {
		return LETTUCE_STATUS_BAD_ARG;
	}

	port = (addr & 0x00FF) / 8;
	if (port >= 2) {
		return LETTUCE_STATUS_BAD_ARG;
	}

	channel = (addr & 0x00FF) % 8;
	if (channel >= 8) {
		return LETTUCE_STATUS_BAD_ARG;
	}

	switch (cmd) {

		case LETTUCE_CMD_GET:
			if (d->devices[device].type == IOPORT_16BIT) {
				d16bit = ((IOPORT_16BIT_DEVICE_DATA*)d->devices[device].data);
				if (d16bit->values[(port*8) + channel]) {
					SetBufferBool(buffer,1);
					return LETTUCE_STATUS_OK;
				}
			}

		  // Set up SPI module
			OpenSPI(p->cls->spicon);

			// Read value from port
			SelectPlugin(p->dev); // Send to device
			WriteSPI(CMD_READ(device));
			WriteSPI(port ? ADDR_GPIOB : ADDR_GPIOA);
			b = WriteSPI(0);
			SelectPlugin(LETTUCE_DEV_NONE);

			SetBufferBool(buffer,!!(b & (1<<channel)));

			CloseSPI();
			return LETTUCE_STATUS_OK;

		case LETTUCE_CMD_SET:
			if (buffer->type != BUFFER_BOOL || buffer->size != 1) {
				return LETTUCE_STATUS_BAD_ARG;
			}

		  // Set up SPI module
			OpenSPI(p->cls->spicon);

			// Read value from port latch
			SelectPlugin(p->dev); // Send to device
			WriteSPI(CMD_READ(device));
			WriteSPI(port ? ADDR_OLATB : ADDR_OLATA);
			b = WriteSPI(0);
			SelectPlugin(LETTUCE_DEV_NONE);

			// Update port latch with required bit
			if (*buffer->data) {
				b |= (1<<channel);
			} else {
				b &= ~(1<<channel);
			}

			// Write value to port latch
			SelectPlugin(p->dev); // Send to device
			WriteSPI(CMD_WRITE(device));
			WriteSPI(port ? ADDR_OLATB : ADDR_OLATA);
			WriteSPI(b);
			SelectPlugin(LETTUCE_DEV_NONE);

			CloseSPI();
			return LETTUCE_STATUS_OK;

		case LETTUCE_CMD_DIR: // Set IO direction on port
			if (buffer->type != BUFFER_BOOL || buffer->size != 1) {
				return LETTUCE_STATUS_BAD_ARG;
			}

		  // Set up SPI module
			OpenSPI(p->cls->spicon);

			// Read value from IO direction register
			SelectPlugin(p->dev); // Send to device
			WriteSPI(CMD_READ(device));
			WriteSPI(port ? ADDR_IODIRB : ADDR_IODIRA);
			b = WriteSPI(0);
			SelectPlugin(LETTUCE_DEV_NONE);

			// Update IO direction register with required bit
			if (*buffer->data) {
				b |= (1<<channel);
			} else {
				b &= ~(1<<channel);
			}

			// Write value to IO direction register
			SelectPlugin(p->dev); // Send to device
			WriteSPI(CMD_WRITE(device));
			WriteSPI(port ? ADDR_IODIRB : ADDR_IODIRA);
			WriteSPI(b);
			SelectPlugin(LETTUCE_DEV_NONE);

			CloseSPI();
			return LETTUCE_STATUS_OK;
	}

	return LETTUCE_STATUS_BAD_ADDR;
}

//===========================================================================
// ISR
//
void PluginISR_ioport(LETTUCE_PLUGIN* p, LETTUCE_INT_TYPE type)
{
	static IOPORT_DATA* d;
	d = (IOPORT_DATA*)p->mem;

	if (type == INT_TYPE_PERIPHERAL) {
		// Record the interrupt so it can be handled later in EVENT_TICK.
		d->flags.interrupt = 1;
	}
}

#endif
