/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce Registry

Copyright (c) 2009 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __LETTUCE_REGISTRY_H
#define __LETTUCE_REGISTRY_H

#include "lettuce.h"
#include "Buffer.h"

//-------------------------------------
// Registry entry types
//
#define REG_EMPTY									0x00u
#define REG_NAME									0x01u
#define REG_CONDITION							0x02u
#define REG_EXTERNAL							0x03u
#define REG_IOTYPE								0x04u
#define REG_TIME									0x05u
#define REG_VALUE									0x06u

//-------------------------------------
// Expression syntax elements
//
#define EXPR_END									0x00u
#define EXPR_INPUT								0x10u
#define EXPR_NOT_INPUT						0x11u
#define EXPR_AND									0x20u
#define EXPR_OR										0x21u
#define EXPR_XOR									0x22u

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_registry(BYTE cmd, WORD addr, BUFFER* buffer);

//-------------------------------------
// Get registry entry
// Retrieve the registry entry with the specified address and type.
// Returns TRUE on success, FALSE if a matching entry cannot be found.
//
BOOL GetRegEntry(BYTE type, DWORD addr, BUFFER* buffer);

//-------------------------------------
// Set registry entry
// Sets a registry entry with the specified address and type to the
// specified value. If an existing entry with this address and type does not
// exist, a new entry is added.
// Returns TRUE on success, FALSE if the entry could not be set.
//
BOOL SetRegEntry(BYTE type, DWORD addr, BUFFER* buffer);

//-------------------------------------
// Delete registry entry
// Deletes a registry entry with the specified address and type.
// Returns TRUE on success, FALSE if the entry could not be found.
//
BOOL DeleteRegEntry(BYTE type, DWORD addr);

//-------------------------------------
// Clear registry
// Erases all registry entries.
//
void ClearRegistry(void);

//-------------------------------------
// Compact registry
// Reduces wasted space in the registry storage, where possible.
//
void CompactRegistry(void);

//-------------------------------------
// Get registry entry by name
// Searches the registry for an entry of type NAME matching the specified 
// name value, retrieving its address.
// Returns TRUE on success, FALSE if a matching entry cannot be found.
//
BOOL LookupRegEntryByName(char* name, DWORD* addr);

//-------------------------------------
// Evaluate an expression
// Evaluates a binary expression, resolving any referenced entries.
// Returns the result of the expression.
//
BYTE EvaluateExpr(char* expr);

//-------------------------------------
// Determine expression depencency
// Determines whether an expression references the specified entry.
//
BOOL ExprDepends(char* expr, DWORD addr);

#endif
