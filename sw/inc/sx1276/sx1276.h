#ifndef _SX1276_H_
#define _SX1276_H_

	#include "k_stdtype.h"
	#include "tmr.h"

	#define SX1276_EUI_SIZE 8
	#define MAX_EEPROM_PARAM_INDEX	0x200U
	
	typedef struct _Sx1276_eeprom_structure {
		uint8_t sx1276_EUI[SX1276_EUI_SIZE];
		uint8 sx1276_configuration_array[MAX_EEPROM_PARAM_INDEX + 1];
	} Sx1276_eeprom_structure;

	extern volatile uint32 sx1276_tx_completed;
	extern volatile uint32 sx1276_rx_started;
	extern volatile uint32 sx1276_rx_delay;
	extern volatile uint32 sx1276_rx_completed;
	extern volatile uint32 sx1276_rx_timeout;

	extern uint8_t DATAEE_ReadByte(uint16_t bAdd);
	extern void DATAEE_WriteByte(uint16_t bAdd, uint8_t bData);
	extern void System_GetExternalEui(uint8_t *id);

	
	extern void sx1276_unexpected_probem(void);
	extern void sx1276_eepManager_NotifyUserFailedRead(int item, uint8 type);
	
	extern void init_sx1276(void);
	extern void do_sx1276(void);
	extern void isr_sx1276_1ms(void);
	extern void isr_sx1276_100us(void);

#endif
