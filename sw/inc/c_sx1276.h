#ifndef _C_SX1276_H_
#define _C_SX1276_H_

	//Olimex board does not have DIO5 to poll for mode change finished
	#define RADIO_DRIVER_SX1276_MODECHANGE_USE_INTERRUPT
	
	//Measure RX message arrive delay For experiment and debug purpose
	//#define SX1276_WIDER_RX_WINDOW

	//#define SX1276_USE_SPI
	//#define SX1276_USE_SPI2
	//#define SX1276_USE_SPI3
	//#define SX1276_USE_SPI4
	#define SX1276_USE_SPI_SW
	
	//NNSXS.6254LPYSFFDGS3TCTE5JIYGP74HKHQBUGEISR4Y.SFQO4BCXZL7CDRQLBO3MWYRUFOXOKTELV3FJ7CU5KZYSTKOXRVIQ
	
	#define SX1276_SPI_FREQUENCY (10000000uL)
	#define SX1276_SMP_CONFIG_DEFAULT 0	//Data is sample at the middle
	#define SX1276_CKP_CONFIG_DEFAULT 0 //Idle state for clock is a low level; active state is a high level
	#define SX1276_CKE_CONFIG_DEFAULT 1	//Serial output data changes on transition from active clock state to Idle clock state. see CKP
	
	//PINGUINO
	//DIO0	3		TXD		RF5
	//DIO1	4		RXD		RF4
	//DIO2	6		SDA		RD9
	//DIO3	N/A
	//DIO4	N/A
	//DIO5	N/A
	//RES	5		SCL		RD10
	//NSS	10		UEXT_CS	F0
	
	#define SX1276_DIO0_TRIS	TRISFbits.TRISF5
	#define SX1276_DIO0_LAT		LATFbits.LATF5
	#define SX1276_DIO0_PORT	PORTFbits.RF5

	#define SX1276_DIO1_TRIS	TRISFbits.TRISF4
	#define SX1276_DIO1_LAT		LATFbits.LATF4
	#define SX1276_DIO1_PORT	PORTFbits.RF4
	
	#define SX1276_DIO2_TRIS	TRISDbits.TRISD9
	#define SX1276_DIO2_LAT		LATDbits.LATD9
	#define SX1276_DIO2_PORT	PORTDbits.RD9

	//#define SX1276_DIO3_TRIS	TRISDbits.TRISD0
	//#define SX1276_DIO3_LAT		LATDbits.LATD0
	//#define SX1276_DIO3_PORT	PORTDbits.RD0

	//#define SX1276_DIO4_TRIS	TRISDbits.TRISD0
	//#define SX1276_DIO4_LAT		LATDbits.LATD0
	//#define SX1276_DIO4_PORT	PORTDbits.RD0

	//#define SX1276_DIO5_TRIS	TRISDbits.TRISD0
	//#define SX1276_DIO5_LAT		LATDbits.LATD0
	//#define SX1276_DIO5_PORT	PORTDbits.RD0

	#define SX1276_nReset_TRIS		TRISDbits.TRISD10
	#define SX1276_nReset_LAT		LATDbits.LATD10

	#define SX1276_nCS_TRIS			TRISFbits.TRISF0
	#define SX1276_nCS_LAT			LATFbits.LATF0

#endif
