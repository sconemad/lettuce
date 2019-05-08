/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce memory plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __PLUGIN_MEM_H
#define __PLUGIN_MEM_H

#include "PluginController.h"

#ifdef LETTUCE_PLUGIN_MEM

//-----------------------------------
// Handle events
//
void PluginEvent_mem(LETTUCE_PLUGIN* p, BYTE event);

//-----------------------------------
// Handle requests
//
BYTE PluginRequest_mem(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer);

//-----------------------------------
// SPI RAM bank handle typedef
//
typedef BYTE SPIMEM_HANDLE;

//-----------------------------------
// Value representing a bad handle
//
#define SPIMEM_BAD_HANDLE 0x00

//-----------------------------------
// Allocate a bank of SPI RAM
//
SPIMEM_HANDLE SPIMEMalloc(void);

//-----------------------------------
// Free a bank of SPI RAM
//
void SPIMEMfree(SPIMEM_HANDLE handle);

//-----------------------------------
// Read an array of n bytes at addr in the specified SPI RAM bank
//
void SPIMEMGetArray(SPIMEM_HANDLE handle, WORD addr, BYTE *data, WORD n);

//-----------------------------------
// Read an array of n bytes at addr in the specified SPI RAM bank
// *** Special version for use in interrupt service routines ***
//
void SPIMEMGetArrayISR(SPIMEM_HANDLE handle, WORD addr, BYTE *data, WORD n);

//-----------------------------------
// Write an array of n bytes at addr in the specified SPI RAM bank
//
void SPIMEMPutArray(SPIMEM_HANDLE handle, WORD addr, BYTE *data, WORD n);

//-----------------------------------
// Write an array of n bytes at addr in the specified SPI RAM bank from ROM
//
void SPIMEMPutROMArray(SPIMEM_HANDLE handle, WORD addr,ROM BYTE *data, WORD n);

#endif

#endif
