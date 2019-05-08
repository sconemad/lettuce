/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce server

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "LettuceServer.h"
#include "LettuceRegistry.h"
#include "LettuceSystem.h"
#include "LettuceCore.h"
#include "Buffer.h"

//-------------------------------------
// Packet format:
//
// BYTE 'L'
// BYTE cmd			(0=get, 1=set)

// BYTE namelen 			(MAX 31)
// BYTE name[namelen]

// WORD datalen				(MAX 128)
// BYTE data[datalen]

//-------------------------------------
// Server states
//
#define	SM_RESET 					0
#define SM_READY 					1
//

//===========================================================================
// Handle server request
//
BYTE HandleRequest(BYTE cmd, DWORD addr, BUFFER* buffer)
{
	BYTE devtype;
	BYTE devloc;
	WORD waddr;
	
	devtype = (addr & 0xFF000000) >> 24;
	devloc = (addr & 0x00FF0000) >> 16;
	waddr = (addr & 0x0000FFFF);

	switch (devtype) {

		case LETTUCE_DEV_NONE:
			return LETTUCE_STATUS_OK;

		case LETTUCE_DEV_INFO:
			return ModuleRequest_info(cmd,waddr,buffer);

		case LETTUCE_DEV_CLOCK:
			return ModuleRequest_clock(cmd,waddr,buffer);

		case LETTUCE_DEV_REGISTRY:
			return ModuleRequest_registry(cmd,waddr,buffer);

		case LETTUCE_DEV_LOGGER:
			return ModuleRequest_logger(cmd,waddr,buffer);

		case LETTUCE_DEV_ETH:
			return ModuleRequest_eth(devloc,cmd,waddr,buffer);

		case LETTUCE_DEV_USART:
			return ModuleRequest_usart(devloc,cmd,waddr,buffer);

		case LETTUCE_DEV_EEPROM:
			return ModuleRequest_eeprom(devloc,cmd,waddr,buffer);

		case LETTUCE_DEV_AUX:
			return ModuleRequest_aux(devloc,cmd,waddr,buffer);

		case LETTUCE_DEV_PWM:
			return ModuleRequest_pwm(devloc,cmd,waddr,buffer);

		case LETTUCE_DEV_COUNTER:
			return ModuleRequest_counter(devloc,cmd,waddr,buffer);

		case LETTUCE_DEV_BUTTONS:
			return ModuleRequest_buttons(devloc,cmd,waddr,buffer);

		case LETTUCE_DEV_LINBUS:
			return ModuleRequest_linbus(devloc,cmd,waddr,buffer);
	}

	// Send request to plugin
	return PluginRequest(cmd,devtype,devloc,waddr,buffer);;
}


//===========================================================================
// Lettuce TCP server
//
void LettuceTCPServer(void)
{
	static BYTE state = SM_RESET;
	static TCP_SOCKET socket;
	static BUFFER buffer;

	BYTE cmd;
	BYTE devtype;
	BYTE devloc;
	WORD w;
	BYTE ret;
	DWORD data;

	if (state == SM_RESET) {
		// Allocate a socket for this server to listen and accept connections on
		socket = TCPOpen(0, TCP_OPEN_SERVER, LETTUCE_TCP_SERVER_PORT, TCP_PURPOSE_LETTUCE_SERVER);
		if(socket == INVALID_SOCKET) {
			return;
		}
		InitBuffer(&buffer);
		state = SM_READY;
		return;
	}

	if (!TCPIsConnected(socket)) {
		// Can stop here if nobody is connected
		return;
	}

	// Figure out how many bytes have been received and how many we can transmit.
	w = TCPIsGetReady(socket);	// Get TCP RX FIFO byte count

	if (state == SM_READY) {
		if (w < 3) {
			return;
		}

		ret = LETTUCE_STATUS_OK;
		TCPGet(socket,&cmd); // Read command prefix
		if (cmd != LETTUCE_PREFIX) {
			// Incorrect command prefix, close the socket
			TCPDisconnect(socket);
			return;
		}

		// Start processing new command
		TCPGet(socket,&cmd); // Read command
		TCPGetBuffer(socket,&buffer); // Read name

		// We're using this temporarily to hold the full address
		data = 0;
		if (buffer.type == BUFFER_STRING) {
			// Resolve device name in registry
			if (!LookupRegEntryByName(buffer.data,&data)) {
				ret = LETTUCE_STATUS_BAD_DEVICE;
			}

		} else if (buffer.type == BUFFER_UINT) {
			// Binary device address
			data = *((DWORD*)buffer.data);

		} else {
			ret = LETTUCE_STATUS_BAD_DEVICE;
		}

		// Read data buffer
		TCPGetBuffer(socket,&buffer);

		// If all is well, process the request
		if (ret == LETTUCE_STATUS_OK) {
			ret = HandleRequest(cmd,data,&buffer);
		}
	}

	// Send the response
	w = TCPIsPutReady(socket);	// Get TCP TX FIFO free space
	TCPPut(socket,'R');
	TCPPut(socket,ret);
	TCPPutBuffer(socket,&buffer);
	DeleteBuffer(&buffer);
	TCPFlush(socket);
}


//===========================================================================
// Lettuce UDP server
//
void LettuceUDPServer(void)
{
	static BYTE state = SM_RESET;
	static UDP_SOCKET socket;
	static BUFFER buffer;

	BYTE cmd;
	BYTE devtype;
	BYTE devloc;
	BYTE ret;
	DWORD data;
	WORD w;

	switch (state) { 
		case SM_RESET:
			// Allocate a socket for this server
			socket = UDPOpen(LETTUCE_UDP_SERVER_PORT, NULL, 0);
			if(socket == INVALID_UDP_SOCKET) {
				break;
			}
			InitBuffer(&buffer);
			state = SM_READY;
			break;

		case SM_READY:
			// See if a packet has been recieved
			w = UDPIsGetReady(socket);	// Get TCP RX FIFO byte count
			
			if (w == 0) {
				break;	
			}	

			// Start processing new command
			ret = LETTUCE_STATUS_OK;
			
			// Read and check command prefix
			if (!UDPGet(&cmd) || cmd != LETTUCE_PREFIX) {
				// Incorrect command prefix, disregard it
				UDPDiscard();
				break;
			}
			
			// Read command
			if (!UDPGet(&cmd)){
				UDPDiscard();
				break;
			}
			
			// Read name/address buffer
			if (!UDPGetBuffer(&buffer)) {
				UDPDiscard();
				break;	
			}	

			// We're using this temporarily to hold the full address
			data = 0;
			if (buffer.type == BUFFER_STRING) {
				// Resolve device name in registry
				if (!LookupRegEntryByName(buffer.data,&data)) {
					ret = LETTUCE_STATUS_BAD_DEVICE;
				}
	
			} else if (buffer.type == BUFFER_UINT) {
				// Binary device address
				data = *((DWORD*)buffer.data);
	
			} else {
				ret = LETTUCE_STATUS_BAD_DEVICE;
			}

			// Read data buffer
			if (!UDPGetBuffer(&buffer)) {
				UDPDiscard();
				break;	
			}	

			// Discard any remaining data
			UDPDiscard();

			// If all is well, process the request
			if (ret == LETTUCE_STATUS_OK) {
				ret = HandleRequest(cmd,data,&buffer);
			}

			// Send the response
			w = UDPIsPutReady(socket);
			if (w) {
				UDPPut('R');
				UDPPut(ret);
				UDPPutBuffer(&buffer);
			}	
			UDPFlush();
			DeleteBuffer(&buffer);
			break;
	}
}
