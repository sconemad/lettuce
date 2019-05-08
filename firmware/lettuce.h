/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce-specific utils

Copyright (c) 2007 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __LETTUCE_H
#define __LETTUCE_H

#include "TCPIP Stack/TCPIP.h"

#define LETTUCE_VERSION 					"v1.03"	// Lettuce firmware version

#define LETTUCE_PREFIX						'L' // Lettuce command prefix

//-------------------------------------
// Device IDs
//
#define LETTUCE_DEV_NONE					0x00u

// Plugin slots (used to address a plugin by physical slot)
#define LETTUCE_DEV_SPI_SLOT_1		0x01u
#define LETTUCE_DEV_PORT_AD1			LETTUCE_DEV_SPI_SLOT_1
#define LETTUCE_DEV_SPI_SLOT_2		0x02u
#define LETTUCE_DEV_PORT_AD2			LETTUCE_DEV_SPI_SLOT_2
#define LETTUCE_DEV_SPI_SLOT_3		0x03u
#define LETTUCE_DEV_PORT_DIG1			LETTUCE_DEV_SPI_SLOT_3
#define LETTUCE_DEV_SPI_SLOT_4		0x04u
#define LETTUCE_DEV_PORT_DIG2			LETTUCE_DEV_SPI_SLOT_4

// System devices
#define LETTUCE_DEV_INFO					0x10u
#define LETTUCE_DEV_CLOCK					0x11u
#define LETTUCE_DEV_REGISTRY			0x12u
#define LETTUCE_DEV_LOGGER				0x13u
#define LETTUCE_DEV_ETH						0x14u
#define LETTUCE_DEV_USART					0x15u

// Core devices
#define LETTUCE_DEV_EEPROM				0x40u
#define LETTUCE_DEV_AUX						0x41u
#define LETTUCE_DEV_PWM						0x42u
#define LETTUCE_DEV_COUNTER				0x43u
#define LETTUCE_DEV_BUTTONS				0x44u
#define LETTUCE_DEV_LINBUS				0x45u

// Plugin devices (used to address a plugin by type)
#define LETTUCE_DEV_PLUGIN				0x80u

#define LETTUCE_PLUGIN_BASIC			0x80u
#define LETTUCE_PLUGIN_IOPORT			0x81u
#define LETTUCE_PLUGIN_MEM				0x82u
#define LETTUCE_PLUGIN_RTC				0x83u
#define LETTUCE_PLUGIN_MP3				0x84u

//-------------------------------------
// Commands
//
#define LETTUCE_CMD_GET						0x00u
#define LETTUCE_CMD_SET						0x01u
#define LETTUCE_CMD_DIR						0x02u
#define LETTUCE_CMD_STATE					0x03u

//-------------------------------------
// Status codes
//
#define LETTUCE_STATUS_OK					0x00u
#define LETTUCE_STATUS_BAD_DEVICE	0x01u
#define LETTUCE_STATUS_BAD_ADDR		0x02u
#define LETTUCE_STATUS_BAD_CMD		0x03u
#define LETTUCE_STATUS_BAD_ARG		0x04u

//-------------------------------------
// Ports
//
#define LETTUCE_UDP_SERVER_PORT		9777u
#define LETTUCE_TCP_SERVER_PORT		9770u
#define LETTUCE_CLIENT_PORT				9779u


//===========================================================================
// MACROS
//

//-------------------------------------
// Construct a DWORD device address from type, loc and word addreess
//
#define DEV_ADDRESS(devtype,devloc,addr) ( ((DWORD)devtype << 24) + ((DWORD)devloc << 16) + (DWORD)addr )

//-------------------------------------
// HI/LOW byte from WORD
//
#define LOWBYTE(a) 		((a) & 0xFF)
#define HIGHBYTE(a) 	(((a)>>8) & 0xFF)


//===========================================================================
// Plugins
//

//-------------------------------------
// Device typedef
//
typedef BYTE LETTUCE_SPI_DEV;

//-------------------------------------
// Select SPI slave device
//
void SelectPlugin(LETTUCE_SPI_DEV dev);

//-------------------------------------
// Get device ID
//
WORD GetPluginID(LETTUCE_SPI_DEV dev);


//===========================================================================
// Interrupts
//

//-------------------------------------
// Interrupt edge
//
typedef BYTE LETTUCE_INT_EDGE;
#define INT_FALLING_EDGE 		0u
#define INT_RISING_EDGE 		1u

//-------------------------------------
// Interrupt types
//
typedef BYTE LETTUCE_INT_TYPE;
#define INT_TYPE_TIMER1			1u
#define INT_TYPE_TIMER2			2u
#define INT_TYPE_TIMER3			3u
#define INT_TYPE_TIMER4			4u
#define INT_TYPE_PERIPHERAL	8u

//-------------------------------------
// Disable interrupts (for critical processing)
//
#define ENTER_CRITICAL			INTCONbits.GIEH = 0;

//-------------------------------------
// Enable interrupts (end critical processing)
//
#define LEAVE_CRITICAL			INTCONbits.GIEH = 1;

//-------------------------------------
// Poll interrupt status
//
BOOL PollPluginInt(LETTUCE_SPI_DEV dev);

//-------------------------------------
// Get interrupt flag
//
BOOL GetPluginInt(LETTUCE_SPI_DEV dev);

//-------------------------------------
// Setup interrupt for plugin
//
void SetupPluginInt(LETTUCE_SPI_DEV dev, LETTUCE_INT_EDGE edge);

//-------------------------------------
// Enable/disable interrupt for plugin
//
void SetPluginInt(LETTUCE_SPI_DEV dev, BOOL active);

//-------------------------------------
// Clear plugin interrupt flag
//
void ClearPluginInt(LETTUCE_SPI_DEV dev);


//===========================================================================
// Ports
//

//-------------------------------------
// Port typedef
//
typedef BYTE LETTUCE_PORT;

//-------------------------------------
// Set tristate of port
//
void SetPortTristate(LETTUCE_PORT port, BYTE val);

//-------------------------------------
// Get tristate of port
//
BYTE GetPortTristate(LETTUCE_PORT port);

//-------------------------------------
// Set whole byte on port
//
void SetPortByte(LETTUCE_PORT port, BYTE val);

//-------------------------------------
// Get byte from port
//
BYTE GetPortByte(LETTUCE_PORT port);

//-------------------------------------
// Set individual bit on port
//
void SetPortBit(LETTUCE_PORT port, BYTE bitnum, BOOL value);

//-------------------------------------
// Get individual bit from port
//
BOOL GetPortBit(LETTUCE_PORT port, BYTE bitnum);


//===========================================================================
// Analog to Digital converter (ADC)
//

//-------------------------------------
// ADC mode
//
#define ADC_MODE_AUX			0x00u // SID + AUX
#define ADC_MODE_AUX_AD1	0x01u // SID + AUX + AD1
#define ADC_MODE_AUX_AD12	0x02u // SID + AUX + AD1 + AD2

//-------------------------------------
// Init ADC and calibrate
//
void ADCInit(void);

//-------------------------------------
// Set the ADC mode
//
void SetADCMode(BYTE mode);

//-------------------------------------
// Get analog reading from port
//
WORD GetPortAnalog(LETTUCE_PORT port, BYTE input);


//===========================================================================
// SPI
//

//-------------------------------------
// Sync mode
//
#define SPI_FOSC_4    0x00u	// SPI Master mode, clock = Fosc/4
#define SPI_FOSC_16   0x01u	// SPI Master mode, clock = Fosc/16
#define SPI_FOSC_64   0x02u	// SPI Master mode, clock = Fosc/64
#define SPI_FOSC_TMR2 0x03u	// SPI Master mode, clock = TMR2 output/2
#define SLV_SSON      0x04u	// SPI Slave mode, /SS pin control enabled
#define SLV_SSOFF     0x05u	// SPI Slave mode, /SS pin control disabled

//-------------------------------------
// Bus mode
//
#define BUS_MODE_00		0x00u	// Idle low, transmit on rising edge
#define BUS_MODE_01		0x08u	// Idle low, transmit on falling edge
#define BUS_MODE_10		0x10u	// Idle high, transmit on falling edge
#define BUS_MODE_11		0x18u	// Idle high, transmit on rising edge

//-------------------------------------
// Sample phase
//
#define SMP_PHASE_END	0x20u	// Sample at end
#define SMP_PHASE_MID	0x00u	// Sample in middle

//-------------------------------------
// Setup SPI with specified mode
// SPI must not be already in use
//
BOOL OpenSPI(BYTE mode);

//-------------------------------------
// Write a byte to SPI, return byte read
//
BYTE WriteSPI(BYTE output);

//-------------------------------------
// Close SPI (mark as not in use)
//
void CloseSPI(void);


//===========================================================================
// Time
//

//-------------------------------------
// Time structure
//
typedef struct _TM {
	BYTE sec;
	BYTE min;
	BYTE hour;
	BYTE wday;
	BYTE mday;
	BYTE mon;
	WORD year;
} TM;

//-------------------------------------
// Convert epoch_seconds to human readable date and time values
//
void gmtime(const DWORD epoch_seconds, TM *time);

//-------------------------------------
// Convert human readable date and time values to epoch_seconds
//
DWORD mktime(const TM* time);

#endif
