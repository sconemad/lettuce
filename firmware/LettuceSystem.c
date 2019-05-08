/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce System devices

Copyright (c) 2008 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "LettuceSystem.h"
#include "LettuceServer.h"
#include "LettuceRegistry.h"
#include "LettuceClient.h"

//===========================================================================
// INFO device
//

rom char* lettuce_ver = "Lettuce " LETTUCE_VERSION;
rom char* tcpip_ver = "Microchip TCPIP Stack " VERSION;
rom char* build_ver = "Built " __DATE__ " " __TIME__;

extern APP_CONFIG AppConfig;

//===========================================================================
BYTE ModuleRequest_info(BYTE cmd, WORD addr, BUFFER* buffer)
{
	BYTE b;

	// INFO device only allows GET commands
	if (cmd != LETTUCE_CMD_GET) {
		return LETTUCE_STATUS_BAD_CMD;
	}

	switch (addr) {

		case INFO_VERSION: // Lettuce firmware version
			SetBufferRomString(buffer,lettuce_ver);
			return LETTUCE_STATUS_OK;

		case INFO_TCPIPVER: // TCP/IP stack version
			SetBufferRomString(buffer,tcpip_ver);
			return LETTUCE_STATUS_OK;

		case INFO_BUILD: // Firmware build time
			SetBufferRomString(buffer,build_ver);
			return LETTUCE_STATUS_OK;
	}

	return LETTUCE_STATUS_BAD_ADDR;
}


//===========================================================================
// CLOCK device
//

//===========================================================================
BYTE ModuleRequest_clock(BYTE cmd, WORD addr, BUFFER* buffer)
{
//	TM time;

	switch (addr) {

		case CLOCK_TIME: // Current UTC time
			SetBufferDWord(buffer,BUFFER_UINT,SNTPGetUTCSeconds());
			return LETTUCE_STATUS_OK;		

		case CLOCK_TICKS: // Current tick value
			SetBufferDWord(buffer,BUFFER_UINT,TickGet());
			return LETTUCE_STATUS_OK;		
	}

	return LETTUCE_STATUS_BAD_ADDR;
}


//-------------------------------------
// LOGGER device
//

static BYTE g_logger_mode = LOGGER_MODE_BOOT;

//===========================================================================
void ModuleInit_logger(void)
{
	g_logger_mode = LOGGER_MODE_ETH;
}	

//===========================================================================
void ModuleTask_logger(void)
{
	
}	

//===========================================================================
BYTE ModuleRequest_logger(BYTE cmd, WORD addr, BUFFER* buffer)
{
	return LETTUCE_STATUS_OK;	
}	

//===========================================================================
void Log(BYTE dev, BYTE devloc, rom char* msg)
{
	BYTE str[6];
/*	
	if (g_logger_mode >= LOGGER_MODE_BOOT) {
		putrsUART((rom char*)"Log ");

		uitoa((WORD)dev,str);
		putsUART(str);

		putrsUART((rom char*)" ");

		uitoa((WORD)devloc,str);
		putsUART(str);

		putrsUART((rom char*)" ");

		putrsUART(msg);

		putrsUART((rom char*)"\n");
	}	
*/
	if (g_logger_mode == LOGGER_MODE_ETH) {
		BroadcastEvent(msg);
	}	
	
}


//===========================================================================
// ETH device
//

//===========================================================================
// Uncomment these two pragmas for production MAC address 
// serialization if using C18. The MACROM=0x1FFF0 statement causes 
// the MAC address to be located at aboslute program memory address 
// 0x1FFF0 for easy auto-increment without recompiling the stack for 
// each device made.  Note, other compilers/linkers use a different 
// means of allocating variables at an absolute address.  Check your 
// compiler documentation for the right method.
//#pragma romdata MACROM=0x1FFF0
static ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
//#pragma romdata

//===========================================================================
void ModuleInit_eth(void)
{
	AppConfig.Flags.bIsDHCPEnabled = TRUE;
	AppConfig.Flags.bInConfigMode = TRUE;
	memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
	AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
	AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
	AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
	AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
	AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
	AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
	AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;

	// Load the default NetBIOS Host Name
	memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
	FormatNetBIOSName(AppConfig.NetBIOSName);

	// Populate AppConfig from system registry
	RefreshAppConfig();
}

//===========================================================================
BYTE ModuleRequest_eth(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer)
{
	BYTE b;

	// ETH device only allows GET commands
	if (cmd != LETTUCE_CMD_GET) {
		return LETTUCE_STATUS_BAD_CMD;
	}

	switch (addr) {

		case ETH_IPADDR:
			SetBufferDWord(buffer,BUFFER_IPADDR,AppConfig.MyIPAddr.Val);
			return LETTUCE_STATUS_OK;

		case ETH_NETMASK:
			SetBufferDWord(buffer,BUFFER_IPADDR,AppConfig.MyMask.Val);
			return LETTUCE_STATUS_OK;

		case ETH_GATEWAY:
			SetBufferDWord(buffer,BUFFER_IPADDR,AppConfig.MyGateway.Val);
			return LETTUCE_STATUS_OK;

		case ETH_PRIDNS:
			SetBufferDWord(buffer,BUFFER_IPADDR,AppConfig.PrimaryDNSServer.Val);
			return LETTUCE_STATUS_OK;

		case ETH_SECDNS:
			SetBufferDWord(buffer,BUFFER_IPADDR,AppConfig.SecondaryDNSServer.Val);
			return LETTUCE_STATUS_OK;

		case ETH_DEFIPADDR:
			SetBufferDWord(buffer,BUFFER_IPADDR,AppConfig.DefaultIPAddr.Val);
			return LETTUCE_STATUS_OK;

		case ETH_DEFNETMASK:
			SetBufferDWord(buffer,BUFFER_IPADDR,AppConfig.DefaultMask.Val);
			return LETTUCE_STATUS_OK;

		case ETH_NAME:
			SetBufferString(buffer,AppConfig.NetBIOSName);
			return LETTUCE_STATUS_OK;

		case ETH_DHCP:
			SetBufferBool(buffer,AppConfig.Flags.bIsDHCPEnabled);
			return LETTUCE_STATUS_OK;

		case ETH_CONFIGMODE:
			SetBufferBool(buffer,AppConfig.Flags.bInConfigMode);
			return LETTUCE_STATUS_OK;

		case ETH_MACADDR:
			AllocBuffer(buffer,sizeof(MAC_ADDR));
			memcpy((void*)buffer->data,(void*)AppConfig.MyMACAddr.v,sizeof(MAC_ADDR));
			return LETTUCE_STATUS_OK;
	}

	return LETTUCE_STATUS_BAD_ADDR;
}

//===========================================================================
void RefreshAppConfig(void)
{
	BYTE i,b;
	BUFFER buffer;

	InitBuffer(&buffer);

	for (i=ETH_IPADDR; i<=ETH_MACADDR; ++i) {
		if (!GetRegEntry(REG_VALUE,DEV_ADDRESS(LETTUCE_DEV_ETH,0,i),&buffer)) {
			continue;
		}
		switch (i) {
			case ETH_IPADDR:
				GetBufferDWord(&buffer,BUFFER_IPADDR,&AppConfig.MyIPAddr.Val);
				break;
			case ETH_NETMASK:
				GetBufferDWord(&buffer,BUFFER_IPADDR,&AppConfig.MyMask.Val);
				break;
			case ETH_GATEWAY:
				GetBufferDWord(&buffer,BUFFER_IPADDR,&AppConfig.MyGateway.Val);
				break;
			case ETH_PRIDNS:
				GetBufferDWord(&buffer,BUFFER_IPADDR,&AppConfig.PrimaryDNSServer.Val);
				break;
			case ETH_SECDNS:
				GetBufferDWord(&buffer,BUFFER_IPADDR,&AppConfig.SecondaryDNSServer.Val);
				break;
			case ETH_DEFIPADDR:
				GetBufferDWord(&buffer,BUFFER_IPADDR,&AppConfig.DefaultIPAddr.Val);
				break;
			case ETH_DEFNETMASK:
				GetBufferDWord(&buffer,BUFFER_IPADDR,&AppConfig.DefaultMask.Val);
				break;
			case ETH_NAME:
				GetBufferString(&buffer,AppConfig.NetBIOSName,sizeof(AppConfig.NetBIOSName));
				break;
			case ETH_DHCP:
				if (GetBufferBool(&buffer,&b)) AppConfig.Flags.bIsDHCPEnabled = b;
				break;
			case ETH_CONFIGMODE:
				if (GetBufferBool(&buffer,&b)) AppConfig.Flags.bInConfigMode = b;
				break;
			case ETH_MACADDR:
				if (buffer.type == BUFFER_BINARY && buffer.size == sizeof(MAC_ADDR)) {
					memcpy((void*)AppConfig.MyMACAddr.v,(void*)buffer.data,sizeof(MAC_ADDR));
				}
				break;
		}
	}

	DeleteBuffer(&buffer);
}

//===========================================================================
// USART device
//

static BYTE g_usart_mode = USART_MODE_DEBUG;

// Pin mappings for LINBus mode:
#define BIT_LIN_NSLP 	0
#define BIT_LIN_INH 	1
#define BIT_LIN_TXD		2
#define BIT_LIN_RXD		3

//===========================================================================
void ModuleInit_usart(void)
{
	BUFFER buffer;

	// Read the usart mode setting
	InitBuffer(&buffer);
	if (GetRegEntry(REG_VALUE,DEV_ADDRESS(LETTUCE_DEV_USART,0,USART_MODE),&buffer)) {
		GetBufferByte(&buffer,BUFFER_UINT,&g_usart_mode);
	}
	DeleteBuffer(&buffer);

	// Set port tristates for USARTs
	SetPortTristate(LETTUCE_DEV_USART,0b1010);
	
	// Set USART interrupts to low priority
	IPR1bits.TX1IP = 0;
	IPR1bits.RC1IP = 0;
	IPR3bits.TX2IP = 0;
	IPR3bits.RC2IP = 0;

	// Disable USART interrupts
	PIE1bits.RC1IE = 0;
	PIE1bits.TX1IE = 0;
	PIE3bits.RC2IE = 0;
	PIE3bits.TX2IE = 0;

	switch (g_usart_mode) {
		
		case USART_MODE_DEBUG:
			break;
			
		case USART_MODE_BRIDGE:
			UART2TCPBridgeInit();
			break;
			
		case USART_MODE_LINBUS:
			// Set MAX13021 into normal slope mode
			SetPortBit(LETTUCE_DEV_USART,BIT_LIN_TXD,1);
			SetPortBit(LETTUCE_DEV_USART,BIT_LIN_NSLP,1);
			Delay10us(2);
			
			// Configure USART2
			TXSTA2 = 0b00100100; // TXEN, BRGH
		  RCSTA2 = 0b10010000; // SPEN, CREN
		  BAUDCON2 = 0b00001000; // BRG16
		  SPBRG1 = 0x3C; // for 9601 baud -> SPBRG = 1084 = 0x043C
		  SPBRGH1 = 0x04;
			break;
	}	
}	

//===========================================================================
void ModuleTask_usart(void)
{
	if (g_usart_mode == USART_MODE_BRIDGE) {
		UART2TCPBridgeTask();
	} 		
}

//===========================================================================
BYTE ModuleRequest_usart(BYTE devloc, BYTE cmd, WORD addr, BUFFER* buffer)
{

	return LETTUCE_STATUS_BAD_ADDR;
}

//===========================================================================
void ModuleISR_usart()
{
	// EUSART1 receive
	if (PIR1bits.RC1IF || PIR1bits.TX1IF) {
	 if (g_usart_mode == USART_MODE_DEBUG) {
		 
		} else if (g_usart_mode == USART_MODE_BRIDGE) {
	 		UART2TCPBridgeISR();
		} 		
	}
	
	// EUSART2
	if (PIR3bits.RC2IF || PIR3bits.TX2IF) {
		if (g_usart_mode == USART_MODE_LINBUS) {
			//			
		}
		
		if (PIR3bits.RC2IF) PIR3bits.RC2IF = 0;
		if (PIR3bits.TX2IF) PIR3bits.TX2IF = 0;
	}	
}	
