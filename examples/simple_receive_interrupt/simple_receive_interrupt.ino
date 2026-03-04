#include <SPI.h>
#include <mcp_can.h>

static const byte CS_PIN = 10; //Set the CAN Module SPI select pin to D10
static const byte CAN_CLOCK = MCP_8MHZ; //Set the CAN CRYSTAL CLOCK SPI to 8MHZ
volatile bool canIrq = false;
void setup() {
  Serial.begin(115200);

  CAN.setCSPin(CS_PIN); //Set the CAN Module SPI select pin to D10
  
 // init can bus, baudrate: 500k @8MHZ. Use MCP_16MHZ if your module's crystal is 16MHZ.
 // Cheap small CAN Bus Modules with TJA1050 transceiver often use 8MHZ Crystal. Bigger CAN Bus Shields with MCP2551 transceiver often use 16MHZ ones. 
 //ALWAYS check your module's crystal!
 
  if (CAN.begin(CAN_500KBPS, CAN_CLOCK) == CAN_OK) {
    Serial.println("CAN init OK, listening...");
  } else {
    Serial.println("CAN init FAIL");
    while (1) {}
  }
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), MCP2515_ISR, FALLING); // start interrupt
  
}

void MCP2515_ISR()
{
     canIrq=true;
}

void processCAN(uint8_t maxFrames) {
  if (!canIrq) return;

  uint8_t processed = 0;
    // Checks interrupt for incoming frames

  // Drain all pending frames; otherwise INT may stay low forever
  while (processed < maxFrames && (CAN.checkReceive() == CAN_MSGAVAIL)) {

    byte len = 0;
    byte buf[8];

    CAN.readMsgBuf(&len, buf);
    unsigned long id = CAN.getCanId();

    Serial.print("RX ID=0x");
    Serial.print(id, HEX);
    Serial.print(" DLC=");
    Serial.print(len);
    Serial.print(" DATA=");

    for (byte i = 0; i < len; i++) {
      if (buf[i] < 0x10) Serial.print("0");
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    processed++;
  }

  

  // Only clear the IRQ flag if there are no more frames pending
  // Keep processing next loop only if frames are still pending
  canIrq =(CAN.checkReceive() == CAN_MSGAVAIL);

  //or clear the IRQ flag after 5 loops always (less CPU load but maybe we can loose some frames)
  //canIrq = false;

}


void loop() {

   processCAN(5);
   static unsigned long lastPrint = 0;
   if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.println("Arduino is doing something else each 500ms!");
  }
    
}
