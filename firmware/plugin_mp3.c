/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce MP3 plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "plugin_mp3.h"
#include "plugin_mem.h"
#include "LettuceServer.h"
#include "sralloc.h"

#ifdef LETTUCE_PLUGIN_MP3

// MP3 server
rom char* mp3_host = "regulus.stouffer.co.uk";

// Chunk (page) size
#define CHUNK 									32

//-------------------------------------
// VS1011 memory map:
//
#define ADDR_MODE					0x00
#define ADDR_STATUS				0x01
#define ADDR_BASS					0x02
#define ADDR_CLOCKF				0x03
#define ADDR_DECODE_TIME	0x04
#define ADDR_AUDATA				0x05
#define ADDR_WRAM					0x06
#define ADDR_WRAMADDR			0x07
#define ADDR_HDAT0				0x08
#define ADDR_HDAT1				0x09
#define ADDR_AIADDR				0x0A
#define ADDR_VOL					0x0B
#define ADDR_AICTRL0			0x0C
#define ADDR_AICTRL1			0x0D
#define ADDR_AICTRL2			0x0E
#define ADDR_AICTRL3			0x0F

//-------------------------------------
// VS1011 commands:
//
#define CMD_WRITE					0x02
#define CMD_READ					0x03

//-------------------------------------
// Port bits:
//
#define BIT_CCS						0
#define BIT_DCS 					1
#define BIT_RESET 				2

//-------------------------------------
// mp3 plugin instance data
//
typedef struct _MP3_DATA {
	BYTE	state;
	struct {
		unsigned char play : 1;
		unsigned char underrun : 1;
		unsigned char buffer_in_spimem : 1;
		unsigned char loopback : 1;
	} flags;

	TCP_SOCKET socket;
	TICK reconnect_timer;

	WORD	size;
	WORD 	head;
	WORD	tail;
	SPIMEM_HANDLE handle;
	BYTE* buffer;
} MP3_DATA;

//-------------------------------------
// Default volume/bass setting
//
static BYTE g_rightvol =0;
static BYTE g_leftvol =0;
static BYTE g_bass =0;
static BYTE g_gfreq =15;

//-------------------------------------
// MP3Client states
//
#define SM_RESET								0
#define SM_DISCONNECTION_WAIT		1
#define SM_CONNECT							2
#define SM_CONNECT_WAIT					3
#define SM_PLAYING							4

// Time to wait before reconnecting if server disconnects
#define RECONNECT_INTERVAL			(2*TICK_SECOND)

//-------------------------------------
// Local functions
//
void MP3Client(LETTUCE_PLUGIN* p);


//===========================================================================
// Plugin event handler
//
void PluginEvent_mp3(LETTUCE_PLUGIN* p, BYTE event)
{
	BYTE b1,b2;
	MP3_DATA* d;

	if (event != EVENT_OPEN && p->mem == 0) {
		return;
	}
	d = (MP3_DATA*)p->mem;

	switch (event) {

		case EVENT_OPEN:
			// Initialize the VLSI VS1011 MP3 decoder
		
			// Disable interrupts during initialisation
			SetPluginInt(p->dev,FALSE);
			SetupPluginInt(p->dev,INT_RISING_EDGE);
			p->timer = 0;

		  // Set up SPI module and enable bus to device
			OpenSPI(p->cls->spicon);
			SelectPlugin(p->dev);
		
			SetPortBit(p->dev,BIT_CCS,1); // Unselect control
			SetPortBit(p->dev,BIT_DCS,1); // Unselect data
			SetPortBit(p->dev,BIT_RESET,0); // Hold device in reset
		
			// Set associated port to outputs
			SetPortTristate(p->dev,0x00);
		
			// Bring the device out of reset
			Delay1KTCYx(10);
			SetPortBit(p->dev,BIT_RESET,1);
		
			// Configure device
			do {

				while (!PollPluginInt(p->dev));
		
				SetPortBit(p->dev,BIT_CCS,0);
				WriteSPI(CMD_WRITE);
				WriteSPI(ADDR_MODE);
				WriteSPI(0x08);			// 16 bit value to write
				WriteSPI(0x20);			// Normal transfer mode (using DREQ), new serial mode, SDI tests enabled
				//WriteSPI(0x60);		// Same, but with stream transfer mode instead of normal mode
				SetPortBit(p->dev,BIT_CCS,1);
		
				while (!PollPluginInt(p->dev));
		
				SetPortBit(p->dev,BIT_CCS,0);
				WriteSPI(CMD_WRITE);
				WriteSPI(ADDR_CLOCKF);
				WriteSPI(0x98);			// Using a 12.288MHz crystal
				WriteSPI(0x00);
				SetPortBit(p->dev,BIT_CCS,1);
		
				while (!PollPluginInt(p->dev));
		
				// Read back first mode byte to make sure the VS1011 is present
				SetPortBit(p->dev,BIT_CCS,0);
				WriteSPI(CMD_READ);
				WriteSPI(ADDR_MODE);
				b1 = WriteSPI(0xFF);
				b2 = WriteSPI(0xFF);
				SetPortBit(p->dev,BIT_CCS,1);
		
			} while (b1 != 0x08);
		
			SelectPlugin(LETTUCE_DEV_NONE);
			CloseSPI();
			
			// Set initial volume and bass levels
			SetVolume(p, g_rightvol, g_leftvol);
			SetBassBoost(p, g_bass, g_gfreq);

			// Initialise instance data for this plugin
			p->mem = SRAMalloc(sizeof(MP3_DATA));
			if (p->mem == 0) {
				// No memory available
				return;
			}
			d = ((MP3_DATA*)p->mem);

			d->state = SM_RESET;
			d->flags.play = 0;
			d->flags.loopback = 0;
			d->socket = INVALID_SOCKET;
			d->reconnect_timer = 0;

			// Try and allocate some SPIMEM for audio the buffer
			d->handle = SPIMEMalloc();
			if (d->handle != SPIMEM_BAD_HANDLE) {
				d->flags.buffer_in_spimem = 1;
				d->size = 32768;
				d->buffer = 0;
			} else {
				// SPIMEM unavailable, use 126 bytes of PIC RAM
				d->flags.buffer_in_spimem = 0;
				d->size = 126;
				d->buffer = SRAMalloc(d->size);
			}
			d->head = 0;
			d->tail = 0;

			// Set underrun flag so buffer gets filled before we start playing
			d->flags.underrun = 1;

			// Enable interrupts for this plugin
			p->timer = 1;
			SetPluginInt(p->dev,TRUE);
			ClearPluginInt(p->dev);
			break;

		case EVENT_CLOSE:
			// Disable interrupts for this plugin
			ENTER_CRITICAL;
			SetPluginInt(p->dev,FALSE);
			p->timer = 0;
			LEAVE_CRITICAL;

			// Clean up buffer
			if (d->flags.buffer_in_spimem) {
				SPIMEMfree(d->handle);
			} else {
				SRAMfree(d->buffer);
			}

			// Clean up instance data
			SRAMfree(p->mem);
			p->mem = 0;
			break;

		case EVENT_TICK:
			// Run the mp3 client
			MP3Client(p);
			break;
	}	
}

//===========================================================================
// Plugin request handler
//
BYTE PluginRequest_mp3(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer)
{
	BYTE a,n,i;
	MP3_DATA* d;
	WORD w;

	d = ((MP3_DATA*)p->mem);
	if (d == 0) {
		return LETTUCE_STATUS_BAD_DEVICE;
	}

	switch (addr) {

		case 0x0000:
			if (cmd == LETTUCE_CMD_SET) {
				if (!GetBufferBool(buffer,&i)) {
					return LETTUCE_STATUS_BAD_ARG;
				}
				d->flags.play = i;
			}
			SetBufferBool(buffer, d->flags.play);
			return LETTUCE_STATUS_OK;

		case 0x0001:
			if (cmd == LETTUCE_CMD_SET) {
				if (buffer->type != BUFFER_UINT || buffer->size != 2) {
					return LETTUCE_STATUS_BAD_ARG;
				}
				SetVolume(p,*buffer->data,*(buffer->data+1));
			}
			w = g_leftvol; w = w << 8; w |= g_rightvol;
			SetBufferWord(buffer, BUFFER_UINT, w);
			return LETTUCE_STATUS_OK;

		case 0x0002:
			if (cmd == LETTUCE_CMD_SET) {
				if (buffer->type != BUFFER_UINT || buffer->size != 2) {
					return LETTUCE_STATUS_BAD_ARG;
				}
				SetBassBoost(p,*buffer->data,*(buffer->data+1));
			}
			w = g_gfreq; w = w << 8; w |= g_bass;
			SetBufferWord(buffer, BUFFER_UINT, w);
			return LETTUCE_STATUS_OK;

#ifdef __DEBUG // Allow internal data to be read out in debug mode

		case 0xFE00:
			if (cmd == LETTUCE_CMD_SET) {
				if (!GetBufferBool(buffer,&i)) {
					return LETTUCE_STATUS_BAD_ARG;
				}
				d->flags.loopback = i;
			}
			SetBufferBool(buffer, d->flags.loopback);
			return LETTUCE_STATUS_OK;

		case 0xFE01: 
			SetBufferBool(buffer, d->flags.underrun);
			return LETTUCE_STATUS_OK;

		case 0xFE02: 
			SetBufferBool(buffer, d->flags.buffer_in_spimem);
			return LETTUCE_STATUS_OK;

		case 0xFE03: 
			SetBufferByte(buffer, BUFFER_UINT, d->state);
			return LETTUCE_STATUS_OK;

		case 0xFE04: 
			SetBufferByte(buffer, BUFFER_UINT, d->socket);
			return LETTUCE_STATUS_OK;

		case 0xFE05: 
			SetBufferWord(buffer, BUFFER_UINT, d->size);
			return LETTUCE_STATUS_OK;

		case 0xFE06: 
			SetBufferWord(buffer, BUFFER_UINT, d->head);
			return LETTUCE_STATUS_OK;

		case 0xFE07: 
			SetBufferWord(buffer, BUFFER_UINT, d->tail);
			return LETTUCE_STATUS_OK;

		case 0xFE08: 
			SetBufferByte(buffer, BUFFER_UINT, d->handle);
			return LETTUCE_STATUS_OK;
#endif
	}

	return LETTUCE_STATUS_BAD_ADDR;
}

//===========================================================================
// ISR
//
void PluginISR_mp3(LETTUCE_PLUGIN* p, LETTUCE_INT_TYPE type)
{
	static WORD n;
	static BYTE i;
	static MP3_DATA* d;
	static BYTE buffer[CHUNK];

	d = (MP3_DATA*)p->mem;

#ifdef __DEBUG
	// Loopback mode, don't send to VS1011
	if (d->flags.loopback) {
		return;
	}
#endif

	// See if VS1011 is ready for data
	if (!PollPluginInt(p->dev)) {
		return;
	}

	// Make sure we have at least one chunk to copy
	if (d->head < d->tail) {
		n = d->size - (d->tail - d->head);
	} else {
		n = d->head - d->tail;
	}

	if (n < CHUNK) {
		// Entering underrun state
		d->flags.underrun = TRUE;
		p->timer = 10;
		return;
	}
	
	// After an underrun condition, do not start playing again 
	// until the buffer is at least half full
	if (d->flags.underrun) {
		if (n < 16384) {
			p->timer = 2;
			return;
		}
		// Leaving underrun state
		d->flags.underrun = FALSE;
	}

	// See if the SPI bus is free
	if (!OpenSPI(p->cls->spicon)) {
		// Try again a bit later
		p->timer = 1;
		return;
	}

	// SPI bus is free

	if (d->flags.buffer_in_spimem) {
		// Close SPI for now as SPIMEMGetArray needs it
		CloseSPI();
	
		// Read chunk from SPIMEM
		SPIMEMGetArrayISR(d->handle, d->tail, buffer, CHUNK);
	
		// Re-open SPI to write to the VS1011
		OpenSPI(p->cls->spicon);
	}

	// Select the device and set the data CS pin low to indicate that we are sending data
	SelectPlugin(p->dev);
	SetPortBit(p->dev,BIT_DCS,0);

	// Write the chunk
	if (d->flags.buffer_in_spimem) {
		for (i=0; i<CHUNK; ++i) {
			WriteSPI(*(buffer + i));
		}
	} else {
		for (i=0; i<CHUNK; ++i) {
			WriteSPI(*(d->buffer + d->tail + i));
		}
	}

	// Set the data CS pin high - finished sending data
	SetPortBit(p->dev,BIT_DCS,1);
	SelectPlugin(LETTUCE_DEV_NONE);

	// Finished with SPI
	CloseSPI();

	// Update FIFO tail pointer
	d->tail += CHUNK;
	if(d->tail >= d->size) {
		d->tail = 0;
	}

	// See if VS1011 is ready for data
	if (PollPluginInt(p->dev)) {
		p->timer = 1;
	} 
}

//===========================================================================
void MP3Client(LETTUCE_PLUGIN* p)
{
	int a,n,i;
	int head,tail,free;
	MP3_DATA* d;
	BYTE buffer[CHUNK];
//	BUFFER host;

	d = (MP3_DATA*)p->mem;

	switch (d->state) {

		case SM_RESET:
			if (d->flags.play) {
				d->state = SM_CONNECT;
			}
			break;

		case SM_DISCONNECTION_WAIT:
			if (TickGet() >= d->reconnect_timer) {
				d->state = SM_CONNECT;
			}
			break;

		case SM_CONNECT:
			// Read the hostname from registry
//			InitBuffer(&host);
			
			// Try to allocate a socket with SPIMEM buffering
			d->socket = TCPOpen((DWORD)&mp3_host[0], TCP_OPEN_ROM_HOST, 9771, TCP_PURPOSE_MP3_CLIENT);

			if(d->socket == INVALID_SOCKET) {
				// No SPIMEM sockets available, allocate a socket with internal buffering
				d->socket = TCPOpen((DWORD)&mp3_host[0], TCP_OPEN_ROM_HOST, 9771, TCP_PURPOSE_DEFAULT);
			}
			
//			DeleteBuffer(&host);

			if(d->socket == INVALID_SOCKET) {
				// No sockets available at all
				return;
			}

			d->state = SM_CONNECT_WAIT;
			break;

		case SM_CONNECT_WAIT:
			if(TCPIsConnected(d->socket) && TCPIsPutReady(d->socket)) {
				d->state = SM_PLAYING;
				
				ENTER_CRITICAL;
				d->head = head;
				d->tail = tail;
				LEAVE_CRITICAL;
			}
			break;
	
		case SM_PLAYING:

			if (!d->flags.play) {
				break;
			}

			if(!TCPIsConnected(d->socket))
			{
				TCPDisconnect(d->socket);
				TCPDisconnect(d->socket);
				d->socket = INVALID_SOCKET;
				d->state = SM_DISCONNECTION_WAIT;
				d->reconnect_timer = TickGet() + RECONNECT_INTERVAL;
				break;
			}

			ENTER_CRITICAL;
			head = d->head;
			tail = d->tail;
			LEAVE_CRITICAL;

#ifdef __DEBUG
			if (d->flags.loopback) {
				// LOOPBACK TEST MODE -------------------------------------------------------

				// In this mode, instead of sending data through to the VS1011, we send it
				// back to the server host. The server can capture this and compare with the
				// original to test if the ringbuffer and SPIMEM is working.

				// Code should be the same as in the ISR
				if (head < tail) {
					n = d->size - (tail - head);
				} else {
					n = head - tail;
				}

				if (n >= CHUNK && TCPIsPutReady(d->socket) >= CHUNK) {
					SPIMEMGetArray(d->handle, tail, buffer, CHUNK);
					tail += CHUNK;
					if(tail >= d->size) {
						tail = 0;
					}
		
					// Write the chunk	
					TCPPutArray(d->socket, buffer, CHUNK);

					ENTER_CRITICAL;
					d->tail = tail;
					LEAVE_CRITICAL;
				}		
				// END LOOPBACK TEST MODE ---------------------------------------------------
			}
#endif

			// See how many bytes are available to read from the socket
			n = TCPIsGetReady(d->socket);
			if (n == 0) {
				break;
			}

			// Calc free space in buffer
			if (head < tail) {
				free = tail - head - 1;
			} else {
				free = tail + d->size - head - 1;
			}

			// Don't fetch more bytes that we can store
			if (n > free) {
				n = free;
			}
			// Don't fetch more bytes than can fit in the FIFO 
			// without causing a wrapparound
			if (n >= d->size - head) {
				// Fetch the bytes
				n = d->size - head;
			}

			// Copy data chunks into the audio buffer
			while (n > 0) {
				if (d->flags.buffer_in_spimem) {
					i = CHUNK;
					if (n < CHUNK) i = n;

					TCPGetArray(d->socket, buffer, i);
					SPIMEMPutArray(d->handle, head, buffer, i);

				} else {
					i = n;
					TCPGetArray(d->socket, (d->buffer + head), i);
				}

				head += i;
				n -= i;
				if (head >= d->size) {
					head = 0;
				}

				ENTER_CRITICAL;
				d->head = head;
				LEAVE_CRITICAL;
			}

			break;
	}

}

//===========================================================================
// Set volume for analog outputs on VS1011
//
// vRight = right channel attenuation from maximum volume, 0.5dB steps (0x00 = full volume, 0xFF = muted)
// vLeft = left channel attenuation from maximum volume, 0.5dB steps (0x00 = full volume, 0xFF = muted)
//
void SetVolume(LETTUCE_PLUGIN* p, BYTE rightvol, BYTE leftvol)
{
	// Save values
	g_rightvol = rightvol;
	g_leftvol = leftvol;

  // Set up SPI module
	OpenSPI(p->cls->spicon);

	// Send command to VS1011
	SelectPlugin(p->dev);
	SetPortBit(p->dev,BIT_DCS,1);
	
	while (!PollPluginInt(p->dev));

	SetPortBit(p->dev,BIT_CCS,0);
	WriteSPI(CMD_WRITE);
	WriteSPI(ADDR_VOL);
	WriteSPI(g_leftvol);
	WriteSPI(g_rightvol);	
	SetPortBit(p->dev,BIT_CCS,1);
	SelectPlugin(LETTUCE_DEV_NONE);

	CloseSPI();
}

//===========================================================================
// Set Bass Boost level
//
// bass = Bass gain in dB, range from 0 to 15
// gfreq = Limit frequency for bass boost, 10 Hz steps (range from 0 to 15)
//
void SetBassBoost(LETTUCE_PLUGIN* p, BYTE bass, BYTE gfreq)
{
	BYTE templ = 0;
	
	// Make sure values are in the allowed range
	if(bass > 15)
		bass = 15;
	if(gfreq > 15)
		gfreq = 15;

	// Save values
	g_bass = bass;
	g_gfreq = gfreq;
		
	// put gfreq into the lower 4 bit
	templ = g_gfreq;

	// put bass boost value into the upper 4 bit
	templ |= (g_bass << 4);

  // Set up SPI module
	OpenSPI(p->cls->spicon);

	// Send command to VS1011
	SelectPlugin(p->dev);
	SetPortBit(p->dev,BIT_DCS,1);

	while (!PollPluginInt(p->dev));
	
	SetPortBit(p->dev,BIT_CCS,0);
	WriteSPI(CMD_WRITE);
	WriteSPI(ADDR_BASS);
	WriteSPI(0xFF);
	WriteSPI(templ);
	SetPortBit(p->dev,BIT_CCS,1);
	SelectPlugin(LETTUCE_DEV_NONE);

	CloseSPI();
}

#endif
