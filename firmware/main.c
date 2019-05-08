/* Lettuce (http://www.sconemad.com/lettuce)
 
Main application

Copyright (c) 2007 Andrew Wedgbury <wedge@sconemad.com>

*/

// Set configuration fuses
#pragma config XINST=OFF, WDT=ON, WDTPS=2048, FOSC2=ON, FOSC=HSPLL, ETHLED=ON
// (Watchdog timeout period 8.192s)

#define THIS_IS_STACK_APPLICATION
#include "TCPIP Stack/TCPIP.h"
#include "lettuce.h"
#include "LettuceRegistry.h"
#include "LettuceSystem.h"
#include "LettuceCore.h"
#include "LettuceClient.h"
#include "LettuceServer.h"
#include "PluginController.h"
#include "sralloc.h"

// This is used by other stack elements.
// Main application must define this and initialize it with proper values.
APP_CONFIG AppConfig;
BYTE AN0String[8];
BYTE myDHCPBindCount = 0xFF;

#if !defined(STACK_USE_DHCP_CLIENT)
	#define DHCPBindCount	(1)
#endif

// Private helper functions.
static void InitializeBoard(void);
static void ProcessIO(void);
void Display(void);
static BOOL MACIsLinkedNow(void);

int nIntHigh = 0;
int nIntLow = 0;


//===========================================================================
// PIC18 Interrupt Service Routines
//
// NOTE: Several PICs, including the PIC18F4620 revision A3 have a RETFIE 
// FAST/MOVFF bug. The interruptlow keyword is used to work around the bug 
// when using C18
//

//===========================================================================
// Low priority ISR
//
#pragma interruptlow LowISR
void LowISR(void)
{
	++nIntLow;
	ModuleISR_usart();
	PluginControllerLowISR();
}

//===========================================================================
// High priority ISR
//
#pragma interrupt HighISR
void HighISR(void)
{
	++nIntHigh;
	TickUpdate();
	ModuleISR_pwm();
	PluginControllerHighISR();
	
	#if defined(ZG_CS_TRIS)
		zgEintISR();
	#endif // ZG_CS_TRIS
}

//===========================================================================
#pragma code lowVector=0x18
void LowVector(void){_asm goto LowISR _endasm}
#pragma code highVector=0x8
void HighVector(void){_asm goto HighISR _endasm}
#pragma code // Return to default code section


//===========================================================================
// Main application entry point.
//
void main(void)
{
	static BOOL ethup = FALSE;
	static int ethuptimes = 0;

  static TICK t = 0;
  static TICK t_sec = 0;
  static TICK t_min = 0;
  static TICK t_hour = 0;

  // Perform basic hardware initialization
	ClrWdt();
  InitializeBoard();

	// Switch on AUX LED and write a log entry to indicate the device
	// is performing initialisation. Note that the log entry will go
	// to USART1 as ETH is not started.
	LED0_IO = 1;
	Log(LETTUCE_DEV_NONE,0,(rom char*)"boot");

	// Initialize the SRAM dynamic memory manager.
	SRAMInitHeap();

	// Initialize SPIRAM
	SPIRAMInit();

	// Initialize time updates
	TickInit();
	INTCON2bits.TMR0IP = 1;		// Change TMR0 to high priority

	// If button 0 is held for >4s on startup, clear the registry
	if (BUTTON0_IO == 0) {
		TICK StartTime = TickGet();
		while (BUTTON0_IO == 0) {
			if (TickGet() - StartTime > 4*TICK_SECOND) {
				LED0_IO = 0;
				ClearRegistry();
				LED0_IO = 1;
				break;
			}
		}
	}

	// Init modules
	ModuleInit_eth(); // Sets up AppConfig
	ModuleInit_logger();
	ModuleInit_usart();
	ModuleInit_pwm();

	#if defined(STACK_USE_MPFS) || defined(STACK_USE_MPFS2)
  // Initialize Microchip File System module
  MPFSInit();
	#endif

	// Start the lettuce plugin controller and detect plugins
	PluginControllerInit();

	// Initialize core stack layers (MAC, ARP, TCP, UDP) and
	// application modules (HTTP, SNMP, etc.)
	StackInit();

	// Turn AUX LED off and write a log entry to indicate that the 
	// device is now ready to run.
	LED0_IO = 0;
	Log(LETTUCE_DEV_NONE,0,(rom char*)"ready");

	// Now that all items are initialized, begin the co-operative
	// multitasking loop.  This infinite loop will continuously 
	// execute all stack-related tasks, as well as your own
	// application's functions.  Custom functions should be added
	// at the end of this loop.
  // Note that this is a "co-operative mult-tasking" mechanism
  // where every task performs its tasks (whether all in one shot
  // or part of it) and returns so that other tasks can do their
  // job.
  // If a task needs very long time to do its job, it must be broken
  // down into smaller pieces so that other tasks can have CPU time.

  while(1) {

		ClrWdt();

		if (ethup) {
			// Ethernet is currently up
			if (!MACIsLinkedNow()) {
				// Gone down
				Log(LETTUCE_DEV_ETH,0,(rom char*)"ethdown");
				ethup = FALSE;
			}
		} else {
			// Ethernet is currently down
			if (MACIsLinkedNow()) {
				// Gone up
				ethup = TRUE;
				++ethuptimes;
				Log(LETTUCE_DEV_ETH,0,(rom char*)"ethup");
			}
		}

		// Get tick count
		t = TickGet();
    if (t - t_hour >= 3600*TICK_SECOND) {
			t_hour = t; // Tasks to perform every hour

			// SNTP update

		}
		if (t - t_min >= 60*TICK_SECOND) {
			t_min = t; // Tasks to perform every minute

			Log(LETTUCE_DEV_CLOCK,0,(rom char*)"tick");
		}
    if (t - t_sec >= TICK_SECOND) {
			t_sec = t; // Tasks to perform every second

//			Display();

     	// Blink status LED
			LED0_IO ^= 1;
    }

		// This task performs normal stack task including checking
		// for incoming packet, type of packet and calling
		// appropriate stack entity to process it.
		StackTask();

		// This tasks invokes each of the core stack application tasks
		StackApplications();

		// Add your application specific tasks here.
		ProcessIO();
		
		// Lettuce modules
		ModuleTask_logger();
		ModuleTask_usart();
		ModuleTask_pwm();
		ModuleTask_buttons();

		// Lettuce plugin controller
		PluginControllerTask();

		// Lettuce servers
		LettuceTCPServer();
		LettuceUDPServer();

		// Lettuce clients
		LettuceClient();
		LettuceStateClient();

    // If the DHCP lease has changed recently, write the new
		// IP address to the LCD display, UART, and Announce service
		if(DHCPBindCount != myDHCPBindCount) {
			myDHCPBindCount = DHCPBindCount;

			//DisplayIPValue(AppConfig.MyIPAddr);
			Log(LETTUCE_DEV_ETH,0,(rom char*)"ip");

			#if defined(STACK_USE_ANNOUNCE)
				AnnounceIP();
			#endif
		}

	} // END MAIN LOOP
}

extern int chunx;

//===========================================================================
void Display(void)
{
	char msg[20];
	DWORD epochsec;
	TM time;

	epochsec = SNTPGetUTCSeconds();
	gmtime(epochsec,&time);

	sprintf(msg,
					"\x0c%02d:%02d:%02d\r\n",
					time.hour,time.min,time.sec);
	putsUART(msg);

	sprintf(msg,
					"%d %d %d",
					chunx,nIntHigh,nIntLow);
	putsUART(msg);

	/*
	sprintf(msg,
					"%d.%2d.%2d.%2d",
					AppConfig.MyIPAddr.v[0],
					AppConfig.MyIPAddr.v[1],
					AppConfig.MyIPAddr.v[2],
					AppConfig.MyIPAddr.v[3]);
	putsUART(msg);
	*/
}

//===========================================================================
static void ProcessIO(void)
{
	// Take a reading from temperature sensor
	WORD v = GetPortAnalog(LETTUCE_DEV_AUX,0);
	uitoa(100.0 * ((v - 155.15151515) / 3.1030303),AN0String);
}


//===========================================================================
static void InitializeBoard(void)
{
	// Enable 4x/5x PLL, this takes the clock speed up to 41.67 MHz
  OSCTUNE = 0x40;

	// Ports are all set as input by default, so we just need to specify outputs

	// Unselect all SPI plugins first
	SPI_SS_EEPROM_IO = 1;
	SPI_SS_SLOT1_IO = 1;
	SPI_SS_SLOT2_IO = 1;
	SPI_SS_SLOT3_IO = 1;
	SPI_SS_SLOT4_IO = 1;

	// Tristate SPI selects
	SPI_SS_EEPROM_TRIS = 0;
	SPI_SS_SLOT1_TRIS = 0;
	SPI_SS_SLOT2_TRIS = 0;
	SPI_SS_SLOT3_TRIS = 0;
	SPI_SS_SLOT4_TRIS = 0;

  // Disable internal PORTB pull-ups
  INTCON2bits.RBPU = 1;

	// SPI Lines
	SPI1_SCK_TRIS	= 0;
	SPI1_SDO_TRIS	= 0;
	SPI2_SCK_TRIS	= 0;
	SPI2_SDO_TRIS	= 0;

	// AUX outputs
	AUX_2_TRIS = 0; // PWM1 output
	AUX_4_TRIS = 0; // PWM2 output
	AUX_5_TRIS = 0; // LED output

	// Configure USART
	USART_TX1_TRIS = 0;
	USART_RX1_TRIS = 1;
	USART_TX2_TRIS = 0;
	USART_RX2_TRIS = 1;
	/*
	TXSTA = 0x20;
  RCSTA = 0x90;

	// See if we can use the high baud rate setting
	#if ((GetPeripheralClock()+2*BAUD_RATE)/BAUD_RATE/4 - 1) <= 255
		SPBRG = (GetPeripheralClock()+2*BAUD_RATE)/BAUD_RATE/4 - 1;
		TXSTAbits.BRGH = 1;
	#else	// Use the low baud rate setting
		SPBRG = (GetPeripheralClock()+8*BAUD_RATE)/BAUD_RATE/16 - 1;
	#endif
	*/
	
	TXSTA1 = 0b00100100; // TXEN, BRGH
  RCSTA1 = 0b10010000; // SPEN, CREN
  BAUDCON1 = 0b00001000; // BRG16
//  SPBRG1 = 0x3C; // for 9601 baud -> SPBRG = 1084 = 0x043C
//  SPBRGH1 = 0x04;
  SPBRG1 = 0x1E; // for 19200 baud
  SPBRGH1 = 0x02;

	// Configure ADC
	ADCInit();
	
	// Configure and enable interrupts
	RCONbits.IPEN = 1;		// Enable interrupt priorities
	INTCONbits.GIEH = 1;	// Enable high priority interrupts
  INTCONbits.GIEL = 1;	// Enable low priority interrupts
}


//===========================================================================
BOOL MACIsLinkedNow(void)
{
	return ReadPHYReg(PHSTAT2).PHSTAT2bits.LSTAT;
}
