@echo off
cls
del /q .\output\*.d
del /q .\output\*.o
del /q .\output\*.hex
del /q .\output\*.map
del /q .\output\*.elf

.\make\make.exe -f Makefile_440F256H clean
.\make\make.exe -f Makefile_440F256H all -j8
copy .\output\output.hex .\output\PIC32_OLIMEX_LORA_440F256H.hex
copy .\output\output.map .\output\PIC32_OLIMEX_LORA_440F256H.map
copy .\output\output.elf .\output\PIC32_OLIMEX_LORA_440F256H.elf
.\make\make.exe -f Makefile_440F256H clean

del /q .\output\output.map
del /q .\output\*.d
del /q .\output\*.o
rem pause
