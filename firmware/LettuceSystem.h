/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce System devices

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __LETTUCE_SYSTEM_H
#define __LETTUCE_SYSTEM_H

#include "lettuce.h"
#include "Buffer.h"

//===========================================================================
// INFO device
//

//-------------------------------------
// Addresses
//
#define INFO_VERSION							0x00u
#define INFO_TCPIPVER							0x01u
#define INFO_BUILD								0x02u

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_info(BYTE cmd, WORD addr, BUFFER* buffer);


//===========================================================================
// CLOCK device

//-------------------------------------
// Addresses
//
#define CLOCK_TIME								0x00u
#define CLOCK_TICKS								0x01u

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_clock(BYTE cmd, WORD addr, BUFFER* buffer);


//===========================================================================
// LOGGER device
//

//-------------------------------------
// Addresses
//
#define LOGGER_MODE								0x00u

//-------------------------------------
// Modes
//
#define LOGGER_MODE_BOOT					0x00u
#define LOGGER_MODE_USART					0x01u
#define LOGGER_MODE_ETH						0x02u

//-------------------------------------
// Standard log codes
//
#define LOG_START									0x0000
#define LOG_READY									0x0001
#define LOG_STOP									0x0002
#define LOG_RESET									0x0003
#define LOG_TICK									0x0004

//-------------------------------------
// Initialise module
//
void ModuleInit_logger(void);

//-------------------------------------
// Perform module tasks
//
void ModuleTask_logger(void);

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_logger(BYTE cmd, WORD addr, BUFFER* buffer);

//-------------------------------------
// Log message
// Log the specified message from the specified device at the specified 
// location
//
void Log(BYTE dev, BYTE devloc, rom char* msg);


//===========================================================================
// ETH device
//

//-------------------------------------
// Addresses
//
#define ETH_IPADDR								0x00u
#define ETH_NETMASK								0x01u
#define ETH_GATEWAY								0x02u
#define ETH_PRIDNS								0x03u
#define ETH_SECDNS								0x04u
#define ETH_DEFIPADDR							0x05u
#define ETH_DEFNETMASK						0x06u
#define ETH_NAME									0x07u
#define ETH_DHCP									0x08u
#define ETH_CONFIGMODE						0x09u
#define ETH_MACADDR								0x0au

//-------------------------------------
// Initialise module
//
void ModuleInit_eth(void);

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_eth(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer);

//-------------------------------------
// Refresh AppConfig by re-reading from registry
//
void RefreshAppConfig(void);


//===========================================================================
// USART device
//

//-------------------------------------
// Addresses
//
#define USART_MODE								0x00u

//-------------------------------------
// Modes
//
#define USART_MODE_DEBUG 					0x00u
#define USART_MODE_BRIDGE 				0x01u
#define USART_MODE_LINBUS 				0x10u

//-------------------------------------
// Initialise module
//
void ModuleInit_usart(void);

//-------------------------------------
// Perform module tasks
//
void ModuleTask_usart(void);

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_usart(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer);

//-------------------------------------
// Handle module interrupts
//
void ModuleISR_usart(void);

#endif
