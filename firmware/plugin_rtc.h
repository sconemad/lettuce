/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce RTC plugin

Communicates with Dallas DS3234 real-time clock chip

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __PLUGIN_RTC_H
#define __PLUGIN_RTC_H

#include "PluginController.h"

#ifdef LETTUCE_PLUGIN_RTC

//-----------------------------------
// Handle events
//
void PluginEvent_rtc(LETTUCE_PLUGIN* p, BYTE event);

//-----------------------------------
// Handle requests
//
BYTE PluginRequest_rtc(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer);

#endif

#endif
