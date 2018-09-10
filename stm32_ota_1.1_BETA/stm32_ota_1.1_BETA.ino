/*
//
//     Copyright (C) 2017  CS.NOL  https://github.com/csnol/STM32-OTA
//
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License,
//     and You have to keep below webserver code
//     "<h2>Version 1.1 by <a style=\"color:white\" href=\"https://github.com/csnol/STM32-OTA\">CSNOL"
//     in your sketch.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    It is assumed that the STM32 MCU is connected to the following pins with NodeMCU or ESP8266/8285.
//    Tested and supported MCU : STM32F03xF/K/C/，F05xF/K/C,F10xx8/B, F105/7
//
//    连接ESP8266模块和STM32系列MCU.    Connect ESP8266 to STM32 MCU
//
//    ESP8266/8285 Pin       STM32 MCU      NodeMCU Pin(ESP8266 based)
//    RXD                  	PA9             RXD
//    TXD                  	PA10            TXD
//    Pin4                 	BOOT0           D2
//    Pin5                 	RST             D1
//    Vcc                  	3.3V            3.3V
//    GND                  	GND             GND
//    En -> 10K -> 3.3V
//
//  New functions in STM32 OTA 1.1 Bata.    
//   
//  * You can OTA upload to ESP8266 based board at 192.168.0.66:8266 port.     
//  * Support OTA all STM32 MCUs in WebBrowser or Arduino IDE.    
//  * How to flash your ESP8266 sketch:  
//   1. Open http://192.168.x.66 or http://stm32-ota.local on Web-Browser, click "Flash ESP8266".
//   2. Select your Esp8266 board at Arduino IDE -> tools -> Board.
//   3. Select "Esp8266-xxxx at 192.168.x.66" at Arduino IDE -> tools -> Port.
//   4. Open Arduino IDE -> File -> ArduinoOTA->BasicOTA
//   5. Add your codes in "BasicOTA.ino"
//   6. Add below codes in setup() to reset STM32
//        pinMode(NRST, OUTPUT) // ESP8266 pin connected to STM32 NRST
//        digitalWrite(NRST, LOW);
//        digitalWrite(LED, LOW);
//        delay(50);
//   
//   Click "Upload" icon to programming your ESP8266.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include "spiffs/spiffs.h"
#include <FS.h>
#include <ESP8266mDNS.h>
#include "stm32ota.h"

const String STM32_CHIPNAME[47] = {
  "Unknown Chip",
  "STM32F030x8/05x",
  "STM32F03xx4/6",
  "STM32F030xC",
  "STM32F04xxx/070x6",
  //"STM32F070x6",
  "STM32F070xB/071xx/072xx",
  //"STM32F071xx/072xx",
  "STM32F09xxx",
  "STM32F10xxx-LD",
  "STM32F10xxx-MD",
  "STM32F10xxx-HD",
  "STM32F10xxx-MD-VL",
  "STM32F10xxx-HD-VL",
  "STM32F105/107",
  "STM32F10xxx-XL-D",
  "STM32F20xxxx",
  "STM32F373xx/378xx",
  //"STM32F378xx",
  "STM32F302xB(C)/303xB(C)/358xx",
  //"STM32F358xx",
  "STM32F301xx/302x4(6/8)/318xx",
  //"STM32F318xx",
  "STM32F303x4(6/8)/334xx/328xx",
  "STM32F302xD(E)/303xD(E)/398xx",
  //"STM32F398xx",
  "STM32F40xxx/41xxx",
  "STM32F42xxx/43xxx",
  "STM32F401xB(C)",
  "STM32F401xD(E)",
  "STM32F410xx",
  "STM32F411xx",
  "STM32F412xx",
  "STM32F446xx",
  "STM32F469xx/479xx",
  "STM32F413xx/423xx",
  "STM32F72xxx/73xxx",
  "STM32F74xxx/75xxx",
  "STM32F76xxx/77xxx",
  "STM32H74xxx/75xxx",
  "STM32L01xxx/02xxx",
  "STM32L031xx/041xx",
  "STM32L05xxx/06xxx",
  "STM32L07xxx/08xxx",
  "STM32L1xxx6(8/B)",
  "STM32L1xxx6(8/B)A",
  "STM32L1xxxC",
  "STM32L1xxxD",
  "STM32L1xxxE",
  "STM32L43xxx/44xxx",
  "STM32L45xxx/46xxx",
  "STM32L47xxx/48xxx",
  "STM32L496xx/4A6xx"
};

#define NRST 5
#define BOOT0 4
#define LED 2

const char* ssid = "your-wifi-ssid";
const char* password = "your-wifi-password";

IPAddress local_IP(192, 168, 0, 66);       // Change the IP to yours
IPAddress gateway(192, 168, 0, 1);        // Change to your WiFi Router IP
IPAddress subnet(255, 255, 255, 0);      // Change to your WiFi subnet

ESP8266WebServer server(80);
const char* serverIndex = "<h1>Upload STM32 BinFile</h1><h2><br><br><form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Upload'></form></h2>";
File fsUploadFile;
uint8_t binread[256];
int bini = 0;
int rdtmp;
int stm32ver;
String stringtmp;
//bool initflag = 0;
bool Runflag = 0;
bool ESP_OTA_Flag = 1;

void WiFiConnect() {
  uint8_t waitt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //     Serial.print(".");
    waitt++;
    if ( waitt > 20) {
      waitt = 0;
      ESP.deepSleep(10000000, WAKE_RF_DEFAULT);   //deepsleep esp8266 10s and reconnect again.
      delay(100);
    }
  }
}

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
  s += "<title >";
  s += title;
  s += "</title></head><body text=#ffffff bgcolor=##4da5b9 align=\"center\">";
  s += contents;
  s += "</body></html>";
  return s;
}

// Upload STM32 Bin file to STM32 MCU from the flash of ESP8266 and shown on HTTPserver
void handleFlash()
{
  String FileName, flashwr;
  int lastbuf = 0;
  uint8_t cflag, fnum = 256;
  Dir dir = SPIFFS.openDir("/");
  while (dir.next())
  {
    FileName = dir.fileName();
  }
  fsUploadFile = SPIFFS.open(FileName, "r");
  if (fsUploadFile) {
    bini = fsUploadFile.size() / 256;
    lastbuf = fsUploadFile.size() % 256;
    flashwr = String(bini) + "-" + String(lastbuf) + "<br>";
    for (int i = 0; i < bini; i++) {
      fsUploadFile.read(binread, 256);
      stm32SendCommand(STM32WR);
      while (!Serial.available()) ;
      cflag = Serial.read();
      if (cflag == STM32ACK)
        if (stm32Address(STM32STADDR + (256 * i)) == STM32ACK) {
          if (stm32SendData(binread, 255) == STM32ACK)
            flashwr += ".";
          else flashwr = "Error";
        }
    }
    fsUploadFile.read(binread, lastbuf);
    stm32SendCommand(STM32WR);
    while (!Serial.available());
    cflag = Serial.read();
    if (cflag == STM32ACK)
      if (stm32Address(STM32STADDR + (256 * bini)) == STM32ACK) {
        if (stm32SendData(binread, lastbuf) == STM32ACK)
          flashwr += "<br>Finished<br>";
        else flashwr = "Error";
      }
    //flashwr += String(binread[0]) + "," + String(binread[lastbuf - 1]) + "<br>";
    fsUploadFile.close();
    String flashhtml = "<h1>Programming</h1><h2>" + flashwr +  "<br><br><a style=\"color:white\" href=\"/up\">Upload STM32 BinFile</a><br><br><a style=\"color:white\" href=\"/list\">List STM32 BinFile</a></h2>";
    server.send(200, "text/html", makePage("Flash Page", flashhtml));
  }
}

// Upload STM32 Bin files to the flash of ESP8266 from local machine and shown on HTTPserver
void handleFileUpload()
{
  if (server.uri() != "/upload") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
  }
}

// Delete STM32 Bin file from the flash of ESP8266 and shown on HTTPserver
void handleFileDelete() {
  //int binhigh = 0;
  String FileList = "File: ";
  String FName;
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    FName = dir.fileName();
  }
  FileList += FName;
  if (SPIFFS.exists(FName)) {
    server.send(200, "text/html", makePage("Deleted", "<h2>" + FileList + " be deleted!<br><br><a style=\"color:white\" href=\"/list\">Return </a></h2>"));
    SPIFFS.remove(FName);
  }
  else
    return server.send(404, "text/html", makePage("File Not found", "404"));
}

// List STM32 Bin files in the flash of ESP8266 and shown on HTTPserver
void handleListFiles()
{
  String FileList = "Bootloader Ver: ";
  String Listcode;
  char blversion = 0;
  Dir dir = SPIFFS.openDir("/");
  blversion = stm32Version();
  FileList += String((blversion >> 4) & 0x0F) + "." + String(blversion & 0x0F) + "<br> MCU: ";
  FileList += STM32_CHIPNAME[stm32GetId()];
  FileList += "<br><br> File: ";
  while (dir.next())
  {
    String FileName = dir.fileName();
    File f = dir.openFile("r");
    String FileSize = String(f.size());
    int whsp = 6 - FileSize.length();
    while (whsp-- > 0)
    {
      FileList += " ";
    }
    FileList +=  FileName + "   Size:" + FileSize;
  }
  Listcode = "<h1>List STM32 BinFile</h1><h2>" + FileList + "<br><br><a style=\"color:white\" href=\"/flash\">Flash Menu</a><br><br><a style=\"color:white\" href=\"/delete\">Delete BinFile </a><br><br><a style=\"color:white\" href=\"/up\">Upload BinFile</a></h2>";
  server.send(200, "text/html", makePage("FileList", Listcode));
}

void setup(void)
{
  SPIFFS.begin();
  Serial.begin(115200, SERIAL_8E1);
  pinMode(BOOT0, OUTPUT);
  pinMode(NRST, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(NRST, LOW);
  digitalWrite(LED, LOW);
  delay(50);
  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);
  delay(100);
  FlashMode();
  WiFiConnect();
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);
  ArduinoOTA.onStart([]() {
    digitalWrite(LED, LOW);
    delay(500);
    digitalWrite(LED, HIGH);
  });
  ArduinoOTA.onEnd([]() {
    for ( int i = 0; i < 3; i++) {
      digitalWrite(LED, !digitalRead(LED));
      delay(100);
    }
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    if ((progress / (total / 100)) <= 99)  {
      digitalWrite(LED, !digitalRead(LED));
    }
  });
  ArduinoOTA.onError([](ota_error_t error) {
  digitalWrite(NRST, LOW);
  digitalWrite(LED, LOW);
  delay(50);
  ESP.restart();
  });
  MDNS.begin("stm32-ota");
  server.on("/up", HTTP_GET, []() {
    server.send(200, "text/html", makePage("Select file", serverIndex));
  });
  server.on("/list", HTTP_GET, handleListFiles);
  server.on("/programm", HTTP_GET, handleFlash);
  server.on("/run", HTTP_GET, []() {
    String Runstate = "STM32 Restart and runing!<br><br>Click Mode switch to RunMode or FlashMode <br><br>Click Home return by Flash mode";
    // stm32Run();
    if (Runflag == 0) {
      RunMode();
      Runflag = 1;
    }
    else {
      FlashMode();
      // initflag = 0;
      Runflag = 0;
    }
    server.send(200, "text/html", makePage("Run", "<h2>" + Runstate + "<br><br><a style=\"color:white\" href=\"/run\">1.Mode </a><br><br><a style=\"color:white\" href=\"/\">2.Home </a></h2>"));
  });
  server.on("/erase", HTTP_GET, []() {
    if (stm32Erase() == STM32ACK)
      stringtmp = "<h1>Erase OK</h1><h2><a style=\"color:white\" href=\"/list\">Return </a></h2>";
    else if (stm32Erasen() == STM32ACK)
      stringtmp = "<h1>Erase OK</h1><h2><a style=\"color:white\" href=\"/list\">Return </a></h2>";
    else
      stringtmp = "<h1>Erase failure</h1><h2><a style=\"color:white\" href=\"/list\">Return </a></h2>";
    server.send(200, "text/html", makePage("Erase page", stringtmp));
  });
  server.on("/flash", HTTP_GET, []() {
    stringtmp = "<h1>FLASH MENU</h1><h2><a style=\"color:white\" href=\"/programm\">Flash STM32</a><br><br><a style=\"color:white\" href=\"/erase\">Erase STM32</a><br><br><a style=\"color:white\" href=\"/run\">Run STM32</a><br><br><a style=\"color:white\" href=\"/list\">Return </a></h2>";
    server.send(200, "text/html", makePage("Flash page", stringtmp));
  });
  server.on("/esp-ota", HTTP_GET, []() {
    stringtmp = "<h1>Update ESP8266 </h1>";
    server.send(200, "text/html", makePage("ESP-OTA-Mode", stringtmp));
    ArduinoOTA.begin();
    ESP_OTA_Flag = 0;
      digitalWrite(LED, 1);    
    for ( int j = 0; j < 3; j++) {
      digitalWrite(LED, !digitalRead(LED));
      delay(100);
    }
  });
  server.on("/delete", HTTP_GET, handleFileDelete);
  server.onFileUpload(handleFileUpload);
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/html", makePage("FileList", "<h1> Uploaded OK </h1><br><br><h2><a style=\"color:white\" href=\"/list\">Return </a></h2>"));
  });
  server.on("/", HTTP_GET, []() {
    if (Runflag == 1) {
      FlashMode();
      Runflag = 0;
    }
    //if (initflag == 0)
    //{
    Serial.write(STM32INIT);
    delay(10);
    if (Serial.available() > 0);
    rdtmp = Serial.read();
    if (rdtmp == STM32ACK)   {
      //initflag = 1;
      stringtmp = STM32_CHIPNAME[stm32GetId()];
    }
    else if (rdtmp == STM32NACK) {
      Serial.write(STM32INIT);
      delay(10);
      if (Serial.available() > 0);
      rdtmp = Serial.read();
      if (rdtmp == STM32ACK)   {
        //initflag = 1;
        stringtmp = STM32_CHIPNAME[stm32GetId()];
      }
    }
    else  {
      stringtmp = "ERROR";
    }
    String starthtml = "<h1>STM32-OTA</h1><h2>Version 1.1 by <a style=\"color:white\" href=\"https://github.com/csnol/STM32-OTA\">CSNOL<br><br><a style=\"color:white\" href=\"/list\">List STM32 BinFile </a><br><br><a style=\"color:white\" href=\"/esp-ota\">Flash ESP8266 </a></h2>";
    server.send(200, "text/html", makePage("Start Page", starthtml + "- Init MCU -<br> " + stringtmp));
  });
  server.begin();
}

void loop(void) {
  if (ESP_OTA_Flag == 0)  {
    ArduinoOTA.handle();
  }
  else
    server.handleClient();
  //delay(0);
}


void FlashMode()  {    //Tested  Change to flashmode
  digitalWrite(BOOT0, HIGH);
  delay(100);
  digitalWrite(NRST, LOW);
  digitalWrite(LED, LOW);
  delay(50);
  digitalWrite(NRST, HIGH);
  delay(200);
  for ( int i = 0; i < 3; i++) {
    digitalWrite(LED, !digitalRead(LED));
    delay(100);
  }
}

void RunMode()  {    //Tested  Change to runmode
  digitalWrite(BOOT0, LOW);
  delay(100);
  digitalWrite(NRST, LOW);
  digitalWrite(LED, LOW);
  delay(50);
  digitalWrite(NRST, HIGH);
  delay(200);
  for ( int i = 0; i < 3; i++) {
    digitalWrite(LED, !digitalRead(LED));
    delay(100);
  }
}

