
![EasyMultiCAN logo](CFB392C9-0A85-4970-B116-3DCB86844741.png)

# EasyMultiCAN

EasyMultiCAN is a modern, maintained and improved revival of the classic
yexiaobo / SeeedStudio CAN_BUS_Shield MCP2515 Arduino library.

The original library has been widely used for years in Arduino CAN projects,
but many forks are unmaintained and contain several long‑standing issues that
can cause unreliable behavior on modern microcontrollers and mixed MCP2515 hardware.

EasyMultiCAN focuses on correctness, reliability, and compatibility with modern
hardware while keeping the same simple Arduino‑style API used by thousands of
existing CAN projects.

---

## Why this fork exists

The original MCP2515 Arduino library is widely used but has seen limited maintenance.

Several long‑standing issues remained unresolved, including:

- incorrect transmission status checks
- undefined return behavior
- inconsistent support for MCP2515 modules using different oscillator frequencies

EasyMultiCAN was created to provide a clean, reliable and maintained version of the
library while preserving the familiar API used by the original project.

---

## Design Goals

EasyMultiCAN was designed with several practical goals in mind.

### Reliability

Ensure stable CAN communication across different MCP2515 modules and transceivers.

### Hardware compatibility

Support the most common MCP2515 hardware variants, including both
8 MHz and 16 MHz crystal modules.

### Modern microcontroller support

Ensure the library works reliably on modern platforms such as ESP32 and ESP32‑S3,
not just classic AVR boards.

### Gateway‑friendly architecture

Allow using multiple MCP2515 controllers on the same SPI bus,
enabling CAN gateway designs.

### Minimal API changes

Maintain compatibility with existing sketches written for the original library.

---

## Origin

Upstream project:
https://github.com/yexiaobo-seeedstudio/CAN_BUS_Shield

Hardware target:

- MCP2515 CAN controller
- CAN transceivers such as TJA1050, MCP2551 or VP230

This fork was created while building multi‑node automotive CAN projects and gateways,
where the original library limitations became evident.

---

## Key Improvements

### Fixed undefined return behavior

Some functions previously returned undefined values:

- sendMsgBuf()
- readMsgBuf()

This could cause unstable behavior on modern architectures like ESP32.

Both functions now return proper status codes.

---

### Correct transmission status detection

The original implementation checked the wrong register when verifying if a CAN
frame finished transmitting.

EasyMultiCAN now correctly checks the TXREQ bit in TXBnCTRL.

---

### Optimized transmission waiting

Legacy versions used delayMicroseconds() inside transmission loops.

This fork replaces those delays with micros()-based timing,
reducing blocking delays and improving behavior on fast MCUs.

---

### Improved multitasking friendliness

Internal loops were optimized to behave better on:

- fast microcontrollers
- multitasking environments
- CAN gateways handling high traffic

This makes the library much more suitable for ESP32 and similar platforms.

---

### Safer message buffer handling

Message buffers are now handled more safely:

- proper DLC validation
- full buffer clearing
- prevention of uninitialized memory reads

---

### Reliable MCP2515 clock configuration

Many MCP2515 modules use 8 MHz crystals while shields often use 16 MHz.

EasyMultiCAN supports both:

CAN.begin(CAN_500KBPS, MCP_8MHZ);
CAN.begin(CAN_500KBPS, MCP_16MHZ);

This prevents corrupted frames, missing ACKs and unstable communication.

---

## Features

- Simple Arduino‑style API
- Reliable CAN transmit and receive
- Support for 11‑bit and 29‑bit CAN identifiers
- Compatible with 8 MHz and 16 MHz MCP2515 modules
- Runtime CS pin configuration
- Support for multiple MCP2515 controllers
- Improved error handling
- Reduced blocking delays
- Better multitasking behavior
- Compatible with modern microcontrollers including ESP32

---

## Supported Hardware

### Microcontrollers (tested)

- Arduino UNO
- Arduino Nano
- Arduino Mega2560
- ESP32
- ESP32‑S3 (e.g. Seeed Studio XIAO ESP32‑S3)

### Microcontrollers (not yet verified)

Expected to work but not yet fully validated:

- RP2040
- RP2350

### CAN Controllers

- MCP2515 with 8 MHz crystal
- MCP2515 with 16 MHz crystal

### CAN Transceivers

- TJA1050
- MCP2551
- VP230

### CAN Tools

Validated using:

- CANalyst‑II CAN analyzer

Both transmission and reception were verified on real CAN networks.

---

## Runtime CS Pin Selection

Example:

CAN.setCSPin(10);
CAN.begin(CAN_500KBPS, MCP_8MHZ);

This keeps the global CAN object but allows flexible chip select configuration.

---

## Multi‑Controller Support (Gateway Ready)

Example:

#define MCP_CAN_NO_GLOBAL
#include <mcp_can.h>

MCP_CAN CAN0(10);
MCP_CAN CAN1(9);

Example gateway (CAN0 → CAN1):

#define MCP_CAN_NO_GLOBAL
#include <SPI.h>
#include <mcp_can.h>

MCP_CAN CAN0(10);
MCP_CAN CAN1(9);

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

---

## Notes

- MCP2551, TJA1050 and VP230 are high‑speed CAN transceivers and generally compatible.
- The CAN bitrate must match the bus.
- Always verify the MCP2515 crystal marking (8.000 or 16.000 MHz).
- If using interrupts, ensure RX buffers are drained properly.

---

## Credits

Original library:
Seeed Studio / yexiaobo CAN_BUS_Shield project

EasyMultiCAN improvements:
maintenance, fixes and optimizations for modern CAN projects.

---

## License

This project inherits the license of the upstream project.
See LICENSE for details.
