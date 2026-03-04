#include <SPI.h>
#include <mcp_can.h>

static const byte CS_PIN      = 10;        // Set this to your receiver's CS pin
static const byte CAN_CLOCK   = MCP_8MHZ;  // Receiver module crystal clock
static const byte CAN_BITRATE = CAN_500KBPS;

static const unsigned long META_ID = 0x700;

bool haveLast = false;
byte lastCounter = 0;

uint32_t metaRxTotal = 0;
uint32_t metaLostTotal = 0;

void setup() {
  Serial.begin(115200);

  CAN.setCSPin(CS_PIN);
  if (CAN.begin(CAN_BITRATE, CAN_CLOCK) != CAN_OK) {
    Serial.println("CAN init FAIL");
    while (1) {}
  }

  // HW filter: accept only META_ID (0x700)
  CAN.init_Mask(0, 0, 0x7FF);
  CAN.init_Filt(0, 0, META_ID);
  CAN.init_Filt(1, 0, META_ID);

  CAN.init_Mask(1, 0, 0x7FF);
  CAN.init_Filt(2, 0, META_ID);
  CAN.init_Filt(3, 0, META_ID);
  CAN.init_Filt(4, 0, META_ID);
  CAN.init_Filt(5, 0, META_ID);

  Serial.println("Receiver ready (META counter loss detection)");
}

void loop() {
  if (CAN.checkReceive() != CAN_MSGAVAIL) return;

  byte len = 0;
  byte buf[8];
  CAN.readMsgBuf(&len, buf);
  unsigned long id = CAN.getCanId();

  if (id != META_ID || len < 4) return;

  byte counter = buf[0];
  unsigned long origId = (unsigned long)buf[1] | ((unsigned long)(buf[2] & 0x07) << 8);
  byte origLen = buf[3];

  metaRxTotal++;

  if (!haveLast) {
    haveLast = true;
    lastCounter = counter;
    Serial.print("META first counter=");
    Serial.println(counter);
    return;
  }

  byte expected = (byte)(lastCounter + 1);

  if (counter != expected) {
    uint8_t lost = (uint8_t)(counter - expected); // mod 256
    metaLostTotal += lost;

    Serial.print("META LOSS: expected=");
    Serial.print(expected);
    Serial.print(" got=");
    Serial.print(counter);
    Serial.print(" lost=");
    Serial.print(lost);
    Serial.print(" metaRxTotal=");
    Serial.print(metaRxTotal);
    Serial.print(" metaLostTotal=");
    Serial.println(metaLostTotal);
  }

  lastCounter = counter;

  // Optional: show which original frame was forwarded when this META was sent
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.print("Last orig ID=0x");
    Serial.print(origId, HEX);
    Serial.print(" DLC=");
    Serial.println(origLen);
  }
}
