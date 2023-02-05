/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*
 *************************************************************************
 *                           parser_system.c
 *
 * This file contains all the commands used for testing the system peripherals
 *
 *
 * Hardware:
 *  USB Carrier with RN2483 module
 *
 * Author            Date            Ver     Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * M15723          2014.07.29        0.5
 * M18029		   2016.09.30	
 ******************************************************************************/

#include <xc.h>
#include <stdlib.h>
#include "parser_private.h"
#include "parser.h"
#include "parser_utils.h"
#include "parser_tsp.h"
#include "bootloader_interface.h"
#include "mal.h"
#include "sx1276.h"

static const char* gapParserSysStatus[] =
{
    "ok",
    "invalid_param",
    "err"
};

static const char* gapParserSysPinMode[] =
{
    "digout",
    "digin",
    "ana",
    "sfun"
};

enum
{
    PM_DIGOUT = 0x00,
    PM_DIGIN,
    PM_ANAIN,
    PM_SFUN
};

static const uint8_t gaAvailablePinFunctions[] = {
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN) | (1 << PM_ANAIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN),
    (1 << PM_DIGOUT) | (1 << PM_DIGIN)
};


void Parser_ExitFromSleep(void)
{
    Parser_RxClearBuffer();
    Parser_TxAddReply((char*)gapParserSysStatus[OK_STATUS_IDX], strlen(gapParserSysStatus[OK_STATUS_IDX]));
}

void Parser_SystemSleep(parserCmdInfo_t* pParserCmdInfo)
{
    uint32_t param1Value = (uint32_t)strtoul(pParserCmdInfo->pParam1, NULL, 10U);

    //TODO: remove (param1Value > 100) and only leave (param1Value > 0)
    if((Validate_UintDecAsciiValue(pParserCmdInfo->pParam1, 10, UINT32_MAX)) &&
       (param1Value >= 100))
    {
        //SysSleepStart(param1Value);
    }
    else
    {
        pParserCmdInfo->pReplyCmd = (char*)gapParserSysStatus[INVALID_PARAM_IDX];
    }
}

void Parser_SystemBootload(parserCmdInfo_t* pParserCmdInfo)
{
	bootloader_interface_setRequest();
	bootloader_reset();
}

void Parser_SystemReboot(parserCmdInfo_t* pParserCmdInfo)
{
    // Go for reboot, no reply necessary
    mal_reset();
}

void Parser_SystemFactReset(parserCmdInfo_t* pParserCmdInfo)
{
    uint16_t iCtr;

    //Delete EEPROM
    for(iCtr = 0; iCtr < 1024; iCtr ++)
    {
        DATAEE_WriteByte(iCtr, 0xFF);
    }
    
    // Go for reboot, no reply necessary
    mal_reset();
}

void Parser_SystemSetNvm(parserCmdInfo_t* pParserCmdInfo)
{
    uint8_t replyIdx = INVALID_PARAM_IDX;
    uint16_t param1Value;
    uint16_t param2Value; // the real value type is uint8_t, but strol truncates the value

    //TODO: set NVM restriction to protect the LoRaWAN params

    if(Validate_HexValue(pParserCmdInfo->pParam1) && Validate_HexValue(pParserCmdInfo->pParam2))
    {
        param1Value = strtol(pParserCmdInfo->pParam1, NULL, 16U);
        param2Value = strtol(pParserCmdInfo->pParam2, NULL, 16U);

        //LoRaWAN params are set between [0, USER_MEM_BASE): protected
        //User is allowed to write only between [USER_MEM_BASE, USER_MEM_BASE + USER_MEM_SIZE)
        //if (param1Value >= USER_MEM_BASE && param1Value <  (USER_MEM_BASE + USER_MEM_SIZE))
        //{
        //    if((param2Value > 0 && param2Value <= 255)  || strcmp("0", pParserCmdInfo->pParam2) == 0)
        //    {
        //        nvm_write(param1Value, param2Value);
        //        replyIdx = OK_STATUS_IDX;
        //    }
        //}
    }

    pParserCmdInfo->pReplyCmd = (char*)gapParserSysStatus[replyIdx];
}

extern Sx1276_eeprom_structure sx1276_eeprom_structure;
void Parser_SystemGetNvm(parserCmdInfo_t* pParserCmdInfo)
{
    uint8_t replyIdx = INVALID_PARAM_IDX;
    uint8_t memLocation;
    uint16_t paramValue;


    if(Validate_HexValue(pParserCmdInfo->pParam1))
    {
        paramValue = strtol(pParserCmdInfo->pParam1, NULL, 16U);
        //LoRaWAN params are set between [0, USER_MEM_BASE): protected
        //User is allowed to write only between [USER_MEM_BASE, USER_MEM_BASE + USER_MEM_SIZE)
        if ((paramValue >= 0) && (paramValue <  (MAX_EEPROM_PARAM_INDEX)))
        {
            memLocation = sx1276_eeprom_structure.sx1276_configuration_array[paramValue];
            utoa(aParserData, memLocation, 16U);
            pParserCmdInfo->pReplyCmd = aParserData;
            return;
        }
    }

    pParserCmdInfo->pReplyCmd = (char*)gapParserSysStatus[replyIdx];

}

void Parser_SystemSetPinMode(parserCmdInfo_t* pParserCmdInfo)
{
    uint8_t* pParam1 = (uint8_t*)pParserCmdInfo->pParam1;
    uint8_t* pParam2 = (uint8_t*)pParserCmdInfo->pParam2;
    uint8_t status = INVALID_PARAM_IDX;
    int8_t pinNumber = -1;
    uint8_t i;
    
    pinNumber = Pin_Index(pParam1);
    
    if (pinNumber >= 0)
    {
        for (i = 0; i < sizeof gapParserSysPinMode; i++)
        {
            if (0 == strcmp(gapParserSysPinMode[i], pParam2))
            {
                // user requested pin function i
                if ((gaAvailablePinFunctions[pinNumber] & (1 << i)) != 0)
                {
                    // pin function i is available
                    status = OK_STATUS_IDX;
                    switch (pinNumber)
                    {
                        case 0: {
                            switch (i)
                            {
                                case PM_DIGOUT:
                                    break;
                                case PM_DIGIN:
                                    break;
                                case PM_ANAIN:
                                    break;
                                case PM_SFUN:
                                    break;
                            }
                            break;
						}
                    }   
                }
            }
        }
    }
    
    pParserCmdInfo->pReplyCmd = (char*)gapParserSysStatus[status];
}

void Parser_SystemSetPinDig(parserCmdInfo_t* pParserCmdInfo)
{
    uint8_t* pParam1 = (uint8_t*)pParserCmdInfo->pParam1;
    uint8_t* pParam2 = (uint8_t*)pParserCmdInfo->pParam2;
    uint8_t status = INVALID_PARAM_IDX;
    int8_t pinNumber = -1;
    int8_t pinValue = -1;

    pinNumber = Pin_Index(pParam1);
    
    if(Validate_UintDecAsciiValue(pParam2, 1, 1))
    {
        pinValue = atoi(pParam2);
    }

    if ((pinNumber >= 0) && (pinValue >= 0))
    {
        status = OK_STATUS_IDX;
        switch (pinNumber)
        {
            case 0:
                if (0 == pinValue) {
                    //GPIO0_SetLow();
                } else {
                    //GPIO0_SetHigh();
				}
                break;
        }
    }

    pParserCmdInfo->pReplyCmd = (char*)gapParserSysStatus[status];
}

void Parser_SystemGetPinDig(parserCmdInfo_t* pParserCmdInfo)
{
    uint8_t* pParam1 = (uint8_t*)pParserCmdInfo->pParam1;
    uint8_t status = INVALID_PARAM_IDX;
    int8_t pinNumber = -1;
    int8_t pinValue = -1;

    pinNumber = Pin_Index(pParam1);
    
    if (pinNumber >= 0)
    {
        switch (pinNumber)
        {
            case 0: {
                pinValue = 0;
                break;
			}
        }
        status = OK_STATUS_IDX;
    }

    if (status != OK_STATUS_IDX)
    {
        pParserCmdInfo->pReplyCmd = (char*)gapParserSysStatus[status];
    }
    else
    {
        utoa(aParserData, pinValue, 10U);
        pParserCmdInfo->pReplyCmd = aParserData;
    }
}

void Parser_SystemGetPinAnalog(parserCmdInfo_t* pParserCmdInfo)
{
    uint8_t* pParam1 = (uint8_t*)pParserCmdInfo->pParam1;
    uint8_t status = INVALID_PARAM_IDX;
    int8_t pinNumber = -1;
    uint16_t pinValue;

    pinNumber = Pin_Index(pParam1);
    
    if (pinNumber >= 0)
    {
        if ((gaAvailablePinFunctions[pinNumber] & (1 << PM_ANAIN)) != 0)
        {
            // Selected pin has analog functionality
            status = OK_STATUS_IDX;
            switch (pinNumber)
            {
                case 0: {
                    pinValue = 0;
                    break;
				}
            }
        }
    }

    if (status != OK_STATUS_IDX)
    {
        pParserCmdInfo->pReplyCmd = (char*)gapParserSysStatus[status];
    }
    else
    {
        utoa(aParserData, pinValue, 10U);
        pParserCmdInfo->pReplyCmd = aParserData;
    }
}

extern uint8 sx1276_master_eep_error;
extern uint8 sx1276_backup_eep_error;
extern uint8 sx1276_critical_eep_error;
void Parser_SystemGetEepStatus(parserCmdInfo_t* pParserCmdInfo) {
	uint32 cnt = 0;
	aParserData[cnt] = 'M';cnt++;
	aParserData[cnt] = ':';cnt++;
	if (sx1276_master_eep_error) {
		aParserData[cnt] = '1';cnt++;
	} else {
		aParserData[cnt] = '0';cnt++;
	}
	aParserData[cnt] = ' ';cnt++;

	aParserData[cnt] = 'B';cnt++;
	aParserData[cnt] = ':';cnt++;
	if (sx1276_backup_eep_error) {
		aParserData[cnt] = '1';cnt++;
	} else {
		aParserData[cnt] = '0';cnt++;
	}
	aParserData[cnt] = ' ';cnt++;

	aParserData[cnt] = 'C';cnt++;
	aParserData[cnt] = ':';cnt++;
	if (sx1276_critical_eep_error) {
		aParserData[cnt] = '1';cnt++;
	} else {
		aParserData[cnt] = '0';cnt++;
	}
	pParserCmdInfo->pReplyCmd = aParserData;
}

void Parser_SystemGetVer(parserCmdInfo_t* pParserCmdInfo)
{
    Parser_GetSwVersion(aParserData);
    pParserCmdInfo->pReplyCmd = aParserData;
}

void Parser_SystemGetBattery(parserCmdInfo_t* pParserCmdInfo)
{
    uint16_t refVoltage;
    uint32_t batteryVoltage;

    pParserCmdInfo->pReplyCmd = (char*)gapParserSysStatus[ERR_STATUS_IDX];

    refVoltage = 0;

    if (refVoltage == 0)
    {
        batteryVoltage = 0xFFFF;
    }
    else
    {
        batteryVoltage = 1047552; // 1023 (max 10 bit value) * 1024 (mV)
        batteryVoltage = batteryVoltage / refVoltage; // value in mV
    }

    if (0xFFFF != batteryVoltage)
    {
        utoa(aParserData, batteryVoltage, 10U);
        pParserCmdInfo->pReplyCmd = aParserData;
    }
}

void Parser_SystemGetHwEui(parserCmdInfo_t* pParserCmdInfo)
{
    uint8_t aDevEui[8];

    System_GetExternalEui(aDevEui);
    Parser_IntArrayToHexAscii(8, aDevEui, aParserData);
    pParserCmdInfo->pReplyCmd = aParserData;
}