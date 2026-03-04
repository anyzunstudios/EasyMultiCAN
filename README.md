# EasyMultiCAN
A modern, maintained revival of the classic yexiaobo-seeedstudio CAN_BUS_Shield Arduino library: cleaned up, fixed, and made reliable again.

# CAN_BUS_Shield (MCP2515) — Fixed & Extended Fork

This repository is a maintained fork of the classic **Seeed Studio / yexiaobo CAN_BUS_Shield** Arduino library for **MCP2515** CAN controllers.

The original library has been widely used for years, but many forks are effectively unmaintained and several common real-world setups (especially cheap MCP2515 modules) tend to break or behave inconsistently out of the box. This fork focuses on **correctness**, **compatibility**, and **practical features** needed for real automotive and gateway work.

---

## Origin

- **Upstream base:** [`yexiaobo-seeedstudio/CAN_BUS_Shield`]([https://example.com](https://github.com/yexiaobo-seeedstudio/CAN_BUS_Shield))
- **Hardware target:** MCP2515 CAN controller + transceiver (TJA1050, MCP2551, etc.)
- **Arduino platforms:** AVR (UNO, Mega2560) and similar SPI-capable boards

This fork was created after encountering reproducible issues when mixing different MCP2515 module variants, especially boards using **different crystal frequencies** (8 MHz vs 16 MHz), and when building multi-node SPI setups for gateway applications.

---

## Problems Found in the Original Library

### 1) Crystal frequency mismatch (8 MHz vs 16 MHz)
Many low-cost MCP2515 modules ship with an **8 MHz** crystal, while others (including many shields) use **16 MHz**.

A lot of upstream variants either:
- assume 16 MHz only, or
- don’t expose a reliable way to configure the crystal frequency

**Symptom:** modules “kind of” see traffic at lower bitrates, but frames are corrupted or ACK fails.  
**Root cause:** wrong CAN bit timing because the MCP2515 clock is wrong.

### 2) Hardcoded / awkward CS handling
Some upstream versions effectively force a single CS configuration, typically pin 10, with hacks required for Mega2560 (pin 53). This becomes painful when you need:
- custom CS pins, or
- multiple MCP2515 modules on the same SPI bus (gateway).

### 3) Poor experience using MCP2515 INT (interrupt)
Many users end up with "stuck" behavior when switching from polling (`checkReceive()`) to interrupts if they do not drain RX buffers correctly. While this is partly usage-related, the library behavior and examples did not guide users toward a safe pattern.

### 4) Real-world gateway usage not supported cleanly
A gateway often needs two MCP2515 controllers connected to the same SPI bus, each with a different CS pin, e.g.:
- CAN0: receive (16 MHz)
- CAN1: transmit (8 MHz)

The original library structure commonly pushes users into global-singleton patterns that complicate multi-controller setups.

---

## What This Fork Fixes / Adds

### ✅ Correct crystal frequency support (8 MHz and 16 MHz)
This fork adds proper MCP2515 timing configurations so you can reliably initialize both 8 MHz and 16 MHz boards:

```cpp
CAN.begin(CAN_500KBPS, MCP_8MHZ);
CAN.begin(CAN_500KBPS, MCP_16MHZ);
```

This eliminates “ghost traffic”, invalid frames, and missing ACK issues caused by wrong bit timing.

---

### ✅ Arduino Mega2560 compatibility
Mega2560 SPI behavior requires special care (hardware SS pin). This fork ensures robust behavior on Mega2560 and similar boards.

---

### ✅ Runtime CS pin selection (single-node friendly)
Arduino IDE builds library `.cpp` files separately from your `.ino`, so compile-time macros like `#define MCP2515_CS_PIN` often do **not** affect a global object created inside the library.

This fork implements a reliable runtime solution:

```cpp
CAN.setCSPin(10);   // choose CS at runtime
CAN.begin(CAN_500KBPS, MCP_8MHZ);
```

This allows a clean single-node API using the classic global `CAN` object, without forcing users to create custom instances.

---

### ✅ Multi-controller support (gateway ready)
This fork supports multiple MCP2515 modules on the **same SPI bus**, each with its own CS pin:

```cpp
#define MCP_CAN_NO_GLOBAL
#include <mcp_can.h>

MCP_CAN CAN0(10); // CS pin
MCP_CAN CAN1(9);  // CS pin
```

This is essential for gateways and multi-CAN projects.

---

### ✅ Better interrupt usage patterns (no “stuck INT”)
While the MCP2515 interrupt line behavior is hardware-defined (active-low while RX is pending), this fork’s examples and recommended patterns include safe handling:
- ISR sets a flag only
- main loop drains RX with a budget (frames/time)
- avoids starving other tasks

This prevents the common “it gets stuck when I enable interrupts” scenario.

---

## Example: Single Node (runtime CS)

```cpp
#include <SPI.h>
#include <mcp_can.h>

void setup() {
  Serial.begin(115200);

  CAN.setCSPin(10);
  if (CAN.begin(CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("CAN init OK");
  } else {
    Serial.println("CAN init FAIL");
  }
}

void loop() {}
```

---

## Example: Dual MCP2515 Gateway (CAN0 → CAN1)

```cpp
#define MCP_CAN_NO_GLOBAL
#include <SPI.h>
#include <mcp_can.h>

MCP_CAN CAN0(10); // RX side
MCP_CAN CAN1(9);  // TX side

void setup() {
  Serial.begin(115200);
  CAN0.begin(CAN_500KBPS, MCP_16MHZ);
  CAN1.begin(CAN_500KBPS, MCP_8MHZ);
}

void loop() {
  if (CAN0.checkReceive() == CAN_MSGAVAIL) {
    byte len = 0;
    byte buf[8];
    CAN0.readMsgBuf(&len, buf);
    unsigned long id = CAN0.getCanId();
    CAN1.sendMsgBuf(id, 0, len, buf);
  }
}
```

---

## Notes / Compatibility

- MCP2551 and TJA1050 transceivers are both high-speed CAN transceivers and are generally compatible electrically.
- Your CAN bitrate must match the bus.
- Always verify your MCP2515 crystal marking (commonly `8.000` or `16.000` MHz).
- For interrupt-based receive, ensure you drain RX buffers correctly; printing every frame can saturate serial output and appear like a “freeze”.

---

## Credits

- Original library and ecosystem: Seeed Studio / yexiaobo community fork
- Fixes and improvements: this repository maintains and extends the codebase for modern real-world usage.

---

## License

This project inherits the license of the upstream project. See `LICENSE` for details.
