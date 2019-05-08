/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce data buffer

Copyright (c) 2007 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "Buffer.h"
#include "sralloc.h"

//===========================================================================
void InitBuffer(BUFFER* buffer)
{
	buffer->type = BUFFER_EMPTY;
	buffer->size = 0;
	buffer->data = 0;
}

//===========================================================================
void AllocBuffer(BUFFER* buffer, BYTE size)
{
	DeleteBuffer(buffer);
	buffer->type = BUFFER_BINARY;
	buffer->size = size;
	buffer->data = SRAMalloc(size);
}

//===========================================================================
void SetBufferBool(BUFFER* buffer, BYTE value)
{
	DeleteBuffer(buffer);
	buffer->type = BUFFER_BOOL;
	buffer->size = 1;
	buffer->data = SRAMalloc(1);
	*buffer->data = value;
}

//===========================================================================
void SetBufferByte(BUFFER* buffer, BYTE type, BYTE value)
{
	DeleteBuffer(buffer);
	buffer->type = type;
	buffer->size = 1;
	buffer->data = SRAMalloc(1);
	*buffer->data = value;
}

//===========================================================================
void SetBufferWord(BUFFER* buffer, BYTE type, WORD value)
{
	DeleteBuffer(buffer);
	buffer->type = type;
	buffer->size = 2;
	buffer->data = SRAMalloc(2);
	*((WORD*)buffer->data) = value;
}

//===========================================================================
void SetBufferDWord(BUFFER* buffer, BYTE type, DWORD value)
{
	DeleteBuffer(buffer);
	buffer->type = type;
	buffer->size = 4;
	buffer->data = SRAMalloc(4);
	*((DWORD*)buffer->data) = value;
}

//===========================================================================
void SetBufferString(BUFFER* buffer, char* value)
{
	DeleteBuffer(buffer);
	buffer->type = BUFFER_STRING;
	buffer->size = strlen(value);
	buffer->data = SRAMalloc(buffer->size + 1);
	strcpy(buffer->data, value);
}

//===========================================================================
void SetBufferRomString(BUFFER* buffer, rom char* value)
{
	DeleteBuffer(buffer);
	buffer->type = BUFFER_STRING;
	buffer->size = strlenpgm(value);
	buffer->data = SRAMalloc(buffer->size + 1);
	strcpypgm2ram(buffer->data, value);
}

//===========================================================================
BOOL GetBufferBool(BUFFER* buffer, BYTE* value)
{
	if (buffer->type != BUFFER_BOOL || buffer->size != 1) {
		return FALSE;
	}
	*value = *buffer->data;
	return TRUE;
}

//===========================================================================
BOOL GetBufferByte(BUFFER* buffer, BYTE expected_type, BYTE* value)
{
	if (buffer->type != expected_type || buffer->size != 1) {
		return FALSE;
	}
	*value = *buffer->data;
	return TRUE;
}

//===========================================================================
BOOL GetBufferWord(BUFFER* buffer, BYTE expected_type, WORD* value)
{
	if (buffer->type != expected_type || buffer->size != 2) {
		return FALSE;
	}
	*value = *((WORD*)buffer->data);
	return TRUE;
}

//===========================================================================
BOOL GetBufferDWord(BUFFER* buffer, BYTE expected_type, DWORD* value)
{
	if (buffer->type != expected_type || buffer->size != 4) {
		return FALSE;
	}
	*value = *((DWORD*)buffer->data);
	return TRUE;
}

//===========================================================================
BOOL GetBufferString(BUFFER* buffer, char* value, BYTE max)
{
	if (buffer->type != BUFFER_STRING || buffer->size > max-1) {
		return FALSE;
	}
	strcpy(value, buffer->data);
	return TRUE;
}

//===========================================================================
void DeleteBuffer(BUFFER* buffer)
{
	if (buffer->data) {
		SRAMfree(buffer->data);
		buffer->data = 0;
	}
	buffer->size = 0;
	buffer->type = BUFFER_EMPTY;
}

//===========================================================================
BOOL UDPGetBuffer(BUFFER* buffer)
{
	DeleteBuffer(buffer);
	if (!UDPGet(&buffer->type) || !UDPGet(&buffer->size)) {
		return FALSE;		
	}	

	if (buffer->type == BUFFER_STRING) {
		buffer->data = SRAMalloc(buffer->size + 1);
		if (buffer->size != UDPGetArray(buffer->data, buffer->size)) {
			return FALSE;	
		}	
		*(buffer->data + buffer->size) = '\0';
	
	} else {
		buffer->data = SRAMalloc(buffer->size);
		if (buffer->size != UDPGetArray(buffer->data, buffer->size)) {
			return FALSE;
		}	
	}
	return TRUE;
}

//===========================================================================
void UDPPutBuffer(BUFFER* buffer)
{
	UDPPut(buffer->type);
	UDPPut(buffer->size);
	UDPPutArray(buffer->data, buffer->size);
}

//===========================================================================
BOOL TCPGetBuffer(TCP_SOCKET socket, BUFFER* buffer)
{
	DeleteBuffer(buffer);
	if (!TCPGet(socket,&buffer->type) || !TCPGet(socket,&buffer->size)) {
		return FALSE;	
	}	

	if (buffer->type == BUFFER_STRING) {
		buffer->data = SRAMalloc(buffer->size + 1);
		if (buffer->size != TCPGetArray(socket, buffer->data, buffer->size)) {
			return FALSE;
		}	
		*(buffer->data + buffer->size) = '\0';

	} else {
		buffer->data = SRAMalloc(buffer->size);
		if (buffer->size != TCPGetArray(socket, buffer->data, buffer->size)) {
			return FALSE;
		}	
	}
	return TRUE;
}

//===========================================================================
void TCPPutBuffer(TCP_SOCKET socket, BUFFER* buffer)
{
	TCPPut(socket,buffer->type);
	TCPPut(socket,buffer->size);
	TCPPutArray(socket, buffer->data, buffer->size);
}

//===========================================================================
void XEEGetBuffer(BUFFER* buffer)
{
	BYTE* p;
	BYTE i;

	DeleteBuffer(buffer);
	buffer->type = XEERead();
	buffer->size = XEERead();

	if (buffer->type == BUFFER_STRING) {
		buffer->data = SRAMalloc(buffer->size + 1);
		p = (BYTE*)buffer->data;
		for (i=0; i<buffer->size; ++i) {
			*p++ = XEERead();
		}
		*(buffer->data + buffer->size) = '\0';
	
	} else {
		buffer->data = SRAMalloc(buffer->size);
		p = (BYTE*)buffer->data;
		for (i=0; i<buffer->size; ++i) {
			*p++ = XEERead();
		}
	}
}

//===========================================================================
void XEEPutBuffer(BUFFER* buffer)
{
	BYTE* p;
	BYTE i;

	XEEWrite(buffer->type);
	XEEWrite(buffer->size);

  p = (BYTE*)buffer->data;
  for (i=0; i<buffer->size; ++i) {
		XEEWrite(*p++);
	}
}
