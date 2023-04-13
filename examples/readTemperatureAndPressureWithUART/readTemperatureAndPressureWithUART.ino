/*****************************************************************
File:        readTemperatureAndPressureWithUART
Description: Power on to obtain the Device_ID once,and then 
             repeatedly obtain the Temperature and Pressure value
             through UART and display the value in the serial port.
             Serial baud rate=38400!Cannot be changed,Otherwise, 
             the communication will fail.
******************************************************************/
#include "BM62S2201-1.h"
#define RX_PIN  2   
#define TX_PIN  3   
BM62S2201_1 Press(RX_PIN,TX_PIN);//Please uncomment out this line of code if you use SW Serial  on BMduino/Arduino
//BM62S2201_1 Press(&Serial1);   //Please uncomment out this line of code if you use HW Serial1 on BMduino
//BM62S2201_1 Press(&Serial2);   //Please uncomment out this line of code if you use HW Serial2 on BMduino
//BM62S2201_1 Press(&Serial3);   //Please uncomment out this line of code if you use HW Serial3 on BMduino
//BM62S2201_1 Press(&Serial4);   //Please uncomment out this line of code if you use HW Serial4 on BMduino

uint8_t devIDbuff[8];
 
void setup()
{
   Serial.begin(9600);
   Press.begin();
   uint8_t chk = Press.getDevID(devIDbuff);  
   if(chk != READ_OK)
   {
        switch(chk)
        {
          case READ_ERROR:Serial.println("Read sensor: CheckSum error");break;
          case NO_ACK    :Serial.println("Read sensor: NO_ACK");        break;
          default        :Serial.println("Read sensor: Unknown error"); break;
        } 
        Serial.println("Please check connection...");
   }
   else
   {
       Serial.println("Read sensor: OK"); 
       Serial.print("DeviceID : ");
       for(int i = 0; i<8 ; i++)
       {
          Serial.print(devIDbuff[i],HEX);
          Serial.print(" ");
       }
       Serial.println();
   } 
   Serial.println();
   delay(1000); 
}

void loop()
{
      Serial.print("Press:");
      Serial.print(Press.readPressure());
      Serial.print(" mpsi   ");
      Serial.print("Temp:");
      Serial.print(Press.readTemperature());
      Serial.println(" ℃");   
      delay(1000);
}
