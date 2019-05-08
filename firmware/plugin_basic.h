/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce basic plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __PLUGIN_BASIC_H
#define __PLUGIN_BASIC_H

#include "PluginController.h"

#ifdef LETTUCE_PLUGIN_BASIC

//-----------------------------------
// Handle events
//
void PluginEvent_basic(LETTUCE_PLUGIN* p, BYTE event);

//-----------------------------------
// Handle requests
//
BYTE PluginRequest_basic(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer);

//-----------------------------------
// Handle interrupts
//
void PluginISR_basic(LETTUCE_PLUGIN* p, LETTUCE_INT_TYPE type);

#endif

#endif
