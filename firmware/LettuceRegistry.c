/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce Registry

Copyright (c) 2009 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "LettuceRegistry.h"
#include "LettuceServer.h"
#include "LettuceSystem.h"


//-------------------------------------
// Registry EEPROM format:
//
// ...
//-
// WORD 	entry_length
// BYTE		type
// DWORD	addr
// BYTE   buffer_type
// BYTE   buffer_size (where buffer_size <= (entry_length - 7))
// BYTE[buffer_size] buffer_data
// BYTE[entry_length - 7 - buffer_size] padding
//-
// ...
//
// end is marked with 0 entry_length

#define ADDR_START 	0
#define ADDR_END 		(MPFS_RESERVE_BLOCK)

#define REG_HEADER_LEN (sizeof(REG_ENTRY) - sizeof(BUFFER))


//===========================================================================
BYTE ModuleRequest_registry(BYTE cmd, WORD addr, BUFFER* buffer)
{
	static BYTE reg_type;
	static DWORD reg_addr;
	static WORD w;
	BYTE b;

	switch (addr) {

		case 0x0000: // Entry type cursor
			if (cmd == LETTUCE_CMD_SET) {
				if (!GetBufferByte(buffer,BUFFER_UINT,&reg_type)) {
					return LETTUCE_STATUS_BAD_ARG;
				}
			}
			SetBufferByte(buffer,BUFFER_UINT,reg_type);
			return LETTUCE_STATUS_OK;

		case 0x0001: // Entry address cursor
			if (cmd == LETTUCE_CMD_SET) {
				if (!GetBufferDWord(buffer,BUFFER_UINT,&reg_addr)) {
					return LETTUCE_STATUS_BAD_ARG;
				}
			}
			SetBufferDWord(buffer,BUFFER_UINT,reg_addr);
			return LETTUCE_STATUS_OK;

		case 0x0010: // Entry data (reads/writes the entry)
			if (cmd == LETTUCE_CMD_SET) {
				return !SetRegEntry(reg_type,reg_addr,buffer);
			}
			return !GetRegEntry(reg_type,reg_addr,buffer);

		case 0x0011: // Lookup entry by name
			if (cmd != LETTUCE_CMD_GET) {
				return LETTUCE_STATUS_BAD_CMD;
			}
			if (buffer->type != BUFFER_STRING) {
				return LETTUCE_STATUS_BAD_ARG;
			}
			reg_addr = 0;
			reg_type = REG_NAME;
			if (!LookupRegEntryByName(buffer->data,&reg_addr)) {
				return LETTUCE_STATUS_BAD_ARG;
			}
			SetBufferDWord(buffer,BUFFER_UINT,reg_addr);
			return LETTUCE_STATUS_OK;

		case 0x0012: // Delete entry
			if (cmd != LETTUCE_CMD_SET) {
				return LETTUCE_STATUS_BAD_CMD;
			}
			return !DeleteRegEntry(reg_type,reg_addr);

		case 0xFFFE:
			if (cmd == LETTUCE_CMD_SET) {
				CompactRegistry();
			}
			return LETTUCE_STATUS_OK;

		case 0xFFFF:
			if (cmd == LETTUCE_CMD_SET) {
				ClearRegistry();
			}
			return LETTUCE_STATUS_OK;
	}

	return LETTUCE_STATUS_BAD_ADDR;
}

//===========================================================================
void XEEReadWord(WORD* w)
{
	BYTE* p = (BYTE*)w;
	*p++ = XEERead();
	*p++ = XEERead();
}

//===========================================================================
void XEEReadDWord(DWORD* d)
{
	BYTE* p = (BYTE*)d;
	*p++ = XEERead();
	*p++ = XEERead();
	*p++ = XEERead();
	*p++ = XEERead();
}

//===========================================================================
void XEEWriteWord(WORD* w)
{
	BYTE* p = (BYTE*)w;
	XEEWrite(*p++);
	XEEWrite(*p++);
}

//===========================================================================
void XEEWriteDWord(DWORD* d)
{
	BYTE* p = (BYTE*)d;
	XEEWrite(*p++);
	XEEWrite(*p++);
	XEEWrite(*p++);
	XEEWrite(*p++);
}

//===========================================================================
WORD ReadHeader(BYTE* rec_type, DWORD* rec_addr)
{
	WORD rec_len;

	// Read the record length
  XEEReadWord(&rec_len);

	if (rec_len == 0u) {
		// Reached the end of the registry
		return 0;
	}

	// Read the record type
	*rec_type = XEERead();

	// Read the record address
  XEEReadDWord(rec_addr);

	return rec_len;
}


//===========================================================================
BOOL GetRegEntry(BYTE type, DWORD addr, BUFFER* buffer)
{
	DWORD a;
	WORD rec_len;
	BYTE rec_type;
	DWORD rec_addr;

	a = ADDR_START;

	while (1) {

	  XEEBeginRead(a);

		// Read the header
		rec_len = ReadHeader(&rec_type, &rec_addr);

		if (rec_len == 0u) {
			// Reached the end of the registry
			XEEEndRead();
			return FALSE;
		}

		if (addr == rec_addr && type == rec_type) {
			// We want this record, read it
			XEEGetBuffer(buffer);
			XEEEndRead();
			break;
		}

		XEEEndRead();

		a += 2 + rec_len;
	}

	return TRUE;
}

//===========================================================================
BOOL SetRegEntry(BYTE type, DWORD addr, BUFFER* buffer)
{
	DWORD a;
	WORD rec_len;
	BYTE rec_type;
	DWORD rec_addr;

	a = ADDR_START;

	// Find a suitable address to write the entry
	while (1) {

	  XEEBeginRead(a);

		// Read the header
		rec_len = ReadHeader(&rec_type, &rec_addr);

		if (rec_len == 0u) {
			// Reached the end of the registry
			XEEEndRead();
			break;
		}

		XEEEndRead();

		if (addr == rec_addr && type == rec_type) {
			// Found the entry to replace
			if (buffer->size <= (rec_len - 7)) {
				// There is room to write the new data at a
				break;
			} else {
				// No room to write the new data - erase the current record
				XEEBeginWrite(a + 2);
				XEEWrite(REG_EMPTY);
				XEEEndWrite();
				// Continue looping to find the end of the registry
			}
		}

		// Move to next record
		a += 2u + rec_len;
	}

	if (a + 9u + buffer->size > ADDR_END) {
		// Cannot fit the new entry
		return FALSE;
	}

	if (rec_len == 0u) {
		// Write new record at end
		XEEBeginWrite(a);

		// Calculate and write the record length
		rec_len = 7u + buffer->size;
	  XEEWriteWord(&rec_len);

	} else {
		// Overwrite existing record
		XEEBeginWrite(a + 2u);
	}

	// Write the record type
	XEEWrite(type);

	// Write the record address
 	XEEWriteDWord(&addr);

	// Write the new data
	XEEPutBuffer(buffer);

	XEEEndWrite();
	return TRUE;
}

//===========================================================================
BOOL DeleteRegEntry(BYTE type, DWORD addr)
{
	DWORD a;
	WORD rec_len;
	BYTE rec_type;
	DWORD rec_addr;

	a = ADDR_START;

	while (1) {

	  XEEBeginRead(a);

		// Read the header
		rec_len = ReadHeader(&rec_type, &rec_addr);

		XEEEndRead();

		if (rec_len == 0u) {
			// Reached the end of the registry
			return FALSE;
		}

		if (addr == rec_addr && type == rec_type) {
			// Found record, mark it as empty
			XEEBeginWrite(a + 2);
			XEEWrite(REG_EMPTY);
			XEEEndWrite();
			break;
		}

		a += 2u + rec_len;
	}

	return TRUE;
}

//===========================================================================
void ClearRegistry(void)
{
	DWORD a;

	a = ADDR_START;
  XEEBeginWrite(a);

	while (a < ADDR_END) {
		XEEWrite(0);
		++a;
	}

	XEEEndWrite();
	Log(LETTUCE_DEV_REGISTRY,0,(rom char*)"reg reset");
}

//===========================================================================
void CompactRegistry(void)
{
	DWORD a;
	WORD rec_len;
	WORD new_rec_len;
	BYTE rec_type;
	DWORD rec_addr;
	BUFFER buffer;

	DWORD b;
	BOOL c;

	InitBuffer(&buffer);
	a = ADDR_START;
	c = FALSE;

	while (1) {

	  XEEBeginRead(a);

		// Read the header
		rec_len = ReadHeader(&rec_type, &rec_addr);

		if (rec_len == 0u) {
			// Reached the end of the registry
			XEEEndRead();

			if (c) {
				// Write end at b
				XEEBeginWrite(b);
			  XEEWriteWord(&rec_len);
				XEEEndWrite();
			}
			break;
		}

		if (!c && rec_type == REG_EMPTY) {
			// First time found an empty entry
			c = TRUE;
			b = a;
		}

		if (c && rec_type != REG_EMPTY) {
			// mv a -> b
			XEEGetBuffer(&buffer);
			XEEEndRead();

			// Rewrite the record at b
			XEEBeginWrite(b);
		
			// Calculate and write the new record length
			new_rec_len = 7u + buffer.size;
		  XEEWriteWord(&new_rec_len);
		
			// Write the record type
			XEEWrite(rec_type);
		
			// Write the record address
		 	XEEWriteDWord(&rec_addr);
		
			// Write the new data
			XEEPutBuffer(&buffer);
		
			XEEEndWrite();

			b += 2u + new_rec_len;
		
		} else {
			XEEEndRead();
		}

		// Move to next record
		a += 2u + rec_len;
	}

	DeleteBuffer(&buffer);
}

//===========================================================================
BOOL LookupRegEntryByName(char* name, DWORD* addr)
{
	DWORD a;
	WORD rec_len;
	BYTE rec_type;
	DWORD rec_addr;
	BUFFER buffer;
	BOOL found;

	found = FALSE;
	InitBuffer(&buffer);
	a = ADDR_START;

	while (1) {

	  XEEBeginRead(a);

		// Read the header
		rec_len = ReadHeader(&rec_type, &rec_addr);

		if (rec_len == 0u) {
			// Reached the end of the registry
			break;
		}

		if (rec_type == REG_NAME) {
			// Only interested in names

			// We want this record, read it
			XEEGetBuffer(&buffer);

			if (strcmp(name,buffer.data) == 0) {
				*addr = rec_addr;
				found = TRUE;
				break;
			}
		}

		XEEEndRead();

		// Move to next record
		a += 2u + rec_len;
	}

	XEEEndRead();
	DeleteBuffer(&buffer);

	return found;
}

//===========================================================================
BYTE EvaluateExpr(char* expr)
{
	BYTE i;
	BYTE a;
	BYTE b;
	BYTE c;
	BYTE op;
	BYTE* p;
	DWORD addr;
	BUFFER buffer;

	InitBuffer(&buffer);
	op = EXPR_END;
	a = 0;

	for (i=0; (c = expr[i]) != EXPR_END; ++i) {

		if (c == EXPR_INPUT || c == EXPR_NOT_INPUT) {

			// Handle short circuits
			if (op == EXPR_AND && a == 0u) {
				break;
			}	
			if (op == EXPR_OR && a != 0u) {
				break;
			}

			// Evaluate the input
			addr = *((DWORD*)(&expr[i+1])); i+=4;
			b = 0;
			if (LETTUCE_STATUS_OK == HandleRequest(LETTUCE_CMD_GET,addr,&buffer)) {
				b = *buffer.data;
			}

			if (c == EXPR_NOT_INPUT) {
				b = !b;
			}

			if (op == EXPR_END) {
				// Initial, store as a
				a = b;

			} else {
				// Evaluate "a <op> b" and store result in a
				switch (op) {
					case EXPR_AND: a = (a && b); break;
					case EXPR_OR:  a = (a || b); break;
					case EXPR_XOR: a = (!!a ^ !!b);  break;
				}
			}

		} else {
			op = c;
		}
	}

	DeleteBuffer(&buffer);
	return a;
}

//===========================================================================
BOOL ExprDepends(char* expr, DWORD addr)
{
	BYTE i;

	for (i=0; expr[i]!=EXPR_END; ++i) {
		switch (expr[i]) {

			case EXPR_INPUT:
			case EXPR_NOT_INPUT:
				if (*((DWORD*)(&expr[i+1])) == addr) {
					return TRUE;
				}
				i+=4;
				break;
		}
	}

	return FALSE;
}
