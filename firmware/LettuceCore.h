/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce Core devices

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __LETTUCE_CORE_H
#define __LETTUCE_CORE_H

#include "lettuce.h"
#include "Buffer.h"

//===========================================================================
// EEPROM device
//
#ifdef LETTUCE_DEV_EEPROM

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_eeprom(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer);

#endif


//===========================================================================
// AUX device
//
#ifdef LETTUCE_DEV_AUX

//-------------------------------------
// Addresses
//
#define AUX_LED										0x00u
#define AUX_TEMPERATURE						0x01u

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_aux(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer);

#endif


//===========================================================================
// PWM device
//
#ifdef LETTUCE_DEV_PWM

//-------------------------------------
// Addresses
//
#define PWM_DUTY1									0x00u
#define PWM_PERIOD1								0x01u

//-------------------------------------
// Initialise module
//
void ModuleInit_pwm(void);

//-------------------------------------
// Perform module tasks
//
void ModuleTask_pwm(void);

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_pwm(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer);

//-------------------------------------
// Handle module interrupts
//
void ModuleISR_pwm(void);

#endif


//===========================================================================
// COUNTER device
//
#ifdef LETTUCE_DEV_COUNTER

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_counter(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer);

#endif

//===========================================================================
// BUTTONS device
//
#ifdef LETTUCE_DEV_BUTTONS

//-------------------------------------
// Addresses
//
#define BUTTONS_0									0x00u
#define BUTTONS_1									0x01u
#define BUTTONS_2									0x02u
#define BUTTONS_3									0x03u
#define BUTTONS_4									0x04u
#define BUTTONS_5									0x05u

//-------------------------------------
// Functions
//
void ModuleTask_buttons(void);

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_buttons(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer);

#endif


//===========================================================================
// LINBUS device
//
#ifdef LETTUCE_DEV_LINBUS

//-------------------------------------
// Handle requests
//
BYTE ModuleRequest_linbus(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer);

#endif

#endif
