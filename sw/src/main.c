#include "main.h"
#include "c_main.h"

#include "k_stdtype.h"
#include "mal.h"
#include "sleep.h"
#include "tmr.h"
#include "stackmeasure.h"
#include "tmr.h"
#include "bootloader_interface.h"

#include "c_app.h"
#include "c_task.h"

#include "ad.h"
#include "app.h"
#include "eep_manager.h"
#include "isr.h"
#include "serial_usb.h"
#include "usb.h"
#include "dee.h"
#include "wakeup.h"

#include "spi_sw.h"
#include "sx1276.h"

#ifndef MAKEFILE_NAME
	#error MAKEFILE_NAME not defined.
#endif

#define SW_YEAR 0x2019
#define SW_MONTH 0x11
#define SW_DAY 0x22
#define SW_TYPE TYPE_GRBL
#define SW_VERSION 0x0101

SoftwareIdentification softwareIdentification = {SW_YEAR, SW_MONTH, SW_DAY, SW_VERSION, SW_TYPE};
unsigned char VERSION_ID[] = "pic32_cnc_stepper";
const char VERSION_DATE[]  __attribute__ ((address(0x9D008100))) = __DATE__;
const char VERSION_TIME[]  __attribute__ ((address(0x9D008120))) = __TIME__;
const char VERSION_ID_FIX[]  __attribute__ ((address(0x9D008140))) = "pic32_cnc_stepper";
const char VERSION_MAKEFILE_NAME[]  __attribute__ ((address(0x9D008160))) = MAKEFILE_NAME;
const char FILE_CRC[]  __attribute__ ((address(0x9D008180))) = "0000CRC0";//Only the first 4 bytes are CRC, the text after it is just to help find it in the file.

volatile unsigned char do_loopCnt = 0;
unsigned long loopCntTemp = 0;
unsigned long loopCnt = 0;
unsigned int tick_count = 0;

unsigned long loopCntHistory[60];
unsigned long loopCntHistoryLast = 0;
unsigned int loopCntHistoryCnt = 0;


int main (void) {
	init_mal();
	init_wakeup();

	init_stackmeasure();

	init_ad();

	init_dee();
	init_eep_manager();

	init_tmr(); //this should be the first beceaus it clears registered timers.

	init_serial_usb();
	init_spi_sw();

	init_usb();
	init_app();
	init_isr();
	init_sx1276();

	do_loopCnt = 0;

	bootloader_interface_clearRequest();
	
	while (1) {
		if (do_loopCnt) {
			do_loopCnt = 0;
			loopCnt = loopCntTemp; //Risk is here that interrupt corrupts the value, but it is taken
			loopCntHistory[loopCntHistoryCnt] = loopCnt;
			loopCntHistoryLast = loopCnt;
			loopCntHistoryCnt++;
			loopCntHistoryCnt %= ((sizeof(loopCntHistory)) / (sizeof(*loopCntHistory)));
			loopCntTemp = 0;
			if (loopCnt < 1000) { //1run/ms
				/*while (1) {
					Nop();
					ERROR_LED = 1;
				}*/
			}
		}
		do_wakeup();
		do_stackmeasure();
		do_ad();
		do_eep_manager();
		do_app();
		do_tmr();
		do_usb();
		do_serial_usb();
		do_dee();
		do_sx1276();

		do_spi_sw();
		//idle_Request();
		loopCntTemp++;
	}
	deinit_spi_sw();
	return 0;
}

void isr_main_1ms(void) {
	static uint16 loopCntTmr = 0;

	tick_count++;

	loopCntTmr ++;
	if (loopCntTmr == 1000) {
		loopCntTmr = 0;
		do_loopCnt = 1;
	}

	isr_eep_manager_1ms();
	isr_usb_1ms();
	isr_ad_1ms();
	isr_app_1ms();
	isr_dee_1ms();
	isr_spi_sw_1ms();
	isr_sx1276_1ms();
}

void isr_main_100us(void) {
	isr_app_100us();
	isr_sx1276_100us();
}

void isr_main_custom(void) {
	isr_app_custom();
}

void isr_main_10us(void) {
}
