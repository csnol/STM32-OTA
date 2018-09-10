  
####  New functions in STM32 OTA 1.1 Bata.    
   
 * You can OTA upload to ESP8266 based board at 192.168.0.66:8266 port.     
 * Support OTA all STM32 MCUs in WebBrowser or Arduino IDE.    
 * How to flash your ESP8266 sketch:  
   1. Open http://192.168.x.66 or http://stm32-ota.local on Web-Browser, click "Flash ESP8266".
   2. Select your Esp8266 board at Arduino IDE -> tools -> Board.
   3. Select "Esp8266-xxxx at 192.168.x.66" at Arduino IDE -> tools -> Port.
   4. Open Arduino IDE -> File -> ArduinoOTA->BasicOTA
   5. Add your codes in "BasicOTA.ino"
   6. Add below codes in setup() to reset STM32
   
   ``` C++
        pinMode(NRST, OUTPUT) // ESP8266 pin connected to STM32 NRST
        digitalWrite(NRST, LOW);
        digitalWrite(LED, LOW);
        delay(50);
   ```
   
   Click "Upload" icon to programming your ESP8266.
