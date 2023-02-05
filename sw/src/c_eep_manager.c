#include "eep_manager.h"
#include "c_eep_manager.h"

#include "k_stdtype.h"
#include "sx1276.h"

extern Sx1276_eeprom_structure sx1276_eeprom_structure;
const Sx1276_eeprom_structure default_sx1276_eeprom_structure = {0};

extern Sx1276_eeprom_structure sx1276_eeprom_structure_backup;
const Sx1276_eeprom_structure default_sx1276_eeprom_structure_backup = {0};

EepManager_ItemTable eepManager_ItemTable[EepManager_Items_LastItem] = {
	{EepManager_Items_SX1276,					sizeof(sx1276_eeprom_structure),		(void *)&sx1276_eeprom_structure,			(void *)&default_sx1276_eeprom_structure,			EEP_USE_CRC,	EEP_HAVE_BACKUP},
	{EepManager_Items_SX1276_Backup,			sizeof(sx1276_eeprom_structure_backup),	(void *)&sx1276_eeprom_structure_backup,	(void *)&default_sx1276_eeprom_structure_backup,	EEP_USE_CRC,	EEP_HAVE_BACKUP},
};
