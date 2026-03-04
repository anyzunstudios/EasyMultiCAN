#define MCP_CAN_NO_GLOBAL   // Disable the global CAN object (we will create CAN0 and CAN1)
#include <SPI.h>
#include <mcp_can.h>

MCP_CAN CAN0(10);   // CS for module 0
MCP_CAN CAN1(9);  // CS for module 1

static const byte CAN0_CLOCK = MCP_16MHZ; // Adjust per module crystal
static const byte CAN1_CLOCK = MCP_8MHZ;  // Example: second module is 8MHz

void setup() {
  Serial.begin(115200);

  if (CAN0.begin(CAN_500KBPS, CAN0_CLOCK) != CAN_OK) {
    Serial.println("CAN0 init FAIL");
    while (1) {}
  }

  if (CAN1.begin(CAN_500KBPS, CAN1_CLOCK) != CAN_OK) {
    Serial.println("CAN1 init FAIL");
    while (1) {}
  }

  Serial.println("Gateway running: CAN0 -> CAN1");
}

void loop() {
  if (CAN0.checkReceive() == CAN_MSGAVAIL) {
    byte len = 0;
    byte buf[8];

    CAN0.readMsgBuf(&len, buf);
    unsigned long id = CAN0.getCanId();

    // Forward as standard 11-bit frame (ext=0). If you use extended IDs, set ext=1 consistently.
    byte r = CAN1.sendMsgBuf(id, 0 /* ext=0 */, len, buf);

    if (r != CAN_OK) {
      Serial.println("Forward TX FAIL");
    }
  }
}
