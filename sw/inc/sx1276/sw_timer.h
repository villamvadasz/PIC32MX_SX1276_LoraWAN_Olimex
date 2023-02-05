/******************************************************************************
  @Company:
    Microchip Technology Inc.

  @File Name:
    sw_timer.h

  @Summary:
    This is the SW Timer header file which provides the SW Timer services

  @Description:
    This header file provides SW Timer services.
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
 *                           sw_timer.h
 *
 * SW Timer header file
 *
 ******************************************************************************/

#ifndef SW_TIMER_H
#define	SW_TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "lorawan_defs.h"

#define MAX_TIMERS                  20

//1 Tick is 1ms
#define MS_TO_TICKS(n)                                  ((n * 1000))
#define TICKS_TO_MS(n)                                  ((n / 1000))
    
#define HW_MAX_TIMER_VAL                                ((uint32_t)0x10000)

void SystemBlockingWaitMs(uint32_t ms);
void SystemTimerInit(void);

uint8_t SwTimerCreate(void);
void SwTimerSetCallback(uint8_t timerId, void (*callback)(uint8_t), uint8_t callbackParameter);
void SwTimerSetCallbackParam(uint8_t timerId, uint8_t newCallBackParameter);
uint8_t SwTimerDecCallbackParam(uint8_t timerId);
void SwTimerSetTimeout(uint8_t timerId, uint32_t msTimeout);
uint32_t SwTimerReadValue(uint8_t timerId);
uint8_t SwTimerIsRunning(uint8_t timerId);
void SwTimerStart(uint8_t timerId);
void SwTimerStop(uint8_t timerId);
void SwTimersExecute(void);
uint8_t SwTimersCanSleep(void);
void SwTimersUpdate(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SW_TIMER_H */

/**
 End of File
*/