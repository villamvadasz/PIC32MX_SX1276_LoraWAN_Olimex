/******************************************************************************
  @Company:
    Microchip Technology Inc.

  @File Name:
    sw_timer.c

  @Summary:
    This is the SW Timer source file which provides SW Timer service to the LoRa

  @Description:
    This source file provides SW Timer service implementation.
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
 *                           sw_timer.c
 *
 * SW Timer source file
 *
 ******************************************************************************/

#include <stddef.h>
#include <xc.h>
#include "tmr.h"
#include "mal.h"
#include "sx1276.h"

#include "sw_timer.h"
#include "tmr_lora_addons.h"
#include "mcc_lora_config.h"
//#include "interrupt_manager.h"
//#include "mcc.h"

typedef struct
{
    uint32_t ticksRemaining;
    uint8_t running;
    uint8_t callbackParameter;
    void (*callback)(uint8_t);
} SwTimer_t;

volatile static SwTimer_t swTimers[MAX_TIMERS];
volatile static uint8_t allocatedTimers;
volatile static uint32_t ticksAccounted;


// This function needs to be called with interrupts stopped
uint32_t TMR_GetDeltaTime(void)
{
    uint32_t retVal = 0;
    static uint32 tmr_current = 0;
    static uint32 tmr_prev = 0;

    tmr_current = getGlobalTimeUs_Lora();

	if (tmr_current >= tmr_prev) {
		retVal = tmr_current - tmr_prev;				//0xFFFFFFFF - 0xFFFFFFFE = 1
	} else {
		retVal = 0xFFFFFFFF - tmr_prev + tmr_current + 1;	//0xFFFFFFFF - 0xFFFFFFFF + 0x00000000 -  = 1
	}
	tmr_prev = tmr_current;
    return retVal;
}

void SystemTimerInit(void)
{
    allocatedTimers = 0;
}

// This function should allow interrupts to happen (unless it was called from
// an interrupt itself) and keep its timing accurate. Ideally it should do the
// waiting with the MCU in sleep.
// Find out how long it takes the MCU to go to and wake up from sleep to see if
// it makes sense to go to sleep at all.
void SystemBlockingWaitMs(uint32_t ms)
{
	delayms(ms);
}

uint8_t SwTimerCreate(void)
{
    uint8_t retVal;
    if (allocatedTimers < MAX_TIMERS)
    {
        //lock_isr();
        retVal = allocatedTimers;
        swTimers[retVal].running = 0;
        swTimers[retVal].ticksRemaining = 0;
        swTimers[retVal].callbackParameter = 0;
        swTimers[retVal].callback = NULL;
        allocatedTimers++;
        //unlock_isr();
    }
    else
    {
		// If you reach this spot it means the MAX_TIMERS is #defined to a
		// lower value than the number of timers that have been
		// SwTimerCreate()
		sx1276_unexpected_probem();
    }
    return retVal;
}

void SwTimerSetCallback(uint8_t timerId, void (*callback)(uint8_t), uint8_t callbackParameter)
{
    swTimers[timerId].callback = callback;
    swTimers[timerId].callbackParameter = callbackParameter;
}

void SwTimerSetCallbackParam(uint8_t timerId, uint8_t newCallBackParameter)
{
    swTimers[timerId].callbackParameter = newCallBackParameter;
}

uint8_t SwTimerDecCallbackParam(uint8_t timerId)
{
    if( 0 != swTimers[timerId].callbackParameter)
    {
        swTimers[timerId].callbackParameter--;
    }
    
    return swTimers[timerId].callbackParameter;
}

void SwTimerSetTimeout(uint8_t timerId, uint32_t mstimeout)
{
    //lock_isr();
    swTimers[timerId].ticksRemaining = MS_TO_TICKS(mstimeout);
    //unlock_isr();
}

uint32_t SwTimerReadValue(uint8_t timerId)
{
    uint32_t ticksCount;

    //lock_isr();
    // Calling this function synchronizes all timers to the current moment, i.e.
    // updates the elapsed times on all timers
    SwTimersUpdate();
    ticksCount = swTimers[timerId].ticksRemaining;
    //unlock_isr();

    return ticksCount;
}

uint8_t SwTimerIsRunning(uint8_t timerId)
{
    uint8_t isRunning = 0;

    //lock_isr();
    isRunning = swTimers[timerId].running;
    //unlock_isr();

    return isRunning;
}

void SwTimerStart(uint8_t timerId)
{
    //lock_isr();
    swTimers[timerId].running = 1;
    //unlock_isr();
}

void SwTimerStop(uint8_t timerId)
{
    //lock_isr();
    swTimers[timerId].running = 0;
    //unlock_isr();
}

// This function needs to be called with interrupts disabled. Returns 1 if MCU
// may go to sleep and 0 if it may not.
uint8_t SwTimersCanSleep(void)
{
    uint8_t i;

    // Calling this function synchronizes all timers to the current moment, i.e.
    // updates the elapsed times on all timers
    SwTimersUpdate();

    for (i = 0; i < allocatedTimers; i++)
    {
        if ((swTimers[i].running == 1) && (0 == swTimers[i].ticksRemaining))
        {
            return 0;
        }
    }
    return 1;
}

void SwTimersExecute(void)
{
    uint8_t i;
	
	SwTimersUpdate();
    
    for (i = 0; i < allocatedTimers; i++)
    {
        // Subtract time elapsed from all running timers and if any hits 0
        // execute its callback (if not null)
        if (swTimers[i].running == 1)
        {
            if (0 == swTimers[i].ticksRemaining)
            {
                swTimers[i].running = 0;
                if (NULL != swTimers[i].callback)
                {
                    swTimers[i].callback(swTimers[i].callbackParameter);
                }
            }
        }
    }
}

// Function called from ISR AND mainline code
void SwTimersUpdate(void)
{
    uint8_t i;
    uint32_t timeElapsed;

    // Find out how much time has passed since the last execution of this
    // function.
    timeElapsed = TMR_GetDeltaTime();

    for (i = 0; i < allocatedTimers; i++)
    {
        // Subtract time elapsed from all running timers and if any hits 0
        // register its callback for execution (msRemaining = 0)
        if (swTimers[i].running == 1)
        {
            if (swTimers[i].ticksRemaining <= timeElapsed)
            {
                swTimers[i].ticksRemaining = 0;
            }
            else
            {
                swTimers[i].ticksRemaining -= timeElapsed;
            }
        }
    }

}

/**
 End of File
*/
