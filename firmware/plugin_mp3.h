/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce MP3 plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __PLUGIN_MP3_H
#define __PLUGIN_MP3_H

#include "PluginController.h"

#ifdef LETTUCE_PLUGIN_MP3

//-----------------------------------
// Handle events
//
void PluginEvent_mp3(LETTUCE_PLUGIN* p, BYTE event);

//-----------------------------------
// Handle requests
//
BYTE PluginRequest_mp3(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer);

//-----------------------------------
// Handle interrupts
//
void PluginISR_mp3(LETTUCE_PLUGIN* p, LETTUCE_INT_TYPE type);

//-----------------------------------
// Set audio volume
//
void SetVolume(LETTUCE_PLUGIN* p, BYTE vRight, BYTE vLeft);

//-----------------------------------
// Set audio bass boost
//
void SetBassBoost(LETTUCE_PLUGIN* p, BYTE bass, BYTE gfreq);

#endif

#endif
