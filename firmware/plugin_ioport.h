/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce I/O port plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __PLUGIN_IOPORT_H
#define __PLUGIN_IOPORT_H

#include "PluginController.h"

#ifdef LETTUCE_PLUGIN_IOPORT

//-----------------------------------
// Handle events
//
void PluginEvent_ioport(LETTUCE_PLUGIN* p, BYTE event);

//-----------------------------------
// Handle requests
//
BYTE PluginRequest_ioport(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer);

//-----------------------------------
// Handle interrupts
//
void PluginISR_ioport(LETTUCE_PLUGIN* p, LETTUCE_INT_TYPE type);

#endif

#endif
