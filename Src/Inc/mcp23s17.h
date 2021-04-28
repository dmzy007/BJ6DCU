#ifndef __MCP23S17
#define __MCP23S17

#include "stm32f10x.h"

// SPI 管脚定义
#define PORT_SPI_CS GPIOD
#define PORT_SPI_SCK GPIOD
#define PORT_SPI_MOSI GPIOD
#define PORT_SPI_MISO

#define PIN_SPI_CS GPIO_Pin_11
#define PIN_SPI_SCK GPIO_Pin_15
#define PIN_SPI_MOSI GPIO_Pin_13
#define PIN_SPI_MISO

#define SPI_CS_ENABLE GPIO_ResetBits(PORT_SPI_CS, PIN_SPI_CS)
#define SPI_CS_DISABLE GPIO_SetBits(PORT_SPI_CS, PIN_SPI_CS)

#define SPI_SCK_LOW GPIO_ResetBits(PORT_SPI_SCK, PIN_SPI_SCK)
#define SPI_SCK_HIGH GPIO_SetBits(PORT_SPI_SCK, PIN_SPI_SCK)

#define SPI_MOSI_LOW GPIO_ResetBits(PORT_SPI_MOSI, PIN_SPI_MOSI)
#define SPI_MOSI_HIGH GPIO_SetBits(PORT_SPI_MOSI, PIN_SPI_MOSI)

#define SPI_MISO_LOW
#define SPI_MISO_HIGH

// Control byte and configuration register information - Control Byte: "0100 A2 A1 A0 R/W" -- W=0
#define OPCODEW (0x40)      // Opcode for MCP23S17 with LSB (bit0) set to write (0), address OR'd in later, bits 1-3
#define OPCODER (0x41)      // Opcode for MCP23S17 with LSB (bit0) set to read (1), address OR'd in later, bits 1-3
#define ADDR_ENABLE (0x08)  // Configuration register for MCP23S17, the only thing we change is enabling hardware addressing
#define OUTPUT (0)
#define INPUT (1)
#define ON (1)
#define OFF (0)
// REGISTERS ARE DEFINED HERE SO THAT THEY MAY BE USED IN THE MAIN PROGRAM
#define IODIRA (0x00)  // MCP23x17 I/O Direction Register
#define IODIRB (0x01)  // 1 = Input (default), 0 = Output

#define IPOLA (0x02)  // MCP23x17 Input Polarity Register
#define IPOLB (0x03)  // 0 = Normal (default)(low reads as 0), 1 = Inverted (low reads as 1)

#define GPINTENA (0x04)  // MCP23x17 Interrupt on Change Pin Assignements
#define GPINTENB (0x05)  // 0 = No Interrupt on Change (default), 1 = Interrupt on Change

#define DEFVALA (0x06)  // MCP23x17 Default Compare Register for Interrupt on Change
#define DEFVALB (0x07)  // Opposite of what is here will trigger an interrupt (default = 0)

#define INTCONA (0x08)  // MCP23x17 Interrupt on Change Control Register
#define INTCONB (0x09)  // 1 = pin is compared to DEFVAL, 0 = pin is compared to previous state (default)

#define IOCON (0x0A)  // MCP23x17 Configuration Register
//                   (0x0B)      //     Also Configuration Register

#define GPPUA (0x0C)  // MCP23x17 Weak Pull-Up Resistor Register
#define GPPUB (0x0D)  // INPUT ONLY: 0 = No Internal 100k Pull-Up (default) 1 = Internal 100k Pull-Up

#define INTFA (0x0E)  // MCP23x17 Interrupt Flag Register
#define INTFB (0x0F)  // READ ONLY: 1 = This Pin Triggered the Interrupt

#define INTCAPA (0x10)  // MCP23x17 Interrupt Captured Value for Port Register
#define INTCAPB (0x11)  // READ ONLY: State of the Pin at the Time the Interrupt Occurred

#define GPPIOA (0x12)  // MCP23x17 GPIO Port Register
#define GPPIOB (0x13)  // Value on the Port - Writing Sets Bits in the Output Latch

#define OLATA (0x14)  // MCP23x17 Output Latch Register
#define OLATB (0x15)  // 1 = Latch High, 0 = Latch Low (default) Reading Returns Latch State, Not Port Value!

extern void _delay_ms(u32 us);

void MCP23S17_Configuration(void);
void SPI_WriteByte(u8 Data);
void byteWrite(u8 reg, u8 value);
void wordWrite(u8 reg, u16 word);
void pinMode(u8 pin, u8 mode);
void pinsMode(u16 mode);
void pullupMode(u8 pin, u8 mode);
void digitalWritePin(uint8_t pin, uint8_t value);
void digitalWritePins(u16 value);

void WriteDigitalTube(int num);
void WriteLED(u8 pin, u8 mode);
#endif
