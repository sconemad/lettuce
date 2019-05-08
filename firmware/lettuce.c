/* Lettuce (http://www.sconemad.com/lettuce)
 
Lettuce-specific utils

Copyright (c) 2007 Andrew Wedgbury <wedge@sconemad.com>

*/

#include "lettuce.h"


//===========================================================================
// Select SPI slave device
//
void SelectPlugin(LETTUCE_SPI_DEV dev)
{
	Nop(); Nop(); // Delay ~160ns
	
	// Unselect all first
	SPI_SS_EEPROM_IO = 1;
	SPI_SS_SLOT1_IO = 1;
	SPI_SS_SLOT2_IO = 1;
	SPI_SS_SLOT3_IO = 1;
	SPI_SS_SLOT4_IO = 1;

	Nop(); Nop(); // Delay ~160ns

	// Select the desired device
	SPI_SS_EEPROM_IO = (dev!=LETTUCE_DEV_EEPROM);
	SPI_SS_SLOT1_IO = (dev!=LETTUCE_DEV_SPI_SLOT_1);
	SPI_SS_SLOT2_IO = (dev!=LETTUCE_DEV_SPI_SLOT_2);
	SPI_SS_SLOT3_IO = (dev!=LETTUCE_DEV_SPI_SLOT_3);
	SPI_SS_SLOT4_IO = (dev!=LETTUCE_DEV_SPI_SLOT_4);

	Nop(); Nop(); // Delay ~160ns
}

//===========================================================================
// Get device ID
//
WORD GetPluginID(LETTUCE_SPI_DEV dev)
{
	OpenSPI(0); // Make sure nothing else uses SPI while we read the SID

	SelectPlugin(dev); // Select the desired plugin

	DelayMs(1); // Wait for device to settle

	ADCON0 = 0x09; // ADON, Channel 2

	DelayMs(1); // Wait for capacitor to charge

	ADCON0bits.GO = 1; // Start conversion
	
	while(ADCON0bits.GO); // Wait until A/D conversion is done

	SelectPlugin(LETTUCE_DEV_NONE); // Deselect plugin

	CloseSPI(); // Finished using SPI
	
	// AD converter errata work around (ex: PIC18F87J10 A2)
	#if !defined(__18F452)
	PRODL = ADCON2;
	ADCON2bits.ADCS0 = 1;
	ADCON2bits.ADCS1 = 1;
	ADCON2 = PRODL;
	#endif
	
	return *((WORD*)(&ADRESL));
}

//===========================================================================
// Poll interrupt status
//
BOOL PollPluginInt(LETTUCE_SPI_DEV dev)
{
	switch (dev) {
		case LETTUCE_DEV_SPI_SLOT_1: return SLOT1_INT_PORT;
		case LETTUCE_DEV_SPI_SLOT_2: return SLOT2_INT_PORT;
		case LETTUCE_DEV_SPI_SLOT_3: return SLOT3_INT_PORT;
		case LETTUCE_DEV_SPI_SLOT_4: return SLOT4_INT_PORT;
	}
}

//===========================================================================
// Get interrupt flag
//
BOOL GetPluginInt(LETTUCE_SPI_DEV dev)
{
	switch (dev) {
		case LETTUCE_DEV_SPI_SLOT_1: return INTCONbits.INT0IE && INTCONbits.INT0IF;
		case LETTUCE_DEV_SPI_SLOT_2: return INTCON3bits.INT1IE && INTCON3bits.INT1IF;
		case LETTUCE_DEV_SPI_SLOT_3: return INTCON3bits.INT2IE && INTCON3bits.INT2IF;
		case LETTUCE_DEV_SPI_SLOT_4: return INTCON3bits.INT3IE && INTCON3bits.INT3IF;
	}
}

//===========================================================================
// Setup interrupt for plugin
//
void SetupPluginInt(LETTUCE_SPI_DEV dev, LETTUCE_INT_EDGE edge)
{
	switch (dev) {
		case LETTUCE_DEV_SPI_SLOT_1: 
			INTCON2bits.INTEDG0 = edge;
			break;
		case LETTUCE_DEV_SPI_SLOT_2:
			INTCON3bits.INT1IP = 1;
			INTCON2bits.INTEDG1 = edge;
			break;
		case LETTUCE_DEV_SPI_SLOT_3:
			INTCON3bits.INT2IP = 1;
			INTCON2bits.INTEDG2 = edge;
			break;
		case LETTUCE_DEV_SPI_SLOT_4:
			INTCON2bits.INT3IP = 1;
			INTCON2bits.INTEDG3 = edge;
			break;
	}
}

//===========================================================================
// Enable/disable interrupt for plugin
//
void SetPluginInt(LETTUCE_SPI_DEV dev, BOOL active)
{
	switch (dev) {
		case LETTUCE_DEV_SPI_SLOT_1: 
			INTCONbits.INT0IE = active;
			break;
		case LETTUCE_DEV_SPI_SLOT_2:
			INTCON3bits.INT1IE = active;
			break;
		case LETTUCE_DEV_SPI_SLOT_3:
			INTCON3bits.INT2IE = active;
			break;
		case LETTUCE_DEV_SPI_SLOT_4:
			INTCON3bits.INT3IE = active;
			break;
	}
}

//===========================================================================
// Clear plugin interrupt flag
//
void ClearPluginInt(LETTUCE_SPI_DEV dev)
{
	switch (dev) {
		case LETTUCE_DEV_SPI_SLOT_1: 
			INTCONbits.INT0IF = 0;
			break;
		case LETTUCE_DEV_SPI_SLOT_2:
			INTCON3bits.INT1IF = 0;
			break;
		case LETTUCE_DEV_SPI_SLOT_3:
			INTCON3bits.INT2IF = 0;
			break;
		case LETTUCE_DEV_SPI_SLOT_4:
			INTCON3bits.INT3IF = 0;
			break;
	}
}

//===========================================================================
// Set tristate on port
//
void SetPortTristate(LETTUCE_PORT port, BYTE val)
{
	switch (port) {
			// For AD and AUX ports we need to set individual bits
		case LETTUCE_DEV_PORT_AD1:
      AD1_0_TRIS = (0 != (val & 1));
			AD1_1_TRIS = (0 != (val & 2));
			AD1_2_TRIS = (0 != (val & 4));
			AD1_3_TRIS = (0 != (val & 8));
			AD1_4_TRIS = (0 != (val & 16));
			AD1_5_TRIS = (0 != (val & 32));
			AD1_6_TRIS = (0 != (val & 64));
			AD1_7_TRIS = (0 != (val & 128));
			break;

		case LETTUCE_DEV_PORT_AD2:
      AD2_0_TRIS = (0 != (val & 1));
			AD2_1_TRIS = (0 != (val & 2));
			AD2_2_TRIS = (0 != (val & 4));
			AD2_3_TRIS = (0 != (val & 8));
			AD2_4_TRIS = (0 != (val & 16));
			AD2_5_TRIS = (0 != (val & 32));
			AD2_6_TRIS = (0 != (val & 64));
			AD2_7_TRIS = (0 != (val & 128));
			break;

			// For DIG ports, we can set the whole byte at once
		case LETTUCE_DEV_PORT_DIG1:
			DIG1_TRIS = val;
			break;

		case LETTUCE_DEV_PORT_DIG2:
			DIG2_TRIS = val;
			break;

		case LETTUCE_DEV_AUX:
      AUX_0_TRIS = (0 != (val & 1));
			AUX_1_TRIS = (0 != (val & 2));
			AUX_2_TRIS = (0 != (val & 4));
			AUX_3_TRIS = (0 != (val & 8));
			AUX_4_TRIS = (0 != (val & 16));
			AUX_5_TRIS = (0 != (val & 32));
			break;

		case LETTUCE_DEV_USART:
      USART_TX1_TRIS = (0 != (val & 1));
			USART_RX1_TRIS = (0 != (val & 2));
      USART_TX2_TRIS = (0 != (val & 4));
			USART_RX2_TRIS = (0 != (val & 8));
			break;
	}
}

//===========================================================================
// Get tristate on port
//
BYTE GetPortTristate(LETTUCE_PORT port)
{
	switch (port) {
			// For AD ports we need to get individual bits
		case LETTUCE_DEV_PORT_AD1:
			return (AD1_0_TRIS * 1 +
							AD1_1_TRIS * 2 + 
							AD1_2_TRIS * 4 + 
							AD1_3_TRIS * 8 + 
							AD1_4_TRIS * 16 + 
							AD1_5_TRIS * 32 + 
							AD1_6_TRIS * 64 + 
							AD1_7_TRIS * 128);

		case LETTUCE_DEV_PORT_AD2:
			return (AD2_0_TRIS * 1 +
							AD2_1_TRIS * 2 + 
							AD2_2_TRIS * 4 + 
							AD2_3_TRIS * 8 + 
							AD2_4_TRIS * 16 + 
							AD2_5_TRIS * 32 + 
							AD2_6_TRIS * 64 + 
							AD2_7_TRIS * 128);

			// For DIG ports, we can get the whole byte at once
		case LETTUCE_DEV_PORT_DIG1:
			return DIG1_TRIS;

		case LETTUCE_DEV_PORT_DIG2:
			return DIG2_TRIS;

		case LETTUCE_DEV_AUX:
			return (AUX_0_TRIS * 1 +
							AUX_1_TRIS * 2 + 
							AUX_2_TRIS * 4 + 
							AUX_3_TRIS * 8 + 
							AUX_4_TRIS * 16 + 
							AUX_5_TRIS * 32);

		case LETTUCE_DEV_USART:
			return (USART_TX1_TRIS * 1 +
							USART_RX1_TRIS * 2 +
							USART_TX2_TRIS * 4 +
							USART_RX2_TRIS * 8);
	}
	return 0;
}

//===========================================================================
// Set whole byte on port
//
void SetPortByte(LETTUCE_PORT port, BYTE val)
{
	switch (port) {
			// For AD ports we need to set individual bits
		case LETTUCE_DEV_PORT_AD1:
      AD1_0_LAT = (0 != (val & 1));
			AD1_1_LAT = (0 != (val & 2));
			AD1_2_LAT = (0 != (val & 4));
			AD1_3_LAT = (0 != (val & 8));
			AD1_4_LAT = (0 != (val & 16));
			AD1_5_LAT = (0 != (val & 32));
			AD1_6_LAT = (0 != (val & 64));
			AD1_7_LAT = (0 != (val & 128));
			break;

		case LETTUCE_DEV_PORT_AD2:
      AD2_0_LAT = (0 != (val & 1));
			AD2_1_LAT = (0 != (val & 2));
			AD2_2_LAT = (0 != (val & 4));
			AD2_3_LAT = (0 != (val & 8));
			AD2_4_LAT = (0 != (val & 16));
			AD2_5_LAT = (0 != (val & 32));
			AD2_6_LAT = (0 != (val & 64));
			AD2_7_LAT = (0 != (val & 128));
			break;

			// For DIG ports, we can set the whole byte at once
		case LETTUCE_DEV_PORT_DIG1:
			DIG1_LAT = val;
			break;

		case LETTUCE_DEV_PORT_DIG2:
			DIG2_LAT = val;
			break;

		case LETTUCE_DEV_AUX:
      AUX_0_LAT = (0 != (val & 1));
			AUX_1_LAT = (0 != (val & 2));
			AUX_2_LAT = (0 != (val & 4));
			AUX_3_LAT = (0 != (val & 8));
			AUX_4_LAT = (0 != (val & 16));
			AUX_5_LAT = (0 != (val & 32));
			break;

		case LETTUCE_DEV_USART:
      USART_TX1_LAT = (0 != (val & 1));
      USART_RX1_LAT = (0 != (val & 2));
      USART_TX2_LAT = (0 != (val & 4));
      USART_RX2_LAT = (0 != (val & 8));
			break;
	}
}

//===========================================================================
// Get whole byte from port
//
BYTE GetPortByte(LETTUCE_PORT port)
{
	switch (port) {
			// For AD ports we need to get individual bits
		case LETTUCE_DEV_PORT_AD1:
			return (AD1_0_PORT * 1 +
							AD1_1_PORT * 2 + 
							AD1_2_PORT * 4 + 
							AD1_3_PORT * 8 + 
							AD1_4_PORT * 16 + 
							AD1_5_PORT * 32 + 
							AD1_6_PORT * 64 + 
							AD1_7_PORT * 128);

		case LETTUCE_DEV_PORT_AD2:
			return (AD2_0_PORT * 1 +
							AD2_1_PORT * 2 + 
							AD2_2_PORT * 4 + 
							AD2_3_PORT * 8 + 
							AD2_4_PORT * 16 + 
							AD2_5_PORT * 32 + 
							AD2_6_PORT * 64 + 
							AD2_7_PORT * 128);

			// For DIG ports, we can get the whole byte at once
		case LETTUCE_DEV_PORT_DIG1:
			return DIG1_PORT;

		case LETTUCE_DEV_PORT_DIG2:
			return DIG2_PORT;

		case LETTUCE_DEV_AUX:
			return (AUX_0_PORT * 1 +
							AUX_1_PORT * 2 + 
							AUX_2_PORT * 4 + 
							AUX_3_PORT * 8 + 
							AUX_4_PORT * 16 + 
							AUX_5_PORT * 32);

		case LETTUCE_DEV_USART:
			return (USART_TX1_PORT * 1 +
							USART_RX1_PORT * 2 +
							USART_TX2_PORT * 4 +
							USART_RX2_PORT * 8);
	}
	return 0;
}

//===========================================================================
// Set individual bit on port
//
void SetPortBit(LETTUCE_PORT port, BYTE bitnum, BOOL value)
{
	switch (port) {
		case LETTUCE_DEV_PORT_AD1:
			switch (bitnum) {
				case 0: AD1_0_LAT = value; break;
				case 1: AD1_1_LAT = value; break;
				case 2: AD1_2_LAT = value; break;
				case 3: AD1_3_LAT = value; break;
				case 4: AD1_4_LAT = value; break;
				case 5: AD1_5_LAT = value; break;
				case 6: AD1_6_LAT = value; break;
				case 7: AD1_7_LAT = value; break;
			}
			break;

		case LETTUCE_DEV_PORT_AD2:
			switch (bitnum) {
				case 0: AD2_0_LAT = value; break;
				case 1: AD2_1_LAT = value; break;
				case 2: AD2_2_LAT = value; break;
				case 3: AD2_3_LAT = value; break;
				case 4: AD2_4_LAT = value; break;
				case 5: AD2_5_LAT = value; break;
				case 6: AD2_6_LAT = value; break;
				case 7: AD2_7_LAT = value; break;
			}
			break;

		case LETTUCE_DEV_PORT_DIG1:
			switch (bitnum) {
				case 0: DIG1_0_LAT = value; break;
				case 1: DIG1_1_LAT = value; break;
				case 2: DIG1_2_LAT = value; break;
				case 3: DIG1_3_LAT = value; break;
				case 4: DIG1_4_LAT = value; break;
				case 5: DIG1_5_LAT = value; break;
				case 6: DIG1_6_LAT = value; break;
				case 7: DIG1_7_LAT = value; break;
			}
			break;

		case LETTUCE_DEV_PORT_DIG2:
			switch (bitnum) {
				case 0: DIG2_0_LAT = value; break;
				case 1: DIG2_1_LAT = value; break;
				case 2: DIG2_2_LAT = value; break;
				case 3: DIG2_3_LAT = value; break;
				case 4: DIG2_4_LAT = value; break;
				case 5: DIG2_5_LAT = value; break;
				case 6: DIG2_6_LAT = value; break;
				case 7: DIG2_7_LAT = value; break;
			}
			break;

		case LETTUCE_DEV_AUX:
			switch (bitnum) {
				case 0: AUX_0_LAT = value; break;
				case 1: AUX_1_LAT = value; break;
				case 2: AUX_2_LAT = value; break;
				case 3: AUX_3_LAT = value; break;
				case 4: AUX_4_LAT = value; break;
				case 5: AUX_5_LAT = value; break;
			}
			break;

		case LETTUCE_DEV_USART:
			switch (bitnum) {
				case 0: USART_TX1_LAT = value; break;
				case 1: USART_RX1_LAT = value; break;
				case 2: USART_TX2_LAT = value; break;
				case 3: USART_RX2_LAT = value; break;
			}
			break;
	}
}

//===========================================================================
// Get individual bit from port
//
BOOL GetPortBit(LETTUCE_PORT port, BYTE bitnum)
{
	switch (port) {
		case LETTUCE_DEV_PORT_AD1:
			switch (bitnum) {
				case 0: return AD1_0_LAT;
				case 1: return AD1_1_LAT;
				case 2: return AD1_2_LAT;
				case 3: return AD1_3_LAT;
				case 4: return AD1_4_LAT;
				case 5: return AD1_5_LAT;
				case 6: return AD1_6_LAT;
				case 7: return AD1_7_LAT;
			}
			break;

		case LETTUCE_DEV_PORT_AD2:
			switch (bitnum) {
				case 0: return AD2_0_LAT;
				case 1: return AD2_1_LAT;
				case 2: return AD2_2_LAT;
				case 3: return AD2_3_LAT;
				case 4: return AD2_4_LAT;
				case 5: return AD2_5_LAT;
				case 6: return AD2_6_LAT;
				case 7: return AD2_7_LAT;
			}
			break;

		case LETTUCE_DEV_PORT_DIG1:
			switch (bitnum) {
				case 0: return DIG1_0_LAT;
				case 1: return DIG1_1_LAT;
				case 2: return DIG1_2_LAT;
				case 3: return DIG1_3_LAT;
				case 4: return DIG1_4_LAT;
				case 5: return DIG1_5_LAT;
				case 6: return DIG1_6_LAT;
				case 7: return DIG1_7_LAT;
			}
			break;

		case LETTUCE_DEV_PORT_DIG2:
			switch (bitnum) {
				case 0: return DIG2_0_LAT;
				case 1: return DIG2_1_LAT;
				case 2: return DIG2_2_LAT;
				case 3: return DIG2_3_LAT;
				case 4: return DIG2_4_LAT;
				case 5: return DIG2_5_LAT;
				case 6: return DIG2_6_LAT;
				case 7: return DIG2_7_LAT;
			}
			break;

		case LETTUCE_DEV_AUX:
			switch (bitnum) {
				case 0: return AUX_0_LAT;
				case 1: return AUX_1_LAT;
				case 2: return AUX_2_LAT;
				case 3: return AUX_3_LAT;
				case 4: return AUX_4_LAT;
				case 5: return AUX_5_LAT;
			}
			break;

		case LETTUCE_DEV_USART:
			switch (bitnum) {
				case 0: return USART_TX1_LAT;
				case 1: return USART_RX1_LAT;
				case 2: return USART_TX2_LAT;
				case 3: return USART_RX2_LAT;
			}
			break;
	}

	return 0;	
}

//-------------------------------------
// Init ADC and calibrate
//
void ADCInit(void)
{
	ADCON0 = 0x09; // ADON, Channel 2
	ADCON1 = 0x0B; // Vdd/Vss is +/-REF, AN0, AN1, AN2, AN3 are analog
	ADCON2 = 0xBE;	// Right justify, 20TAD ACQ time, Fosc/64 (~21.0kHz)

  // Do a calibration A/D conversion
	ADCON0bits.ADCAL = 1;
  ADCON0bits.GO = 1;
	while (ADCON0bits.GO) {;}
	ADCON0bits.ADCAL = 0;
}	

//-------------------------------------
// Set the ADC mode
//
void SetADCMode(BYTE mode)
{
	switch (mode) {
		case ADC_MODE_AUX: // AN0-AN3
			ADCON1 = 0b00001011;
			break;

		case ADC_MODE_AUX_AD1: // AN0-AN9
			ADCON1 = 0b00000101;
			break;

		case ADC_MODE_AUX_AD12: // AN0-AN15
			ADCON1 = 0b00000000;
			break;
	}		
}	

//===========================================================================
// Get analog reading from port
//
WORD GetPortAnalog(LETTUCE_PORT port, BYTE input)
{
	switch (port) {
		case LETTUCE_DEV_PORT_AD1:
			if (input > 5) return 0;
			ADCON0 = 0x01 + ((input + 0x04) << 2);
			break;

		case LETTUCE_DEV_PORT_AD2:
			if (input > 5) return 0;
			ADCON0 = 0x01 + ((input + 0x0A) << 2);
			break;

		case LETTUCE_DEV_AUX:
			if (input != 0) return 0;
			ADCON0 = 0x0D;
			break;

		default: return 0;
	}

	DelayMs(1);

	// Start conversion
	ADCON0bits.GO = 1;
	
	// Wait until A/D conversion is done
	while(ADCON0bits.GO);

	// AD converter errata work around (ex: PIC18F87J10 A2)
	#if !defined(__18F452)
	PRODL = ADCON2;
	ADCON2bits.ADCS0 = 1;
	ADCON2bits.ADCS1 = 1;
	ADCON2 = PRODL;
	#endif
	
	return *((WORD*)(&ADRESL));
}

volatile short int spi1_in_use = 0;

//===========================================================================
// Open SPI
//
BOOL OpenSPI(BYTE mode)
{
	++spi1_in_use;

	if (spi1_in_use > 1) {
		--spi1_in_use;
		return FALSE;
	}

	// BIT:    7    6    5    4    3    2    1    0
	// VALUE:  0x80 0x40 0x20 0x10 0x08 0x04 0x02 0x01 
	// MODE:            |SMP |bus mode |  sync mode   |
	// MASK:             0x20   0x18         0x07

	switch (mode & 0x07) {
		case SPI_FOSC_4:
			SSP1CON1bits.SSPM0 = 0;
			SSP1CON1bits.SSPM1 = 0;
			SSP1CON1bits.SSPM2 = 0;
			SSP1CON1bits.SSPM3 = 0;
			break;
		case SPI_FOSC_16:
			SSP1CON1bits.SSPM0 = 1;
			SSP1CON1bits.SSPM1 = 0;
			SSP1CON1bits.SSPM2 = 0;
			SSP1CON1bits.SSPM3 = 0;
			break;
		case SPI_FOSC_64:
			SSP1CON1bits.SSPM0 = 0;
			SSP1CON1bits.SSPM1 = 1;
			SSP1CON1bits.SSPM2 = 0;
			SSP1CON1bits.SSPM3 = 0;
			break;
	}

	switch (mode & 0x18) {
		case BUS_MODE_00:
			SSP1CON1bits.CKP = 0;       // clock idle state low
			SSP1STATbits.CKE = 1;       // data transmitted on rising edge
			break;

		case BUS_MODE_01:
			SSP1CON1bits.CKP = 0;       // clock idle state low
			SSP1STATbits.CKE = 0;       // data transmitted on falling edge
			break;

		case BUS_MODE_10:
			SSP1CON1bits.CKP = 1;       // clock idle state high
			SSP1STATbits.CKE = 1;       // data transmitted on falling edge
			break;

		case BUS_MODE_11:
			SSP1CON1bits.CKP = 1;       // clock idle state high
			SSP1STATbits.CKE = 0;       // data transmitted on rising edge
			break;
	}

	if (mode & 0x20) {
		SSP1STATbits.SMP = 1;
	} else {
		SSP1STATbits.SMP = 0;
	}

	SSP1CON1bits.SSPEN = 1;					// Enable SSP

	return TRUE;
}

//===========================================================================
// Write to SPI
//
BYTE WriteSPI(BYTE output)
{
	PIR1bits.SSPIF = 0;
	SSPBUF = output;
	while(!PIR1bits.SSPIF);
	PIR1bits.SSP1IF = 0;
	return SSPBUF;
}

//===========================================================================
// Close SPI
//
void CloseSPI(void)
{
	--spi1_in_use;
}

//===========================================================================
// Macros and data used by time routines below
//
#define LEAPYEAR(year) 	(!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year) 	(LEAPYEAR(year) ? 366 : 365)
#define TIME_MAX        2147483647L
const ROM BYTE _ytab[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

//===========================================================================
// Convert epoch_seconds to human readable date and time values
//
void gmtime(const DWORD epoch_seconds, TM* time)
{
  unsigned long dayclock, dayno;
  int year = 1970;

  dayclock = (unsigned long) epoch_seconds % 86400;
  dayno = (unsigned long) epoch_seconds / 86400;

  time->sec = dayclock % 60;
  time->min = (dayclock % 3600) / 60;
  time->hour = dayclock / 3600;
  time->wday = (dayno + 4) % 7; // Day 0 was a thursday

  while (dayno >= (unsigned long) YEARSIZE(year)) 
  {
    dayno -= YEARSIZE(year);
    year++;
  }
  time->year = year; //- 1900; why?
  time->mon = 0;
  while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][time->mon]) 
  {
		dayno -= _ytab[LEAPYEAR(year)][time->mon];
    time->mon++;
  }
  time->mday = dayno + 1;
}

//===========================================================================
// Convert human readable date and time values to epoch_seconds
//
DWORD mktime(const TM* time)
{
  long day, year;
  int tm_year;
  int yday, month;
  long seconds;
  int overflow;

  time->min += time->sec / 60;
  time->sec %= 60;
  if (time->sec < 0) 
  {
    time->sec += 60;
    time->min--;
  }
  time->hour += time->min / 60;
  time->min = time->min % 60;
  if (time->min < 0) 
  {
    time->min += 60;
    time->hour--;
  }
  day = time->hour / 24;
  time->hour= time->hour % 24;
  if (time->hour < 0) 
  {
    time->hour += 24;
    day--;
  }
  time->year += time->mon / 12;
  time->mon %= 12;
  if (time->mon < 0) 
  {
    time->mon += 12;
    time->year--;
  }
  day += (time->mday - 1);
  while (day < 0) 
  {
    if(--time->mon < 0) 
    {
      time->year--;
      time->mon = 11;
    }
    day += _ytab[LEAPYEAR(1900 + time->year)][time->mon];
  }
  while (day >= _ytab[LEAPYEAR(1900 + time->year)][time->mon]) 
  {
    day -= _ytab[LEAPYEAR(1900 + time->year)][time->mon];
    if (++(time->mon) == 12) 
    {
      time->mon = 0;
      time->year++;
    }
  }
  time->mday = day + 1;
  year = 1970;
  if (time->year < year - 1900) return 0;
  seconds = 0;
  day = 0;                      // Means days since day 0 now
  overflow = 0;

  // Assume that when day becomes negative, there will certainly
  // be overflow on seconds.
  // The check for overflow needs not to be done for leapyears
  // divisible by 400.
  // The code only works when year (1970) is not a leapyear.
  tm_year = time->year + 1900;

  if (TIME_MAX / 365 < tm_year - year) overflow++;
  day = (tm_year - year) * 365;
  if (TIME_MAX - day < (tm_year - year) / 4 + 1) overflow++;
  day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
  day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
  day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

  yday = month = 0;
  while (month < time->mon)
  {
    yday += _ytab[LEAPYEAR(tm_year)][month];
    month++;
  }
  yday += (time->mday - 1);
  if (day + yday < 0) overflow++;
  day += yday;

  time->wday = (day + 4) % 7;               // Day 0 was thursday (4)

  seconds = ((time->hour * 60L) + time->min) * 60L + time->sec;

  if ((TIME_MAX - seconds) / 86400 < day) overflow++;
  seconds += day * 86400;

  if (overflow) return 0;
  return (DWORD)seconds;
}
