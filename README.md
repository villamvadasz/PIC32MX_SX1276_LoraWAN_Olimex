# PIC32MX_SX1276_LoraWAN_Olimex

## Description
Port of the EU RN2483 Firmware from Microchip to PIC32 olimex board with SX1276. Project was used on The Things Network V3.

## Requried SW:
- This github
- XC32 Compiler v2.30
- TTN account
- Terminal SW like RealTerm

## Required HW:
- PIC32-PINGUINO-MICRO https://www.olimex.com/Products/Duino/PIC32/PIC32-PINGUINO-MICRO/open-source-hardware
- LoRa868 https://www.olimex.com/Products/IoT/LoRa/LoRa868/open-source-hardware
- Optionally a TTN Gateway if you do not have any nearby. Suggest higly to have one, for easier debugging.

## Referenced documents
- RN2483 https://www.microchip.com/en-us/product/RN2483
- RN2483 Firmware Microchip gihtub https://github.com/search?q=topic:rn2483+org:MicrochipTech
- RN2483 Command Reference https://ww1.microchip.com/downloads/en/DeviceDoc/RN2483-LoRa-Technology-Module-Command-Reference-User-Guide-DS40001784G.pdf
- SX1276 pdf https://www.mouser.com/datasheet/2/761/sx1276-1278113.pdf
- SX1276 Product sitehttps://www.semtech.com/products/wireless-rf/lora-connect/sx1276

## Setup
### Change EUI
Before you start you must change the EUI in the code. It is set to FF, which is invalid. Generate one in TTN and then add that one here.
PIC32MX_SX1276_LoraWAN_Olimex\sw\src\sx1276\sx1276.c 
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

### Update command files
In the folder there are commands file as example. Please check them and update the Keys and the DeviceID.
PIC32MX_SX1276_LoraWAN_Olimex\sw\src\sx1276\config_txt\
mac set appskey FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
mac set nwkskey FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
mac set devaddr 260B438D


### Compile and flash
Compile the project, then flash it. The Microchip stack generates a lot of warnings. 

### Connect over USB and play
The PIC32 can be connected over USB to the PC. In a terminal windows you can use the same commands are for the RN2483. Some sys commands are not supported. For testing I would recommend the command file PIC32MX_SX1276_LoraWAN_Olimex\sw\src\sx1276\config_txt\lora_config_duty_100.txt.

Otherwise the txt files should work with TTN.

## Fixed BUGS

### FindSmallestDataRate
The function
static bool FindSmallestDataRate (void)
had I think a small bug. Not sure, but it was stuck in an endless loop under special conditions.
Original:
        while ( (found == false) && (dataRate >= loRa.minDataRate) )
Fix:
        while ( (found == false) && (dataRate > loRa.minDataRate) )
If this might cause a problem just tell me.

### Unaligned access
The lorawan_eu.c had some unalligned access:
    frequency = (*((uint32_t*)ptr)) & 0x00FFFFFF;
This would try to read a uint8 pointer as a uint32 pointer, pointing to not a 4 byte aligned location, causing an exception. Fixed with:
    frequency = sx1276_read_uint32_unaligend(ptr) & 0x00FFFFFF;
### Unaligned access
DIO5 is not connected, so mode switch is now polling