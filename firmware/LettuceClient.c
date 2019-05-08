/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce client

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "LettuceClient.h"
#include "LettuceServer.h"
#include "LettuceRegistry.h"
#include "LettuceSystem.h"
#include "LettuceCore.h"
#include "Buffer.h"

//-------------------------------------
// Client states
//
#define	SM_RESET 					0
#define SM_READY 					1
//

static BYTE state = SM_RESET;
static UDP_SOCKET socket;
static BUFFER name_buffer;
static BUFFER data_buffer;

static BYTE state_update = 0;

//===========================================================================
// Broadcast event
//
void BroadcastEvent(rom char* msg)
{
	LettuceClient();
	if (name_buffer.type == BUFFER_EMPTY) {
		SetBufferDWord(&name_buffer,BUFFER_UINT,0);
		SetBufferRomString(&data_buffer,msg);
	}
	LettuceClient();
}

void BroadcastEventRam(char* msg)
{
	LettuceClient();
	if (name_buffer.type == BUFFER_EMPTY) {
		SetBufferDWord(&name_buffer,BUFFER_UINT,0);
		SetBufferString(&data_buffer,msg);
	}
	LettuceClient();
}	

//===========================================================================
// Broadcast status
//
void BroadcastState(DWORD addr, BYTE value)
{
	WORD id;
	state_update = 1;
	LettuceClient();
	if (name_buffer.type == BUFFER_EMPTY) {

		if (!GetRegEntry(REG_NAME,addr,&name_buffer)) {
			// Not found name, use UINT32 address
			SetBufferDWord(&name_buffer,BUFFER_UINT,addr);
		}
		SetBufferByte(&data_buffer,BUFFER_UINT,value);
	}
	LettuceClient();
}

//===========================================================================
// Lettuce client
//
void LettuceClient(void)
{
	WORD w;

	switch (state) { 
		case SM_RESET:
			InitBuffer(&name_buffer);
			InitBuffer(&data_buffer);
			state = SM_READY;
			break;

		case SM_READY:
			if (name_buffer.type == BUFFER_EMPTY) {
				break;
			}

			// Open a UDP socket to send the broadcast
			socket = UDPOpen(LETTUCE_CLIENT_PORT, NULL, LETTUCE_UDP_SERVER_PORT);
			if(socket == INVALID_UDP_SOCKET) {
				break;
			}

			// Make certain the socket can be written to
			if(UDPIsPutReady(socket) == 0) {
				UDPClose(socket);
				break;
			}

			// Send the data
			UDPPut(LETTUCE_PREFIX);
			UDPPut(LETTUCE_CMD_STATE);
			UDPPutBuffer(&name_buffer);
			UDPPutBuffer(&data_buffer);
			UDPFlush();
			UDPClose(socket);

			DeleteBuffer(&name_buffer);
			DeleteBuffer(&data_buffer);
			break;
	}
}

//===========================================================================
// Lettuce state client
//
void LettuceStateClient(void)
{
	if (state_update) {
		PluginControllerStateEvent();

		state_update = 0;
	}
}
