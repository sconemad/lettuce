/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce server

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __LETTUCE_SERVER_H
#define __LETTUCE_SERVER_H

#include "lettuce.h"
#include "PluginController.h"

//-------------------------------------
// Handle requests
// All server requests are sent here, where they are dispatched to the
// appropriate device module, or to the plugin controller for handling.
//
BYTE HandleRequest(BYTE cmd, DWORD addr, BUFFER* buffer);

//-------------------------------------
// Perform lettuce TCP server tasks
// Handles any incoming lettuce protocol requests over TCP
//
void LettuceTCPServer(void);

//-------------------------------------
// Perform lettuce UDP server tasks
// Handles any incoming lettuce protocol requests over UDP
//
void LettuceUDPServer(void);

#endif
