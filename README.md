
# STM32-OTA on Arduino IDE (for MACOS and Linux)

-----
### With a ESP8266, It can upload sketch to STM32 series MCU on Arduino IDE without any wires.

### [Veiw on YOUTUBE](https://youtu.be/3Bh6Y-V-LYg)

#### TESTED: STM32F03xF/K/C/,F103C8/B, (Blue Pill and Black Pill), F103VCT, F107RCT.

|If use ESP8266 Pin |STM32 MCU  |if use NodeMCU Pin |
| :-----: |:-----:| :-----:|
|RXD|PA9|RXD|
|TXD|PA10|TXD|
|Pin4|BOOT0|D2|
|Pin5|RST|D1|
|Vcc|3.3|3.3V|
|GND|GND|GND|
| En -> 3.3V |

-----
## Boards
<img src="./images/Esp-12E.jpg" width="200">  --- <img src="./images/BluePill-Pinout.png" width="200">  --- <img src="./images/nodemcu-pinout.jpg" width="200">  

-----
## Schematic

#### 1. Use ESP-12E 
<img src="./images/stm32-ota-esp8266.jpg" width="800">     

#### 2. Use NODEMCU 
<img src="./images/stm32-ota-nodemcu.jpg" width="800"> 

-----
## SETUP

### 1st step -> Flash ESP8266 

1. Download source-codes from [HERE](https://github.com/csnol/STM32-OTA/tree/master/ESP8266-Src)
2. Setup your WiFi and ESP8266 IPAddress to yours
<img src="./images/ESP8266-setup.png" width="800"> 
3. upload to ESP8266.   Don't know how to Flash ESP8266?  [HELP](https://github.com/esp8266/Arduino)  

### 2nd step -> Arduino IDE setup

#### -> For Arduino_STM32  [Repo link](https://github.com/rogerclarkmelbourne/Arduino_STM32)
1. Copy "stm32-ota" to "~/Document/Arduino/hardware/Arduino_STM32/tools/macosx"  or linux or linux64, and change it to become executable.     
 ~$ sudo chmod +x stm32-ota    
 
2. Find the file 'platform.txt'. and add below lines to the end of 'platform.txt'    
	> \# Upload using STM32 OTA over HTTPServer added by CSNOL        
>tools.stm32\_ota.cmd=stm32_ota        
>tools.stm32\_ota.path.macosx={runtime.hardware.path}/tools/macosx    
>tools.stm32\_ota.path.linux={runtime.hardware.path}/tools/linux    
>tools.stm32\_ota.path.linux64={runtime.hardware.path}/tools/linux64    
>tools.stm32\_ota.upload.params.verbose=\-d    
>tools.stm32\_ota.upload.params.quiet=    
>tools.stm32\_ota.upload.pattern="{path}/{cmd}" {upload.IPAddress} "{build.path}/{build.project\_name}.bin"

3. Please add below lines to the end of your Arduino_STM32 $path/STM32F1/boards.txt     
	> \# you can change 'genericSTM32F103C' to you want for flash others STM32 serial MCUs    
>\# STM32 OTA uploader ---- added by CSNOL    
>genericSTM32F103C.menu.upload\_method.httpMethod=STM32 OTA    
>genericSTM32F103C.menu.upload\_method.httpMethod.upload.protocol=stm32_ota    
>genericSTM32F103C.menu.upload\_method.httpMethod.upload.tool=stm32_ota    
>genericSTM32F103C.menu.upload\_method.httpMethod.upload.IPAddress=http://192.168.0.66    \# Change to your ESP8266 board's IPAddress

4. Restart Arduino IDE, you will find STM OTA upload method has been added.

<img src="./images/STM32-OTA-1.png" width="800"> 

#### -> For STM32GENERIC  [Repo link](https://github.com/danieleff/STM32GENERIC)

1. See upside Arduino_STM32 method.
2. See upside Arduino_STM32 method.
3. Please add below lines to the end of your STM32GENERIC $path/STM32/boards.txt         

	> \#Please add below lines to the end of your $path/STM32F1/boards.txt    
	>\# you can change 'BluePill' to you wanted for flash others STM32 serial MCUs    
>\# STM32 OTA uploader ---- added by CSNOL    
>BluePill.menu.upload\_method.httpMethod=STM32 OTA    
>BluePill.menu.upload\_method.httpMethod.upload.protocol=stm32\_ota    
>BluePill.menu.upload\_method.httpMethod.upload.tool=stm32\_ota    
>BluePill.menu.upload\_method.httpMethod.upload.IPAddress=http://192.168.0.66      
>\# Change to your ESP8266 board's IPAddress    

4. Restart Arduino IDE, you will find STM OTA upload method has been added.
<img src="./images/STM32GENERIC.jpg" width="800">

#### -> For stm32duino  [Repo link](https://github.com/stm32duino/Arduino_Core_STM32)

* Sameway to setup. but I haven't the ST official board. so can't test it.

### -> Now, you can OTA upload to STM32 MCU without any wires!

-----

# STM32-OTA on Arduino IDE (for Windows)

### THANKS for [LEVKOVIGOR](https://github.com/levkovigor)

* Please move to [levkovigor/STM32-OTA-ESP8266-for-windows](https://github.com/levkovigor/STM32-OTA-ESP8266-for-windows)

* Use busybox tools for windows, really good Idea!

-----
### BTW
* If you just wanna OTA upload a bin file to STM32, please see [HERE](https://github.com/csnol/1CHIP-Programmers)
* If you want to keep STM32-bootloader, please change the STM32STADDR in "stm32ota.h"    
 
>\#define STM32STADDR  	0x8000000     // STM32 codes start address, you can change to other address if use custom bootloader: like 0x8002000

-----
## TODO: 
> 1. OTA upload all STM32 series MCU in Arduino IDE.
> 2. Make a video upload to Youtube.

## LICENSE

Copyright (C) 2017 by  [CS.NOL](https://github.com/csnol/STM32-OTA) . This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, but you have to keep below webserver code in your sketch for sharing.
>  Version 1.0 by \<a style=\"color:white\" href=\"https://github.com/csnol/STM32-OTA\">CSNOL 

