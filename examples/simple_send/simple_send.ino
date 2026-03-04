// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>
// Pick ANY CS pin you want here
static const byte CAN_CS_PIN = 10; //SPI Select PIN D10
                                   //By default on all Arduinos (UNO, Nano, Micro...) is D10
                                   //But in Arduino MEGA2560 is Pin D53, but can be changed to D10 or any pin if you want



void setup()
{
  Serial.begin(115200);
  CAN.setCSPin(CAN_CS_PIN); //Set the CAN Module SPI select pin to D10
  
 // init can bus, baudrate: 500k @8MHZ. Use MCP_16MHZ if your module's crystal is 16MHZ.
 // Cheap small CAN Bus Modules with TJA1050 transceiver often use 8MHZ Crystal. Bigger CAN Bus Shields with MCP2551 transceiver often use 16MHZ ones. 
 //ALWAYS check your module's crystal!
  if(CAN.begin(CAN_500KBPS, MCP_8MHZ) ==CAN_OK) Serial.print("can init ok!!\r\n");
  else Serial.print("Can init fail!!\r\n");
}

unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};
void loop()
{
  // send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
  CAN.sendMsgBuf(0x00, 0, 8, stmp);  
  delay(100);
}
