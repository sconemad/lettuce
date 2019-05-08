/* Lettuce (http://www.sconemad.com/lettuce)
 
Plugin Controller

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __PLUGIN_CONTROLLER_H
#define __PLUGIN_CONTROLLER_H

#include "lettuce.h"
#include "Buffer.h"

//-----------------------------------
// Plugin class
//
typedef struct _LETTUCE_PLUGIN_CLASS {
	BYTE						type;
	rom char*				name;
	BYTE						spicon;
	WORD 						sidl;		// SPI plugin identifier lower limit
	WORD 						sidu;		// SPI plugin identifier upper limit
} LETTUCE_PLUGIN_CLASS;

//-----------------------------------
// Plugin instance
//
typedef struct _LETTUCE_PLUGIN {
	rom LETTUCE_PLUGIN_CLASS* cls;
	LETTUCE_SPI_DEV dev;	// Device location
	BYTE* mem;						// Instance data
	WORD	timer;					// Timer count
	struct {
		unsigned char intf : 1;
	} flags;

} LETTUCE_PLUGIN;

//-----------------------------------
// Number of plugin slots on board
//
#define LETTUCE_NUM_SLOTS 	4

//-----------------------------------
// Plugin update reasons
//
#define EVENT_TICK				0
#define EVENT_OPEN				1
#define EVENT_CLOSE				2
#define EVENT_STATE				4

//-----------------------------------
// Initialize plugin controller
//
void PluginControllerInit(void);

//-----------------------------------
// Perform plugin controller tasks
//
void PluginControllerTask(void);

//-----------------------------------
// Handle state updates
//
void PluginControllerStateEvent(void);

//-----------------------------------
// Handle high priority plugin interrupts
//
void PluginControllerHighISR(void);

//-----------------------------------
// Handle low priority plugin interrupts
//
void PluginControllerLowISR(void);

//-----------------------------------
// Get plugin given device type and index
//
LETTUCE_PLUGIN* GetPlugin(BYTE dev, BYTE index);

//-----------------------------------
// Lookup index plugin index
//
BYTE LookupIndex(LETTUCE_PLUGIN* p);

//-----------------------------------
// Lookup plugin class from SID
//
rom LETTUCE_PLUGIN_CLASS* LookupClass(WORD sid);

//-----------------------------------
// Handle plugin event
//
void PluginEvent(LETTUCE_PLUGIN* p, BYTE event);

//-----------------------------------
// Handle plugin request
//
BYTE PluginRequest(BYTE cmd, BYTE devtype, BYTE devloc, WORD addr, BUFFER* data);

//-----------------------------------
// Handle plugin interrupt
//
void PluginISR(LETTUCE_PLUGIN* p, LETTUCE_INT_TYPE type);

#endif
