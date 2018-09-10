
#include "stm32ota.h"



void stm32SendCommand(unsigned char commd) {    // Tested
  Serial.write(commd);
  Serial.write(~commd);
}

unsigned char stm32Erase() {     // Tested
  stm32SendCommand(STM32ERASE);
  while (!Serial.available());
  if (Serial.read() == STM32ACK)
  {
    Serial.write(0xFF);
    Serial.write(0x00);
  }
  else return STM32ERR;
  while (!Serial.available());
  return Serial.read();
}

unsigned char stm32Erasen() {     // Tested
  stm32SendCommand(STM32ERASEN);
  while (!Serial.available());
  if (Serial.read() == STM32ACK)
  {
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0x00);
  }
  else return STM32ERR;
  while (!Serial.available());
  return Serial.read();
}

// No test yet
unsigned char stm32Run()   {
  stm32SendCommand(STM32RUN);
  while (!Serial.available());
  if (Serial.read() == STM32ACK) {
    stm32Address(STM32STADDR);
    return STM32ACK;
  }
  else
    return STM32ERR;
}

// No test yet
unsigned char stm32Read(unsigned char * rdbuf, unsigned long rdaddress, unsigned char rdlen) {
  stm32SendCommand(STM32RD);
  while (!Serial.available());
  if (Serial.read() == STM32ACK)
    stm32Address(rdaddress);
  else return STM32ERR;
  while (!Serial.available());
  if (Serial.read() == STM32ACK)
    stm32SendCommand(rdlen);
  while (!Serial.available());
  size_t getlen = Serial.available();
  Serial.readBytes(rdbuf, getlen);
  return STM32ACK;
}

unsigned char stm32Address(unsigned long addr) {    // Tested
  unsigned char sendaddr[4];
  unsigned char addcheck = 0;
  sendaddr[0] = addr >> 24;
  sendaddr[1] = (addr >> 16) & 0xFF;
  sendaddr[2] = (addr >> 8) & 0xFF;
  sendaddr[3] = addr & 0xFF;
  for (int i = 0; i <= 3; i++) {
    Serial.write(sendaddr[i]);
    addcheck ^= sendaddr[i];
  }
  Serial.write(addcheck);
  while (!Serial.available());
  return Serial.read();
}

unsigned char stm32SendData(unsigned char * data, unsigned char wrlen) {     // Tested
  Serial.write(wrlen);
  for (int i = 0; i <= wrlen; i++) {
    Serial.write(data[i]);
  }
  Serial.write(getChecksum(data, wrlen));
  while (!Serial.available());
  return Serial.read();
}

char stm32Version() {     // Tested
  unsigned char vsbuf[14];
  stm32SendCommand(STM32GET);
  while (!Serial.available());
  vsbuf[0] = Serial.read();
  if (vsbuf[0] != STM32ACK)
    return STM32ERR;
  else {
    Serial.readBytesUntil(STM32ACK, vsbuf, 14);
    return vsbuf[1];
  }
}

unsigned char stm32GetId() {     // Tested
  int getid = 0;
  unsigned char sbuf[5];
  stm32SendCommand(STM32ID);
  while (!Serial.available());
  sbuf[0] = Serial.read();
  if (sbuf[0] == STM32ACK) {
    Serial.readBytesUntil(STM32ACK, sbuf, 4);
    getid = sbuf[1];
    getid = (getid << 8) + sbuf[2];
    if (getid == 0x440)
      return 1;
    if (getid == 0x444)
      return 2;
    if (getid == 0x442)
      return 3;
    if (getid == 0x445)
      return 4;
    if (getid == 0x448)
      return 5;
    if (getid == 0x442)
      return 6;
    if (getid == 0x412)
      return 7;
    if (getid == 0x410)
      return 8;
    if (getid == 0x414)
      return 9;
    if (getid == 0x420)
      return 10;
    if (getid == 0x428)
      return 11;
    if (getid == 0x418)
      return 12;
    if (getid == 0x430)
      return 13;
    if (getid == 0x411)
      return 14;
    if (getid == 0x432)
      return 15;
    if (getid == 0x422)
      return 16;
    if (getid == 0x439)
      return 17;
    if (getid == 0x438)
      return 18;
    if (getid == 0x446)
      return 19;
    if (getid == 0x413)
      return 20;
    if (getid == 0x419)
      return 21;
    if (getid == 0x423)
      return 22;
    if (getid == 0x433)
      return 23;
    if (getid == 0x458)
      return 24;
    if (getid == 0x431)
      return 25;
    if (getid == 0x441)
      return 26;
    if (getid == 0x421)
      return 27;
    if (getid == 0x434)
      return 28;
    if (getid == 0x463)
      return 29;
    if (getid == 0x452)
      return 30;
    if (getid == 0x449)
      return 31;
    if (getid == 0x451)
      return 32;
    if (getid == 0x450)
      return 33;
    if (getid == 0x457)
      return 34;
    if (getid == 0x425)
      return 35;
    if (getid == 0x417)
      return 36;
    if (getid == 0x447)
      return 37;
    if (getid == 0x416)
      return 38;
    if (getid == 0x429)
      return 39;
    if (getid == 0x427)
      return 40;
    if (getid == 0x436)
      return 41;
    if (getid == 0x437)
      return 42;
    if (getid == 0x435)
      return 43;
    if (getid == 0x462)
      return 44;
    if (getid == 0x415)
      return 45;
    if (getid == 0x461)
      return 46;
  }
  else
    return 0;
}

unsigned char getChecksum( unsigned char * data, unsigned char datalen) {    // Tested
  unsigned char lendata = datalen;
  for (int i = 0; i <= datalen; i++)
    lendata ^= data[i];
  return lendata;
}



