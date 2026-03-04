#define MCP_CAN_NO_GLOBAL
#include <SPI.h>
#include <mcp_can.h>

// --- IMPORTANT: Your wiring (CS pins) ---
static const byte CAN0_CS = 10;   // CAN0 (RX side) CS pin = D10
static const byte CAN1_CS = 9;    // CAN1 (TX side) CS pin = D9

MCP_CAN CAN0(CAN0_CS);
MCP_CAN CAN1(CAN1_CS);

// --- Clocks per module crystal ---
static const byte CAN0_CLOCK = MCP_16MHZ;  // CAN0 module crystal
static const byte CAN1_CLOCK = MCP_8MHZ;   // CAN1 module crystal

// --- Bitrates (must match each CAN bus speed) ---
static const byte CAN0_BITRATE = CAN_500KBPS;
static const byte CAN1_BITRATE = CAN_500KBPS;

// --- META frame settings ---
static const unsigned long META_ID = 0x700; // 11-bit ID used only for counter/meta
static byte metaCounter = 0;

void setup() {
  Serial.begin(115200);

  if (CAN0.begin(CAN0_BITRATE, CAN0_CLOCK) != CAN_OK) {
    Serial.println("CAN0 init FAIL");
    while (1) {}
  }

  if (CAN1.begin(CAN1_BITRATE, CAN1_CLOCK) != CAN_OK) {
    Serial.println("CAN1 init FAIL");
    while (1) {}
  }

  Serial.println("Gateway ready: CAN0 -> CAN1 + META counter");
}

static void sendMeta(unsigned long origId, byte origLen) {
  byte m[8] = {0};

  // META payload format:
  // m[0] = counter (0..255)
  // m[1] = original 11-bit ID LSB
  // m[2] = original 11-bit ID MSB (only 3 bits used)
  // m[3] = original DLC
  // m[4..7] = reserved
  m[0] = metaCounter;
  m[1] = (byte)(origId & 0xFF);
  m[2] = (byte)((origId >> 8) & 0x07);
  m[3] = origLen;

  CAN1.sendMsgBuf(META_ID, 0 /* standard */, 8, m);
  metaCounter++; // wraps 255->0 automatically
}

void loop() {
  if (CAN0.checkReceive() != CAN_MSGAVAIL) return;

  byte len = 0;
  byte buf[8];
  CAN0.readMsgBuf(&len, buf);
  unsigned long id = CAN0.getCanId();

  // Forward original frame unchanged
  byte r = CAN1.sendMsgBuf(id, 0 /* standard */, len, buf);
  if (r != CAN_OK) {
    Serial.println("Forward TX FAIL");
    return;
  }

  // Send META counter frame
  sendMeta(id, len);
}
