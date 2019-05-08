/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce data buffer

Copyright (c) 2007 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __BUFFER_H
#define __BUFFER_H

#include "lettuce.h"

//-------------------------------------
// Buffer types
//
#define BUFFER_EMPTY							0x00u
#define BUFFER_BOOL								0x10u
#define BUFFER_UINT								0x20u
#define BUFFER_INT								0x21u
#define BUFFER_REAL								0x30u
#define BUFFER_STRING							0x40u
#define BUFFER_BINARY							0x50u
#define BUFFER_IPADDR							0x60u
#define BUFFER_IP6ADDR						0x61u

//-------------------------------------
// Buffer data structure
//
typedef struct _BUFFER {
	BYTE type;
	BYTE size;
	BYTE* data;
} BUFFER;

//-------------------------------------
// Initialise a buffer
// Must be called on any newly declated buffers before using.
//
void InitBuffer(BUFFER* buffer);

//-------------------------------------
// Allocate a buffer
// Allocates the specified amount of memory, if possible, and
// sets the buffer type to binary.
//
void AllocBuffer(BUFFER* buffer, BYTE size);

//-------------------------------------
// Set buffer as boolean
// Sets the buffer to boolean type (allocating 1 byte) with the specified 
// value.
//
void SetBufferBool(BUFFER* buffer, BYTE value);

//-------------------------------------
// Set buffer as byte
// Sets the buffer to the specified type (allocating 1 byte) with the
// specified value.
//
void SetBufferByte(BUFFER* buffer, BYTE type, BYTE value);

//-------------------------------------
// Set buffer as word
// Sets the buffer to the specified type (allocating 2 bytes) with the
// specified value.
//
void SetBufferWord(BUFFER* buffer, BYTE type, WORD value);

//-------------------------------------
// Set buffer as double-word
// Sets the buffer to the specified type (allocating 4 bytes) with the
// specified value.
//
void SetBufferDWord(BUFFER* buffer, BYTE type, DWORD value);

//-------------------------------------
// Set buffer as string (initialised from RAM)
// Sets the buffer to string type (allocating the number of bytes required
// to hold the string, if possible) and copies over the string.
//
void SetBufferString(BUFFER* buffer, char* value);

//-------------------------------------
// Set buffer as string (initiased from ROM)
// Sets the buffer to string type (allocating the number of bytes required
// to hold the string, if possible) and copies over the string from ROM.
//
void SetBufferRomString(BUFFER* buffer, rom char* value);

//-------------------------------------
// Gets boolean value from buffer
// Retrieves the boolean value stored in the buffer.
// Returns TRUE on success, or FALSE if the buffer does not contain a 
// boolean type.
//
BOOL GetBufferBool(BUFFER* buffer, BYTE* value);

//-------------------------------------
// Gets a byte value from buffer
// Retrieves the byte value of the expected type stored in the buffer.
// Returns TRUE on success, or FALSE if the buffer does not contain a 
// byte value of the expected type.
//
BOOL GetBufferByte(BUFFER* buffer, BYTE expected_type, BYTE* value);

//-------------------------------------
// Gets a word value from buffer
// Retrieves the word value of the expected type stored in the buffer.
// Returns TRUE on success, or FALSE if the buffer does not contain a 
// word value of the expected type.
//
BOOL GetBufferWord(BUFFER* buffer, BYTE expected_type, WORD* value);

//-------------------------------------
// Gets a double-word value from buffer
// Retrieves the double-word value of the expected type stored in the buffer.
// Returns TRUE on success, or FALSE if the buffer does not contain a 
// double-word value of the expected type.
//
BOOL GetBufferDWord(BUFFER* buffer, BYTE expected_type, DWORD* value);

//-------------------------------------
// Gets a string value from buffer
// Retrieves the string value stored in the buffer, upto the maximum
// specified size.
// Returns TRUE on success, or FALSE if the buffer does not contain a 
// string value, or if it exceeds the maxiumum size specified.
//
BOOL GetBufferString(BUFFER* buffer, char* value, BYTE max);

//-------------------------------------
// Delete a buffer
// Frees up any allocated memory associated with the buffer. 
// This must be called on a buffer before it goes out of scope otherwise 
// memory will be leaked.
//
void DeleteBuffer(BUFFER* buffer);

//-------------------------------------
// Receive a serialized buffer from a UDP socket
// Returns TRUE if the whole buffer was read from the socket.
//
BOOL UDPGetBuffer(BUFFER* buffer);

//-------------------------------------
// Send a serialized buffer to a UDP socket
//
void UDPPutBuffer(BUFFER* buffer);

//-------------------------------------
// Receive a serialized buffer from a TCP socket
// Returns TRUE if the whole buffer was read from the socket.
//
BOOL TCPGetBuffer(TCP_SOCKET socket, BUFFER* buffer);

//-------------------------------------
// Send a serialized buffer to a TCP socket
//
void TCPPutBuffer(TCP_SOCKET socket, BUFFER* buffer);

//-------------------------------------
// Read a serialized buffer from EEPROM storage
//
void XEEGetBuffer(BUFFER* buffer);

//-------------------------------------
// Write a serialized buffer to EEPROM storage
//
void XEEPutBuffer(BUFFER* buffer);

#endif /* __BUFFER_H */
