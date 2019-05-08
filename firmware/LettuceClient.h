/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce client

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __LETTUCE_CLIENT_H
#define __LETTUCE_CLIENT_H

#include "lettuce.h"

//-------------------------------------
// Broadcast an event over ethernet
//
void BroadcastEvent(rom char* msg);
void BroadcastEventRam(char* msg);

//-------------------------------------
// Broadcast device state
//
void BroadcastState(DWORD source_addr, BYTE value);

//-------------------------------------
// Perform lettuce client tasks
// Sends any queued event or state broadcasts.
//
void LettuceClient(void);

//-------------------------------------
// Perform lettuce state client tasks
// Informs modules of any device state updates.
//
void LettuceStateClient(void);

#endif
