/*****************************************************************
File:             BM62S2201-1.h
Author:           BESTMODULES
Description:      Define classes and required variables
History：         
V1.0.1   -- initial version；2023-03-03；Arduino IDE :v1.8.15
******************************************************************/

#ifndef _BM62S2201_1_H_
#define _BM62S2201_1_H_

#include <Wire.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

#define READ_OK          0
#define READ_ERROR       1
#define NO_ACK           2
#define TIMEOUT_ERROR    3
const uint8_t BM62S2201_1_ADDR = 0x50;
#define DISABLE         0
#define ENABLE          1 

class BM62S2201_1
{
   public:
   BM62S2201_1(TwoWire *theWire = &Wire); //HW Wire
   BM62S2201_1(HardwareSerial *theSerial); //HW Serial
   BM62S2201_1(uint8_t rxPin,uint8_t txPin); //SW Serial
   void begin(uint8_t addr = BM62S2201_1_ADDR);
   
   float readTemperature();
   uint16_t readPressure();
   
   uint8_t getDevID(uint8_t devIDbuff[]); 
   uint8_t getI2CAddress();
   uint8_t getDeviceStatus();

   void enterSleepMode();
   void reset();
   void restoreDefault();
   
   void setI2CAddress(uint8_t addr);

   private:
   void writeBytes(uint8_t wbuf[], uint8_t wlen);
   uint8_t readBytes(uint8_t rbuf[], uint8_t rlen, uint16_t timeout=10);
   
   uint32_t readRawPressure();
   uint32_t readRawTemperature();
   uint8_t  analysis();
   
   void clearBuf();
   uint8_t dataBuff[12];   //Array for storing data
   uint8_t _i2caddr;
   uint8_t _len;     //Length of data reveviced
   uint8_t _state;   
   uint8_t _sum;
   uint8_t _checksum;//Checksum of data reveviced     
   uint8_t _res;
   uint8_t _datacnt = 0;
   TwoWire *_wire = NULL;
   HardwareSerial *_theSerial = NULL; 
   SoftwareSerial *_softSerial = NULL ;
};

#endif
