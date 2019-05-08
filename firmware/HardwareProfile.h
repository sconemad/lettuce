/* Lettuce (http://www.sconemad.com/lettuce)
 
Hardware profile

Copyright (c) 2007 Andrew Wedgbury <wedge@sconemad.com>

*/

#ifndef __HARDWARE_PROFILE_H
#define __HARDWARE_PROFILE_H

#define LETTUCE_V1
//#define LETTUCE_PROTO

//-------------------------------------
// System clock frequency and utils
//
#define CLOCK_FREQ						(41666667ul) // 41.67 MHz
#define GetSystemClock()			CLOCK_FREQ
#define GetInstructionClock()	(GetSystemClock()/4)
#define GetPeripheralClock()	GetInstructionClock()

//-------------------------------------
// Default baud rate for USART
//
#define BAUD_RATE     19200					// bps

//-------------------------------------
// BTNS - Push button inputs
//
#define BUTTON0_TRIS		(TRISGbits.TRISG6)
#define	BUTTON0_IO			(PORTGbits.RG6)
#define BUTTON1_TRIS		(TRISGbits.TRISG5)
#define	BUTTON1_IO			(PORTGbits.RG5)
#define BUTTON2_TRIS		(TRISFbits.TRISF7)
#define	BUTTON2_IO			(PORTFbits.RF7)
#define BUTTON3_TRIS		(TRISGbits.TRISG4)
#define	BUTTON3_IO			(PORTGbits.RG4)
#define BUTTON4_TRIS		(TRISGbits.TRISG7)
#define	BUTTON4_IO			(PORTGbits.RG7)
#define BUTTON5_TRIS		(TRISGbits.TRISG3)
#define	BUTTON5_IO			(PORTGbits.RG3)

//-------------------------------------
// AUX - Auxillary port
//
#define AUX_0_TRIS			(TRISAbits.TRISA3)
#define AUX_0_LAT				(LATAbits.LATA3)
#define AUX_0_PORT			(PORTAbits.RA3)
#define AUX_1_TRIS			(TRISAbits.TRISA4)
#define AUX_1_LAT				(LATAbits.LATA4)
#define AUX_1_PORT			(PORTAbits.RA4)
#define AUX_2_TRIS			(TRISCbits.TRISC1)
#define AUX_2_LAT				(LATCbits.LATC1)
#define AUX_2_PORT			(PORTCbits.RC1)
#define AUX_3_TRIS			(TRISCbits.TRISC0)
#define AUX_3_LAT				(LATCbits.LATC0)
#define AUX_3_PORT			(PORTCbits.RC0)
#define AUX_4_TRIS			(TRISCbits.TRISC2)
#define AUX_4_LAT				(LATCbits.LATC2)
#define AUX_4_PORT			(PORTCbits.RC2)
#define AUX_5_TRIS			(TRISBbits.TRISB5)
#define AUX_5_LAT				(LATBbits.LATB5)
#define AUX_5_PORT			(PORTBbits.RB5)

//-------------------------------------
// USART
//
#define USART_TX1_TRIS	(TRISCbits.TRISC6)
#define USART_TX1_LAT		(LATCbits.LATC6)
#define USART_TX1_PORT	(PORTCbits.RC6)
#define USART_RX1_TRIS	(TRISCbits.TRISC7)
#define USART_RX1_LAT		(LATCbits.LATC7)
#define USART_RX1_PORT	(PORTCbits.RC7)
#define USART_TX2_TRIS	(TRISGbits.TRISG1)
#define USART_TX2_LAT		(LATGbits.LATG1)
#define USART_TX2_PORT	(PORTGbits.RG1)
#define USART_RX2_TRIS	(TRISGbits.TRISG2)
#define USART_RX2_LAT		(LATGbits.LATG2)
#define USART_RX2_PORT	(PORTGbits.RG2)

//-------------------------------------
// SPI ports
//
#define SPI1_SCK_TRIS		(TRISCbits.TRISC3)
#define SPI1_SDI_TRIS		(TRISCbits.TRISC4)
#define SPI1_SDO_TRIS		(TRISCbits.TRISC5)
#define SPI1_SID_TRIS		(TRISAbits.TRISA2)
#define SPI2_SCK_TRIS		(TRISDbits.TRISD6)
#define SPI2_SDI_TRIS		(TRISDbits.TRISD5)
#define SPI2_SDO_TRIS		(TRISDbits.TRISD4)
#define SPI2_SS_TRIS		(TRISDbits.TRISD7)

//-------------------------------------
// SPI slave device selects
//
#define SPI_SS_EEPROM_TRIS	(TRISGbits.TRISG0)
#define SPI_SS_EEPROM_IO	(LATGbits.LATG0)
#define SPI_SS_SLOT1_TRIS	(TRISDbits.TRISD0)
#define SPI_SS_SLOT1_IO		(LATDbits.LATD0)
#define SPI_SS_SLOT2_TRIS	(TRISDbits.TRISD1)
#define SPI_SS_SLOT2_IO		(LATDbits.LATD1)
#define SPI_SS_SLOT3_TRIS	(TRISDbits.TRISD2)
#define SPI_SS_SLOT3_IO		(LATDbits.LATD2)
#define SPI_SS_SLOT4_TRIS	(TRISDbits.TRISD3)
#define SPI_SS_SLOT4_IO		(LATDbits.LATD3)
#define EEPROM_CS_TRIS 	SPI_SS_EEPROM_TRIS
#define EEPROM_CS_IO 		SPI_SS_EEPROM_IO
#define SPIRAM_CS_TRIS	1
// NOTE: Only 1 of these should be low at any time.

//-------------------------------------
// SPI slave device interrupts
//
#define SLOT1_INT_TRIS	(TRISBbits.TRISB0)
#define SLOT1_INT_PORT	(PORTBbits.RB0)
#define SLOT2_INT_TRIS	(TRISBbits.TRISB1)
#define SLOT2_INT_PORT	(PORTBbits.RB1)
#define SLOT3_INT_TRIS	(TRISBbits.TRISB2)
#define SLOT3_INT_PORT	(PORTBbits.RB2)
#define SLOT4_INT_TRIS	(TRISBbits.TRISB3)
#define SLOT4_INT_PORT	(PORTBbits.RB3)

//-------------------------------------
// DIG1
//
#define DIG1_TRIS				(*((volatile unsigned char*)(&TRISE)))
#define DIG1_PORT				(*((volatile unsigned char*)(&PORTE)))
#define DIG1_LAT				(*((volatile unsigned char*)(&LATE)))
#define DIG1_0_TRIS			(TRISEbits.TRISE0)
#define DIG1_0_LAT			(LATEbits.LATE0)
#define DIG1_0_PORT			(PORTEbits.RE0)
#define DIG1_1_TRIS			(TRISEbits.TRISE1)
#define DIG1_1_LAT			(LATEbits.LATE1)
#define DIG1_1_PORT			(PORTEbits.RE1)
#define DIG1_2_TRIS			(TRISEbits.TRISE2)
#define DIG1_2_LAT			(LATEbits.LATE2)
#define DIG1_2_PORT			(PORTEbits.RE2)
#define DIG1_3_TRIS			(TRISEbits.TRISE3)
#define DIG1_3_LAT			(LATEbits.LATE3)
#define DIG1_3_PORT			(PORTEbits.RE3)
#define DIG1_4_TRIS			(TRISEbits.TRISE4)
#define DIG1_4_LAT			(LATEbits.LATE4)
#define DIG1_4_PORT			(PORTEbits.RE4)
#define DIG1_5_TRIS			(TRISEbits.TRISE5)
#define DIG1_5_LAT			(LATEbits.LATE5)
#define DIG1_5_PORT			(PORTEbits.RE5)
#define DIG1_6_TRIS			(TRISEbits.TRISE6)
#define DIG1_6_LAT			(LATEbits.LATE6)
#define DIG1_6_PORT			(PORTEbits.RE6)
#define DIG1_7_TRIS			(TRISEbits.TRISE7)
#define DIG1_7_LAT			(LATEbits.LATE7)
#define DIG1_7_PORT			(PORTEbits.RE7)

//-------------------------------------
// DIG2
//
#define DIG2_TRIS				(*((volatile unsigned char*)(&TRISJ)))
#define DIG2_PORT				(*((volatile unsigned char*)(&PORTJ)))
#define DIG2_LAT				(*((volatile unsigned char*)(&LATJ)))
#define DIG2_0_TRIS			(TRISJbits.TRISJ0)
#define DIG2_0_LAT			(LATJbits.LATJ0)
#define DIG2_0_PORT			(PORTJbits.RJ0)
#define DIG2_1_TRIS			(TRISJbits.TRISJ1)
#define DIG2_1_LAT			(LATJbits.LATJ1)
#define DIG2_1_PORT			(PORTJbits.RJ1)
#define DIG2_2_TRIS			(TRISJbits.TRISJ2)
#define DIG2_2_LAT			(LATJbits.LATJ2)
#define DIG2_2_PORT			(PORTJbits.RJ2)
#define DIG2_3_TRIS			(TRISJbits.TRISJ3)
#define DIG2_3_LAT			(LATJbits.LATJ3)
#define DIG2_3_PORT			(PORTJbits.RJ3)
#define DIG2_4_TRIS			(TRISJbits.TRISJ4)
#define DIG2_4_LAT			(LATJbits.LATJ4)
#define DIG2_4_PORT			(PORTJbits.RJ4)
#define DIG2_5_TRIS			(TRISJbits.TRISJ5)
#define DIG2_5_LAT			(LATJbits.LATJ5)
#define DIG2_5_PORT			(PORTJbits.RJ5)
#define DIG2_6_TRIS			(TRISJbits.TRISJ6)
#define DIG2_6_LAT			(LATJbits.LATJ6)
#define DIG2_6_PORT			(PORTJbits.RJ6)
#define DIG2_7_TRIS			(TRISJbits.TRISJ7)
#define DIG2_7_LAT			(LATJbits.LATJ7)
#define DIG2_7_PORT			(PORTJbits.RJ7)

//-------------------------------------
// AD1
//
#define AD1_0_TRIS			(TRISHbits.TRISH0)
#define AD1_0_LAT				(LATHbits.LATH0)
#define AD1_0_PORT			(PORTHbits.RH0)
#define AD1_1_TRIS			(TRISHbits.TRISH1)
#define AD1_1_LAT				(LATHbits.LATH1)
#define AD1_1_PORT			(PORTHbits.RH1)
#define AD1_2_TRIS			(TRISAbits.TRISA5)
#define AD1_2_LAT				(LATAbits.LATA5)
#define AD1_2_PORT			(PORTAbits.RA5)
#define AD1_3_TRIS			(TRISFbits.TRISF0)
#define AD1_3_LAT				(LATFbits.LATF0)
#define AD1_3_PORT			(PORTFbits.RF0)
#define AD1_4_TRIS			(TRISFbits.TRISF1)
#define AD1_4_LAT				(LATFbits.LATF1)
#define AD1_4_PORT			(PORTFbits.RF1)
#define AD1_5_TRIS			(TRISFbits.TRISF2)
#define AD1_5_LAT				(LATFbits.LATF2)
#define AD1_5_PORT			(PORTFbits.RF2)
#define AD1_6_TRIS			(TRISFbits.TRISF3)
#define AD1_6_LAT				(LATFbits.LATF3)
#define AD1_6_PORT			(PORTFbits.RF3)
#define AD1_7_TRIS			(TRISFbits.TRISF4)
#define AD1_7_LAT				(LATFbits.LATF4)
#define AD1_7_PORT			(PORTFbits.RF4)

//-------------------------------------
// AD2
//
#define AD2_0_TRIS			(TRISHbits.TRISH2)
#define AD2_0_LAT				(LATHbits.LATH2)
#define AD2_0_PORT			(PORTHbits.RH2)
#define AD2_1_TRIS			(TRISHbits.TRISH3)
#define AD2_1_LAT				(LATHbits.LATH3)
#define AD2_1_PORT			(PORTHbits.RH3)
#define AD2_2_TRIS			(TRISFbits.TRISF5)
#define AD2_2_LAT				(LATFbits.LATF5)
#define AD2_2_PORT			(PORTFbits.RF5)
#define AD2_3_TRIS			(TRISFbits.TRISF6)
#define AD2_3_LAT				(LATFbits.LATF6)
#define AD2_3_PORT			(PORTFbits.RF6)
#define AD2_4_TRIS			(TRISHbits.TRISH4)
#define AD2_4_LAT				(LATHbits.LATH4)
#define AD2_4_PORT			(PORTHbits.RH4)
#define AD2_5_TRIS			(TRISHbits.TRISH5)
#define AD2_5_LAT				(LATHbits.LATH5)
#define AD2_5_PORT			(PORTHbits.RH5)
#define AD2_6_TRIS			(TRISHbits.TRISH6)
#define AD2_6_LAT				(LATHbits.LATH6)
#define AD2_6_PORT			(PORTHbits.RH6)
#define AD2_7_TRIS			(TRISHbits.TRISH7)
#define AD2_7_LAT				(LATHbits.LATH7)
#define AD2_7_PORT			(PORTHbits.RH7)

//-------------------------------------
// LEDs - all map to AUX 5
//
#define LED0_TRIS			AUX_5_TRIS
#define LED0_IO				AUX_5_LAT
#define LED1_TRIS			AUX_5_TRIS
#define LED1_IO				AUX_5_LAT
#define LED2_TRIS			AUX_5_TRIS
#define LED2_IO				AUX_5_LAT
#define LED3_TRIS			AUX_5_TRIS
#define LED3_IO				AUX_5_LAT
#define LED4_TRIS			AUX_5_TRIS
#define LED4_IO				AUX_5_LAT
#define LED5_TRIS			AUX_5_TRIS
#define LED5_IO				AUX_5_LAT
#define LED6_TRIS			AUX_5_TRIS
#define LED6_IO				AUX_5_LAT
#define LED7_TRIS			AUX_5_TRIS
#define LED7_IO				AUX_5_LAT

//-------------------------------------
// EEPROM I/O pins
//
#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
#define EEPROM_SPI_IF		(PIR1bits.SSPIF)
#define EEPROM_SSPBUF		(SSPBUF)
#define EEPROM_SPICON1		(SSP1CON1)
#define EEPROM_SPICON1bits	(SSP1CON1bits)
#define EEPROM_SPICON2		(SSP1CON2)
#define EEPROM_SPISTAT		(SSP1STAT)
#define EEPROM_SPISTATbits	(SSP1STATbits)

//-------------------------------------
// Misc stuff
//
#if defined(__18CXX) || defined(HI_TECH_C)
	#define BusyUART()				BusyUSART()
	#define CloseUART()				CloseUSART()
	#define ConfigIntUART(a)		ConfigIntUSART(a)
	#define DataRdyUART()			DataRdyUSART()
	#define OpenUART(a,b,c)			OpenUSART(a,b,c)
	#define ReadUART()				ReadUSART()
	#define WriteUART(a)			WriteUSART(a)
	#define getsUART(a,b,c)			getsUSART(b,a)
	#define putsUART(a)				putsUSART(a)
	#define getcUART()				ReadUSART()
	#define putcUART(a)				WriteUSART(a)
	#define putrsUART(a)			putrsUSART((far rom char*)a)
#endif


#endif
