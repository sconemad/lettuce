/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce RTC plugin

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/SNTP.h"
#include "plugin_rtc.h"
#include "LettuceServer.h"
#include "sralloc.h"

#ifdef LETTUCE_PLUGIN_RTC

//-------------------------------------
// RTC commands
//
#define CMD_SET_RTC_FROM_SYS		0x80
#define CMD_SET_SYS_FROM_RTC		0x81


//-------------------------------------
// DS3234 memory map:
//
#define ADDR_TIME_SECOND		0x00
#define ADDR_TIME_MINUTE 		0x01
#define ADDR_TIME_HOUR 			0x02
#define ADDR_TIME_WDAY			0x03
#define ADDR_TIME_MDAY			0x04
#define ADDR_TIME_MONTHCEN	0x05
#define ADDR_TIME_YEAR			0x06

#define ADDR_ALARM1_SECOND	0x07
#define ADDR_ALARM1_MINUTE	0x08
#define ADDR_ALARM1_HOUR		0x09
#define ADDR_ALARM1_WMDAY		0x0A

#define ADDR_ALARM2_MINUTE	0x0B
#define ADDR_ALARM2_HOUR		0x0C
#define ADDR_ALARM2_WMDAY		0x0D

#define ADDR_CONTROL				0x0E
#define ADDR_STATUS					0x0F

#define ADDR_XTAL_AGING			0x10
#define ADDR_TEMP_MSB				0x11
#define ADDR_TEMP_LSB				0x12
#define ADDR_TEMP_DISABLE		0x13

#define ADDR_SRAM_ADDR			0x18
#define ADDR_SRAM_DATA			0x19

//-------------------------------------
// DS3234 commands (| with address):
//
#define CMD_READ						0x00
#define CMD_WRITE						0x80

// From SNTP to allow us to tweak system time
extern DWORD dwSNTPSeconds;
extern DWORD dwLastUpdateTick;

//-------------------------------------
// rtc plugin instance data
//
typedef struct _RTC_DATA {
	BYTE dummy;
} RTC_DATA;


BYTE ToBCD(BYTE b);
BYTE FromBCD(BYTE b);
void SetSystemFromRTC(LETTUCE_PLUGIN* p);
void SetRTCFromSystem(LETTUCE_PLUGIN* p);


//===========================================================================
// Plugin event handler
//
void PluginEvent_rtc(LETTUCE_PLUGIN* p, BYTE event)
{
	RTC_DATA* d;

	d = (RTC_DATA*)p->mem;

	switch (event) {

		case EVENT_OPEN:
			// Disable interrupts as we use polling for now
			SetPluginInt(p->dev,FALSE);

			// Set associated port to output all zeros as it isn't used
			SetPortByte(p->dev,0x00);
			SetPortTristate(p->dev,0x00);

			p->mem = SRAMalloc(sizeof(RTC_DATA));
			d = ((RTC_DATA*)p->mem);
		
			//SetSystemFromRTC(p);
			break;

		case EVENT_CLOSE:
			// Clean up our buffer
			SRAMfree(p->mem);
			p->mem = 0;
			break;
	}
}

//===========================================================================
// Plugin request handler
//
BYTE PluginRequest_rtc(LETTUCE_PLUGIN* p, BYTE cmd, WORD addr, BUFFER* buffer)
{
	switch (cmd) {
		
		case LETTUCE_CMD_SET: // Set rtc from current system time
			SetRTCFromSystem(p);
			return LETTUCE_STATUS_OK;

		case LETTUCE_CMD_GET: // Set system time from RTC
			SetSystemFromRTC(p);
			return LETTUCE_STATUS_OK;
	}

	return LETTUCE_STATUS_BAD_ADDR;
}

//===========================================================================
// Convert byte to BCD
//
BYTE ToBCD(BYTE b)
{
	BYTE t = b/10;
	return (t << 4) - (t*10) + b;
}

//===========================================================================
// Convert byte from BCD
//
BYTE FromBCD(BYTE b)
{
	return (((b & 0xF0) >> 4) * 10) + (b & 0x0F);
}

//===========================================================================
// Set the system time from the RTC
//
void SetSystemFromRTC(LETTUCE_PLUGIN* p)
{
	TM time;
	BYTE b;

	OpenSPI(p->cls->spicon);
	
	// Read current time values from RTC
	SelectPlugin(p->dev);
	WriteSPI(CMD_READ | ADDR_TIME_SECOND);
	time.sec = FromBCD(WriteSPI(0));
	time.min = FromBCD(WriteSPI(0));
	time.hour = FromBCD(WriteSPI(0));
	time.wday = FromBCD(WriteSPI(0));
	time.mday = FromBCD(WriteSPI(0));
	b = FromBCD(WriteSPI(0));
	time.year = 1900 + FromBCD(WriteSPI(0));
	SelectPlugin(LETTUCE_DEV_NONE);

	CloseSPI();	

	if (b & 0x80) time.year += 100; 
	time.mon = FromBCD(b & 0x7F);
	
	// Convert to seconds since epoch and set system time
	dwLastUpdateTick = TickGet();
	dwSNTPSeconds = mktime(&time);
}

//===========================================================================
// Set the RTC from the system time
//
void SetRTCFromSystem(LETTUCE_PLUGIN* p)
{
	DWORD epochsec;
	TM time;

	OpenSPI(p->cls->spicon);

	epochsec = SNTPGetUTCSeconds();
	gmtime(epochsec,&time);
	SelectPlugin(p->dev);
	WriteSPI(CMD_WRITE | ADDR_TIME_SECOND);
	WriteSPI(ToBCD(time.sec));
	WriteSPI(ToBCD(time.min));
	WriteSPI(ToBCD(time.hour));
	WriteSPI(ToBCD(time.wday));
	WriteSPI(ToBCD(time.mday));
	WriteSPI((time.year >= 2000 ? 0x80 : 0) | ToBCD(time.mon));
	WriteSPI(ToBCD(time.year % 100));
	SelectPlugin(LETTUCE_DEV_NONE);
	
	CloseSPI();
}

#endif
