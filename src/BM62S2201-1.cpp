/*****************************************************************
File:          BM62S2201-1.cpp
Author:        BESTMODULES
Description:   The sensor with IIC and obtain the corresponding value
History：         
V1.0.1   -- initial version；2023-03-03；Arduino IDE :v1.8.15
******************************************************************/
#include "BM62S2201-1.h"


/**********************************************************
Description: Constructor
Parameters:: *theWire:&Wire/&Wire1/&Wire2 on BMduino
Return:      none  
Others:      none
**********************************************************/
BM62S2201_1::BM62S2201_1(TwoWire *theWire)
{
   _wire = theWire;
}
/**********************************************************
Description: Constructor
Parameters:  *theSerial:&Serial/&Serial1/&Serial2/&Serial3/&Serial4 on BMduino
Return:      none  
Others:      none
**********************************************************/
BM62S2201_1::BM62S2201_1(HardwareSerial *theSerial)
{
     _theSerial = theSerial;
}
/**********************************************************
Description: Constructor
Parameters:  rxPin : Receiver pin of the UART
             txPin : Send signal pin of UART
Return:      none  
Others:      none
**********************************************************/
BM62S2201_1::BM62S2201_1(uint8_t rxPin,uint8_t txPin)
{
    _softSerial = new SoftwareSerial(rxPin, txPin);
}
/**********************************************************
Description: Module initial
Parameters:
          addr : I2C Slave addr
Return:   void        
Others:      
**********************************************************/
void BM62S2201_1::begin(uint8_t addr)
{
  if(_wire != NULL)
  {
      _i2caddr = addr;
      _wire->begin(); 
      _wire->setClock(40000);
  }
  else if(_theSerial != NULL)
  {
    _theSerial->begin(38400); 
  }   
  else    //(_softSerial != NULL)
  {
    _softSerial->begin(38400);
  }              
}
/**********************************************************
Description: read Temperature
Parameters:  none                       
Return:      temperature data(unit: ℃)       
Others:      
**********************************************************/
float BM62S2201_1::readTemperature()
{
    float tempValue = 0;
    uint8_t sendbuf[4] = {(uint8_t)0xfa, 0x02, 0x00, 0x02};
    writeBytes(sendbuf, 4);
    delay(6);
    if(readBytes(dataBuff,6) == READ_OK)
    {
        tempValue = (int)dataBuff[4]*256 + dataBuff[3]; 
        tempValue = tempValue/10;
    }
    delay(11);
    return tempValue;
}
/**********************************************************
Description: read Pressure
Parameters:  none
Return:      Pressure data (unit: mpsi)      
Others:      
**********************************************************/
uint16_t BM62S2201_1::readPressure()
{
    int pressureValue = 0;
    uint8_t sendbuf[4] = {(uint8_t)0xfa, 0x01, 0x00, 0x01};
    writeBytes(sendbuf, 4);
    delay(6);
    if(readBytes(dataBuff,6) == READ_OK)
    {
        pressureValue = (int)dataBuff[4]*256 + dataBuff[3];
    }
    delay(11);
    return pressureValue;
}
/**********************************************************
Description: get Devce ID Number
Parameters:  devIDbuff[]:array for storing ID data
Return:      READ_OK / READ_ERROR / NO_ACK  
Others:      
**********************************************************/
uint8_t BM62S2201_1::getDevID(uint8_t devIDbuff[])
{
  uint8_t sendbuf[4] = {0xfa, 0x05, 0x00, 0x05};
  writeBytes(sendbuf, 4);
  delay(6);
  if(readBytes(dataBuff,12) == READ_OK)
  {
    for(uint8_t i = 0; i < 8; i++)
    {
      devIDbuff[i] = dataBuff[i+3];
    }
  }
  delay(17);
  return _res;
}
/**********************************************************
Description: get IIC address
Parameters:  none
Return:      Device address(1 byte)      
Others:      if you foget the IIC address you set,you can
             call this function once.
**********************************************************/
uint8_t BM62S2201_1::getI2CAddress()
{
   uint8_t temp = 0, addrValue = 0;
   int addr;
   do
   {    
       addr = temp;
       uint8_t sendbuf[4] = {(uint8_t)0xfa, 0x02, 0x00, 0x02};
       _wire->beginTransmission(addr);  
       _wire->write(sendbuf, 4);                 
       _wire->endTransmission();    
       delay(6);
       _wire->requestFrom(addr, 6);
       while(_wire->available()>0) 
       {
          dataBuff[_datacnt] = _wire->read();      
          _datacnt++;
       }
        _datacnt = 0; 
        _res = analysis();  //analysis the received data
        delay(11);
        if(temp == 0xff && _res == NO_ACK)
        {
          _res = READ_ERROR;
        }
        if(_res == NO_ACK) temp++;
        
   }while(_res == NO_ACK); //Find address by polling
   if(_res == READ_OK)
   {
       addrValue = addr;
   }
   else
   {
       addrValue = 0;
   }
   _i2caddr = addrValue;
   return addrValue;
}
/**********************************************************
Description: get Device Status
Parameters:  none 
Return:      Device status data(1 byte)      
Others:      
**********************************************************/
uint8_t BM62S2201_1::getDeviceStatus()
{
    uint8_t statusValue = 0;
    uint8_t sendbuf[4] = {(uint8_t)0xfa, 0x0A, 0x00, 0x0A};
    writeBytes(sendbuf, 4);
    delay(6);
    if(readBytes(dataBuff,5) == READ_OK)
    {
        statusValue = dataBuff[3];
    }
    delay(10);
    return statusValue;
}
/**********************************************************
Description: Request device enter to sleep mode
Parameters:  none
Return:      void
Others:   
**********************************************************/
void BM62S2201_1::enterSleepMode()
{
   uint8_t sendbuf[4] = {(uint8_t)0xfa, 0x0b, 0x00, 0x0b};
   writeBytes(sendbuf, 4);
   delay(9);
}
/**********************************************************
Description: reset
Parameters:  none
Return:      void
Others:   
**********************************************************/
void BM62S2201_1::reset()
{
   uint8_t sendbuf[4] = {(uint8_t)0xfa, 0x0c, 0x00, 0x0c};
   writeBytes(sendbuf, 4);
   delay(15);
}
/**********************************************************
Description: factory Reset
Parameters:  none
Return:      void
Others:      Restore CONFIG & I2CADR register to factory setting.
**********************************************************/
void BM62S2201_1::restoreDefault()
{
   uint8_t sendbuf[4] = {(uint8_t)0xfa, 0x0d, 0x00, 0x0d};
   writeBytes(sendbuf, 4);
   delay(9);
}
/**********************************************************
Description: Setup I2C slave address
Parameters:  addr:Variables for storing addr data(bit7-bit1)
Return:      void    
Others:      When setting , be sure to set bit0 to 0;
**********************************************************/
void BM62S2201_1::setI2CAddress(uint8_t addr)
{
    _i2caddr = addr;
    uint8_t sendbuf[5] = {(uint8_t)0xfa, 0x07, 0x01, 0,0};
    sendbuf[3] = addr;
    sendbuf[4] = sendbuf[1] + sendbuf[2] + sendbuf[3];//checksum
    writeBytes(sendbuf, 5);
    delay(10);
}

/**********************************************************
Description: write data through IIC or UART
Parameters:  buff:Variables for storing Data to be sent
             len:Length of data sent
Return:      void
Others:      
**********************************************************/
void BM62S2201_1::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
    if(_wire != NULL)
    {
        while(_wire->available() > 0)
        {
          _wire->read();
        }
        _wire->beginTransmission(_i2caddr);    //IIC start with 7bit addr
        _wire->write(wbuf, wlen);                  //send data
        _wire->endTransmission();              //IIC Stop
    }
    else if(_softSerial != NULL) 
    {
       while(_softSerial->available() > 0)
       {
         _softSerial->read();
       }
       _softSerial->write(wbuf, wlen);
    }
    else 
    {
       while(_theSerial->available() > 0)
       {
         _theSerial->read();
       }
       _theSerial->write(wbuf, wlen);
    }
}
/**********************************************************
Description: read data through IIC or UART
Parameters:  rlen:Length of data to be obtained
Return:      READ_OK/READ_ERROR/NO_ACK/TIMEOUT_ERROR
Others:      
**********************************************************/
uint8_t BM62S2201_1::readBytes(uint8_t rbuf[], uint8_t rlen, uint16_t timeout)
{
    clearBuf();
    uint8_t i = 0, delayCnt = 0;
    if(_wire != NULL)
    {
        _wire->requestFrom(_i2caddr, rlen);
        for(i = 0; i < rlen; i++)
        {
          rbuf[i] = _wire->read();
        }
    } 
    else if(_softSerial != NULL)
    {
       for (i = 0; i < rlen; i++)
       {
          delayCnt = 0;
          while (_softSerial->available() == 0)
          {
             if (delayCnt > timeout)
             {
                return TIMEOUT_ERROR; // Timeout error
             }
             delay(1);
             delayCnt++;
          }
          rbuf[i] = _softSerial->read();
       }
    }
    else
    {
       for (i = 0; i < rlen; i++)
       {
          delayCnt = 0;
          while (_theSerial->available() == 0)
          {
             if (delayCnt > timeout)
             {
                 return TIMEOUT_ERROR; // Timeout error
             }
             delay(1);
             delayCnt++;
          }
          rbuf[i] = _theSerial->read();
       }
    }
    _res = analysis();             //analysis the received data
    return _res;
}

/**********************************************************
Description: Request pressure A/D convert raw data
Parameters:  none                        
Return:      pressureRawValue:Pressure raw data   
Others:      
**********************************************************/
uint32_t BM62S2201_1::readRawPressure()
{
    uint32_t pressureRawValue = 0;
    uint8_t sendbuf[4] = {(uint8_t)0xfa, 0x03, 0x00, 0x03};
    writeBytes(sendbuf, 4);
    delay(6);
    if(readBytes(dataBuff,7) == READ_OK)
    {
        pressureRawValue = ((long)dataBuff[5]<<16)|((long)dataBuff[4]<<8)|dataBuff[3];
    }
    delay(12);
    return pressureRawValue;
}
/**********************************************************
Description: Request temperature A/D convert raw data
Parameters:  none                      
Return:      tempRawValue: Tempreture raw data       
Others:      
**********************************************************/
uint32_t BM62S2201_1::readRawTemperature()
{
    uint32_t tempRawValue = 0;
    uint8_t sendbuf[4] = {(uint8_t)0xfa, 0x04, 0x00, 0x04};
    writeBytes(sendbuf, 4);
    delay(6);
    if(readBytes(dataBuff,7) == READ_OK)
    {
        tempRawValue = ((long)dataBuff[5]<<16)|((long)dataBuff[4]<<8)|dataBuff[3];
    }
    delay(12);
    return tempRawValue;
}
/**********************************************************
Description: Data analysis
Parameters:  none    
Return:      READ_OK / READ_ERROR / NO_ACK       
Others:      
**********************************************************/
uint8_t BM62S2201_1::analysis()
{
  if(dataBuff[0] == 0xfa )          //frame header correct!
    {
      _len = dataBuff[2];            
      _checksum = dataBuff[_len +  3];
      for(uint8_t j = 1; j < (_len +3); j++)
      {
        _sum += dataBuff[j];
      }
      if(_sum == _checksum)   //checksum correct!
      {
        _sum = 0;
        _state = READ_OK;     //return READ_OK
      }
      else                     //checksum correct!
      {
        _sum = 0;
        _state = READ_ERROR;   //return READ_ERROR
      }
    }
    else   _state = NO_ACK;     //frame header error!
  switch(_state)
  {
   case READ_OK   :_state = 0;return READ_OK;   break;
   case READ_ERROR:_state = 0;return READ_ERROR;break;
   case NO_ACK    :_state = 0;return NO_ACK;    break;   
   default        :           return NO_ACK;    break;
  }
}
/**********************************************************
Description: clear Buff
Parameters:  none    
Return:      void    
Others:      
**********************************************************/
void BM62S2201_1::clearBuf()
{
  for(int a = 0; a < 12; a++)
  {
    dataBuff[a] = 0;
  } 
}
