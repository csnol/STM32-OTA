#include "stm32ota.h"

#define TIMEOUT 1000  // Set a timeout for reading from Serial

void stm32SendCommand(unsigned char commd) {
    Serial.write(commd);
    Serial.write(~commd);
}

unsigned char waitForSerialData(unsigned long timeout) {
    unsigned long startTime = millis();
    while (!Serial.available()) {
        if (millis() - startTime > timeout) {
            return STM32ERR;  // Timeout error
        }
    }
    return Serial.read();
}

unsigned char stm32Erase() {
    stm32SendCommand(STM32ERASE);
    return (waitForSerialData(TIMEOUT) == STM32ACK) ? STM32ACK : STM32ERR;
}

// Similar modifications for stm32Erasen, stm32Run, stm32Read and other functions ...

unsigned char stm32Address(unsigned long addr) {
    unsigned char sendaddr[4];
    sendaddr[0] = addr >> 24;
    sendaddr[1] = (addr >> 16) & 0xFF;
    sendaddr[2] = (addr >> 8) & 0xFF;
    sendaddr[3] = addr & 0xFF;
    unsigned char addcheck = sendaddr[0] ^ sendaddr[1] ^ sendaddr[2] ^ sendaddr[3];
    Serial.write(sendaddr, 4);
    Serial.write(addcheck);
    return waitForSerialData(TIMEOUT);
}

unsigned char stm32SendData(unsigned char * data, unsigned char wrlen) {
    Serial.write(wrlen);
    for (int i = 0; i <= wrlen; i++) {
        Serial.write(data[i]);
    }
    Serial.write(getChecksum(data, wrlen));
    return waitForSerialData(TIMEOUT);
}

char stm32Version() {
    unsigned char vsbuf[14];
    stm32SendCommand(STM32GET);
    int len = Serial.readBytesUntil(STM32ACK, vsbuf, 14);
    return (len > 1) ? vsbuf[1] : STM32ERR;
}

unsigned char stm32GetId() {
    unsigned char sbuf[5];
    stm32SendCommand(STM32ID);
    if (waitForSerialData(TIMEOUT) == STM32ACK) {
        int len = Serial.readBytesUntil(STM32ACK, sbuf, 4);
        if (len < 3) {
            return STM32ERR;  // Error reading ID
        }
        int getid = (sbuf[1] << 8) + sbuf[2];
        switch (getid) {
            case 0x444: return 1;
            case 0x440: return 2;
            case 0x442: return 3;
            case 0x412: return 4;
            case 0x410: return 5;
            case 0x414: return 6;
            case 0x418: return 7;
            default: return 0;  // Unknown ID
        }
    } else {
        return STM32ERR;
    }
}

unsigned char getChecksum(unsigned char * data, unsigned char datalen) {
    unsigned char lendata = datalen;
    for (int i = 0; i <= datalen; i++)
        lendata ^= data[i];
    return lendata;
}

