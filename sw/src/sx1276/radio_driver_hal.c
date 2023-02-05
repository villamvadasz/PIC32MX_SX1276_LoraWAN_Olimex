/******************************************************************************
  @Company:
    Microchip Technology Inc.

  @File Name:
    radio_driver_hal.c

  @Summary:
    This is the Radio Driver HAL source file which contains LoRa-specific Radio Driver
    Hardware Abstract Layer

  @Description:
    This source file provides LoRa-specific implementations for Radio Driver HAL APIs.
    Copyright (c) 2013 - 2016 released Microchip Technology Inc.  All rights reserved.

    Microchip licenses to you the right to use, modify, copy and distribute
    Software only when embedded on a Microchip microcontroller or digital signal
    controller that is integrated into your product or third party product
    (pursuant to the sublicense terms in the accompanying license agreement).

    You should refer to the license agreement accompanying this Software for
    additional information regarding your rights and obligations.

    SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
    EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
    MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
    IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
    CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
    OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
    INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
    CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
    SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
    (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *************************************************************************
 *                           radio_driver_hal.c
 *
 * Radio Driver HAL source file
 *
 ******************************************************************************/

#include <xc.h>
#include "sx1276.h"
#include "c_sx1276.h"
#include "k_stdtype.h"
#include "radio_driver_hal.h"
#include "mcc_lora_config.h"

#ifdef SX1276_USE_SPI
	#include "spi.h"
	#include "c_spi.h"
#endif
#ifdef SX1276_USE_SPI2
	#include "spi2.h"
	#include "c_spi2.h"
	#define SPI_USER_SX1276 SPI2_USER_SX1276
	#define spi_calculate_BRG spi2_calculate_BRG
	#define spi_lock spi2_lock
	#define spi_unlock spi2_unlock
	#define spi_reconfigure spi2_reconfigure
	#define spi_readWrite_synch spi2_readWrite_synch
#endif
#ifdef SX1276_USE_SPI3
	#include "spi3.h"
	#include "c_spi3.h"
	#define SPI_USER_SX1276 SPI3_USER_SX1276
	#define spi_calculate_BRG spi3_calculate_BRG
	#define spi_lock spi3_lock
	#define spi_unlock spi3_unlock
	#define spi_reconfigure spi3_reconfigure
	#define spi_readWrite_synch spi3_readWrite_synch
#endif
#ifdef SX1276_USE_SPI4
	#include "spi4.h"
	#include "c_spi4.h"
	#define SPI_USER_SX1276 SPI4_USER_SX1276
	#define spi_calculate_BRG spi4_calculate_BRG
	#define spi_lock spi4_lock
	#define spi_unlock spi4_unlock
	#define spi_reconfigure spi4_reconfigure
	#define spi_readWrite_synch spi4_readWrite_synch
#endif
#ifdef SX1276_USE_SPI_SW
	#include "spi_sw.h"
	#include "c_spi_sw.h"
	#define SPI_USER_SX1276 SPI_SW_USER_SX1276
	#define spi_calculate_BRG spi_sw_calculate_BRG
	#define spi_lock spi_sw_lock
	#define spi_unlock spi_sw_unlock
	#define spi_reconfigure spi_sw_reconfigure
	#define spi_readWrite_synch spi_sw_readWrite_synch
#endif

unsigned char sx1278_bufferOut[8] = {0};
unsigned char sx1278_bufferIn[8] = {0};

void HALResetPinMakeOutput(void)
{
    SX1276_nReset_TRIS = 0;
}

void HALResetPinMakeInput(void)
{
    SX1276_nReset_TRIS = 1;
}

void HALResetPinOutputValue(uint8_t value)
{
    if (value == 0) {
        SX1276_nReset_LAT = 0;
	} else if (value == 1) {
        SX1276_nReset_LAT = 1;
	}
}

void HALSPICSAssert(void)
{
	spi_lock(SPI_USER_SX1276);
	spi_reconfigure(SPI_USER_SX1276, SX1276_SMP_CONFIG_DEFAULT, SX1276_CKP_CONFIG_DEFAULT, SX1276_CKE_CONFIG_DEFAULT, spi_calculate_BRG(SX1276_SPI_FREQUENCY));
	SX1276_nCS_LAT = 0;
}

void HALSPICSDeassert(void)
{
	SX1276_nCS_LAT = 1;
	spi_unlock(SPI_USER_SX1276);
}

uint8_t HALSPISend(uint8_t data)
{
	uint8_t result = 0;
	sx1278_bufferOut[0] = data;
	spi_readWrite_synch(SPI_USER_SX1276, sx1278_bufferOut, sx1278_bufferIn, 1);
	result = sx1278_bufferIn[0];
    return result;
}

/**
 End of File
*/