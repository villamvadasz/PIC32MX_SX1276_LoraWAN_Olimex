#ifndef _C_EEP_MANAGER_H_
#define _C_EEP_MANAGER_H_

	#define EEPROM_START_ADDR	0x0000
	#define EEPROM_SIZE			0x0800

	//Use 0xFF to disable version check
	//#define EEPROM_VERSION 0xFF

	#define EEPROM_VERSION 0x01
	//#define EEPROM_INVALIDATE_OLD

	typedef enum _EepManager_Item {
		EepManager_Items_SX1276 = 0,
		EepManager_Items_SX1276_Backup,
		EepManager_Items_LastItem,
	} EepManager_Item;

#endif
