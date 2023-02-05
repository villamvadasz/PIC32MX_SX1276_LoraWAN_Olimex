#ifndef _C_ILI9341_H_
#define _C_ILI9341_H_

	#include "mal.h"
	#include "c_lcd.h"
	#include "c_spi_sw.h"
	
	//#define ILI9341_USE_SPI
	//#define ILI9341_USE_SPI2
	//#define ILI9341_USE_SPI3
	//#define ILI9341_USE_SPI4
	#define ILI9341_USE_SPI_SW
	

	#define ILI9341_SYNCH() MAL_SYNC()

	//Need to modify LCD. Short PIN 3 and PIN 7 on UEXT
	#define ILI9341_DC_TRIS	TRISFbits.TRISF5
	#define ILI9341_DC_LAT	LATFbits.LATF5
	#define ILI9341_DC_PORT	PORTFbits.RF5

	#define ILI9341_CS_TRIS	TRISEbits.TRISE0
	#define ILI9341_CS_LAT	LATEbits.LATE0
	#define ILI9341_CS_PORT	PORTEbits.RE0

	#define ILI9341_RESET_TRIS	TRISEbits.TRISE1
	#define ILI9341_RESET_LAT	LATEbits.LATE1
	#define ILI9341_RESET_PORT	PORTEbits.RE1

	#define ILI9341_LED_TRIS	TRISEbits.TRISE3
	#define ILI9341_LED_LAT		LATEbits.LATE3
	#define ILI9341_LED_PORT	PORTEbits.RE3


#endif
