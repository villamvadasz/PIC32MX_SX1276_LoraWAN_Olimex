#include "sx1276.h"
#include "c_sx1276.h"

#include "lorawan.h"
#include "sw_timer.h"
#include "eep_manager.h"
#include "parser.h"

uint8 sx1276_master_eep_error = 0;
uint8 sx1276_backup_eep_error = 0;
uint8 sx1276_critical_eep_error = 0;
uint8 master_eep_error_single_shoot = 1;
uint8 backup_eep_error_single_shoot = 1;

Sx1276_eeprom_structure sx1276_eeprom_structure = {0};
Sx1276_eeprom_structure sx1276_eeprom_structure_backup = {0};

UINT8 sx1276_DATAEE_WriteByte_update = 0;

uint8_t sx1276_external_eui_buffer[SX1276_EUI_SIZE];
volatile uint32 sx1276_globalTime_us_high_res = 0;
volatile uint32 sx1276_tx_completed = 0;
volatile uint32 sx1276_rx_started = 0;
volatile uint32 sx1276_rx_delay = 0;
volatile uint32 sx1276_rx_completed = 0;
volatile uint32 sx1276_rx_timeout = 0;
	

void AppRestoreMacFromEep(void);

void init_sx1276(void) {
	#ifdef SX1276_DIO0_TRIS
		SX1276_DIO0_TRIS = 1;
	#endif
	#ifdef SX1276_DIO1_TRIS
		SX1276_DIO1_TRIS = 1;
	#endif
	#ifdef SX1276_DIO2_TRIS
		SX1276_DIO2_TRIS = 1;
	#endif
	#ifdef SX1276_DIO3_TRIS
		SX1276_DIO3_TRIS = 1;
	#endif
	#ifdef SX1276_DIO4_TRIS
		SX1276_DIO4_TRIS = 1;
	#endif
	#ifdef SX1276_DIO5_TRIS
		SX1276_DIO5_TRIS = 1;
	#endif

	SX1276_nReset_TRIS = 0;
	SX1276_nReset_LAT = 0; //Reset

	SX1276_nCS_TRIS = 0;
	SX1276_nCS_LAT = 1;

	SystemTimerInit();
	Parser_Init();
	
	
	if (sx1276_master_eep_error == 0) {
		//master is ok no need to check backup
	} else {
		//master failed, see for back up
		if (sx1276_backup_eep_error) {
			//panic...
			sx1276_critical_eep_error = 1;
		} else {
			//load backup
			memcpy(&sx1276_eeprom_structure, &sx1276_eeprom_structure_backup, sizeof(sx1276_eeprom_structure));
		}
	}
}

void do_sx1276(void) {
	static unsigned int do_sx1276_state = 0;
	
	sx1276_globalTime_us_high_res = getGlobalTimeUs_Lora();

	switch (do_sx1276_state) {
		case 0 : {
			if (sx1276_master_eep_error) {
				if (master_eep_error_single_shoot) {
					master_eep_error_single_shoot = 0;
					//TODO some eeprom handling
				}
			}
			if (sx1276_critical_eep_error) {
				if (backup_eep_error_single_shoot) {
					backup_eep_error_single_shoot = 0;
					//TODO some eeprom handling
				}
			}
			
			if (sx1276_critical_eep_error) {
				//Unconfigured EEPROM or damaged eeprom
				//Reset LoRaWAN
				LORAWAN_Reset(ISM_EU868);            
				
				// Set default EUI to unique value
				System_GetExternalEui(sx1276_external_eui_buffer);
				LORAWAN_SetDeviceEui(sx1276_external_eui_buffer);
			} else {
				//Normal behaviour to load config parameters from eeprom
				AppRestoreMacFromEep();
			}
			do_sx1276_state = 1;
			break;
		}
		case 1 : {
			if (sx1276_DATAEE_WriteByte_update) {
				sx1276_DATAEE_WriteByte_update = 0;
				eep_manager_WriteAll_Trigger();
			}
			LORAWAN_Mainloop();
			Parser_Main();
			break;
		}
		default : {
			do_sx1276_state = 0;
			break;
		}
	}
}

void isr_sx1276_1ms(void) {
}

void isr_sx1276_100us(void) {
}

void sx1276_unexpected_probem(void) {
	volatile int debug_cahtcher_sx1276_unexpected_probem = 0;
	debug_cahtcher_sx1276_unexpected_probem++;
	debug_cahtcher_sx1276_unexpected_probem++;
	debug_cahtcher_sx1276_unexpected_probem++;
	debug_cahtcher_sx1276_unexpected_probem++;
}

void sx1276_eepManager_NotifyUserFailedRead(int item, uint8 type) {
	switch (item) {
		case EepManager_Items_SX1276 : {
			sx1276_master_eep_error = 1;
			break;
		}
		case EepManager_Items_SX1276_Backup : {
			if (type == EEP_MANAGER_LOAD_FAILED) {
				sx1276_backup_eep_error = 1;
			}
			break;
		}
		default : {
			break;
		}
	}
}

uint8_t DATAEE_ReadByte(uint16_t bAdd) {
	uint8_t	result = 0xFF;
	if (bAdd < MAX_EEPROM_PARAM_INDEX) {
		result = sx1276_eeprom_structure.sx1276_configuration_array[bAdd];
	} else {
		sx1276_unexpected_probem();
	}
	return result;
}

void DATAEE_WriteByte(uint16_t bAdd, uint8_t bData) {
	if (bAdd < MAX_EEPROM_PARAM_INDEX) {
		sx1276_eeprom_structure.sx1276_configuration_array[bAdd] = bData;
		sx1276_DATAEE_WriteByte_update = 1;
	} else {
		sx1276_unexpected_probem();
	}
}

void System_GetExternalEui(uint8_t *id) {
	if (id != NULL) {
		#warning TODO Get here some magical unique EUI
		memset(id, 0x00, SX1276_EUI_SIZE);
		//FFFFFFFFFFFFFFFF
		id[0] = 0xFF;
		id[1] = 0xFF;
		id[2] = 0xFF;
		id[3] = 0xFF;
		id[4] = 0xFF;
		id[5] = 0xFF;
		id[6] = 0xFF;
		id[7] = 0xFF;
	}
}

void AppRestoreMacFromEep(void) {
	auint32_t temp32 = {0};
	uint16_t temp16 = 0;
	auint16_t itemp16 = {0};
	uint16_t startIdx = 0;
	uint8_t tempBuff[16] = {0};
	uint8_t iCtr = 0;
	uint8_t temp = 0;
	uint8_t jCtr = 0;

	parserConfiguredJoinParameters_t configuredJoinParameters;
	
	//Read the LoRaWAN configuration parameters from internal EEPROM
	startIdx = 0;

	//ISM frequency band
	temp = DATAEE_ReadByte(startIdx ++);
	LORAWAN_Reset(temp);
	
	// Restore RX2 window parameters
	temp = DATAEE_ReadByte(startIdx ++);
	for(iCtr = 0U; iCtr < 4U; iCtr ++)
	{
		temp32.buffer[iCtr] = DATAEE_ReadByte(startIdx ++);
	}  
	LORAWAN_SetReceiveWindow2Parameters(temp32.value, temp);

	// Restore configured join parameters
	temp = DATAEE_ReadByte(startIdx ++);
	temp16 = temp;
	temp16 <<= 8U;
	temp = DATAEE_ReadByte(startIdx ++);
	temp16 += temp;
	Parser_SetConfiguredJoinParameters(temp16);
	configuredJoinParameters.value = temp16;
	
	temp = DATAEE_ReadByte(startIdx ++);
	LORAWAN_SetClass(temp);
	
	// Restore configured uplink counter
	for(iCtr = 0U; iCtr < 4U; iCtr ++)
	{
		temp32.buffer[iCtr] = DATAEE_ReadByte(startIdx ++);
	}
	LORAWAN_SetUplinkCounter(temp32.value);

	// Restore configured downlink counter
	for(iCtr = 0U; iCtr < 4U; iCtr ++)
	{
		temp32.buffer[iCtr] = DATAEE_ReadByte(startIdx ++);
	}
	LORAWAN_SetDownlinkCounter(temp32.value);

	//EEPROM[0..7] device EUI
	for(iCtr = 0U; iCtr < 8U; iCtr ++)
	{
		tempBuff[iCtr] = DATAEE_ReadByte(startIdx ++);
	}

	if (1 == configuredJoinParameters.flags.deveui)
	{
		LORAWAN_SetDeviceEui(tempBuff);
	}

	//EEPROM[8..15] application EUI
	for(iCtr = 0U; iCtr < 8U; iCtr ++)
	{
		tempBuff[iCtr] = DATAEE_ReadByte(startIdx ++);
	}

	if (1 == configuredJoinParameters.flags.appeui)
	{
		LORAWAN_SetApplicationEui(tempBuff);
	}

	//EEPROM[16..31] application key
	for(iCtr = 0U; iCtr < 16U; iCtr ++)
	{
		tempBuff[iCtr] = DATAEE_ReadByte(startIdx ++);
	}

	if (1 == configuredJoinParameters.flags.appkey)
	{
		LORAWAN_SetApplicationKey(tempBuff);
	}

	//EEPROM[32..47] network session key
	for(iCtr = 0U; iCtr < 16U; iCtr ++)
	{
		tempBuff[iCtr] = DATAEE_ReadByte(startIdx ++);
	}

	if (1 == configuredJoinParameters.flags.nwkskey)
	{
		LORAWAN_SetNetworkSessionKey(tempBuff);
	}

	//EEPROM[48..63] application session key
	for(iCtr = 0U; iCtr < 16U; iCtr ++)
	{
		tempBuff[iCtr] = DATAEE_ReadByte(startIdx ++);
	}

	if (1 == configuredJoinParameters.flags.appskey)
	{
		LORAWAN_SetApplicationSessionKey(tempBuff);
	}

	//EEPROM[64..68] device address
	temp32.value = 0;
	for(iCtr = 0U; iCtr < 4U; iCtr ++)
	{
		temp32.buffer[iCtr] = DATAEE_ReadByte(startIdx ++);
	}

	if (1 == configuredJoinParameters.flags.devaddr)
	{
		LORAWAN_SetDeviceAddress(temp32.value);
	}
	
	//EEPROM[69..85] app multicast session key
	for(iCtr = 0U; iCtr < 16U; iCtr ++)
	{
		tempBuff[iCtr] = DATAEE_ReadByte(startIdx ++);
	}
	
	if (1 == configuredJoinParameters.flags.appmultiskey)
	{
		LORAWAN_SetMcastApplicationSessionKey(tempBuff);
	}
	
	//EEPROM[86..102] network multicast session key
	for(iCtr = 0U; iCtr < 16U; iCtr ++)
	{
		tempBuff[iCtr] = DATAEE_ReadByte(startIdx ++);
	}
	
	if (1 == configuredJoinParameters.flags.nwkmultiskey)
	{
		LORAWAN_SetMcastNetworkSessionKey(tempBuff);
	}
	
	//EEPROM[103..107] device multicast address
	temp32.value = 0;
	for(iCtr = 0U; iCtr < 4U; iCtr ++)
	{
		temp32.buffer[iCtr] = DATAEE_ReadByte(startIdx ++);
	}
	
	if (1 == configuredJoinParameters.flags.devmultiaddr)
	{
		LORAWAN_SetMcastDeviceAddress(temp32.value);
	}
	
	temp = DATAEE_ReadByte(startIdx ++);
	LORAWAN_SetMcast(temp);
	
	//EU specific parser code - channel list update
	//For the first 3 channels only the status and the duty cycle can be configured
	for(jCtr = 0U; jCtr < 3U; jCtr ++)
	{
		//Channel status
		//TODO: check if memory == 0 then status = disable
		temp = DATAEE_ReadByte(startIdx ++);
		LORAWAN_SetChannelIdStatus(jCtr, (temp > 0) ? 1: 0);

		//Channel DRRange
		temp = DATAEE_ReadByte(startIdx ++);
		LORAWAN_SetDataRange(jCtr, temp);

		//Channel dutycycle
		temp32.value = 0;
		for(iCtr = 0U; iCtr < 4U; iCtr ++)
		{
			temp32.buffer[iCtr] = DATAEE_ReadByte(startIdx ++);
		}
		LORAWAN_SetDutyCycle(jCtr, temp32.value);
	}

	//Restore from EEPROM the parameters for the rest of the channels
	for(jCtr = 0U; jCtr < 29U; jCtr ++)
	{
		//Frequency
		temp32.value = 0;
		for(iCtr = 0U; iCtr < 4U; iCtr ++)
		{
			temp32.buffer[iCtr] = DATAEE_ReadByte(startIdx ++);
		}
		LORAWAN_SetFrequency(jCtr + 3, temp32.value);

		//Dutycycle
		temp32.value = 0;
		for(iCtr = 0U; iCtr < 4U; iCtr ++)
		{
			temp32.buffer[iCtr] = DATAEE_ReadByte(startIdx ++);
		}
		LORAWAN_SetDutyCycle(jCtr + 3, temp32.value);

		//DR Range
		temp = DATAEE_ReadByte(startIdx ++);
		LORAWAN_SetDataRange(jCtr + 3, temp);

		//Status
		temp = DATAEE_ReadByte(startIdx ++);
		LORAWAN_SetChannelIdStatus(jCtr + 3, temp);
	}
	
	//Current data rate
	temp = DATAEE_ReadByte(startIdx ++);
	LORAWAN_SetCurrentDataRate(temp);
	
	//Current ADR state
	temp = DATAEE_ReadByte(startIdx ++);
	LORAWAN_SetAdr(temp);

	//Current Receive Offset
	temp = DATAEE_ReadByte(startIdx ++);
	LORAWAN_SetReceiveOffset(temp);
	
	//Current Rx1Delay
	itemp16.value = 0;
	for(iCtr = 0U; iCtr < 2U; iCtr ++)
	{
		itemp16.buffer[iCtr] = DATAEE_ReadByte(startIdx ++);
	}
	LORAWAN_SetReceiveDelay1(itemp16.value);
}
