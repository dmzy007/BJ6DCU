#include "mcp23s17.h"

static u8  MCP23S17Address = 0;       // A0,A1,A2均为低
static u16 ModeCache       = 0xFFFF;  // Default I/O mode is all input, 0xFFFF
static u16 OutPutCache     = 0x0000;  // Default output state is all off, 0x0000
static u16 PullUpCache     = 0x0000;  // Default pull-up state is all off, 0x0000

static u8 DigitalTube = 0x00;  // default Display 0
static u8 LED         = 0xFF;  // default all led off

void MCP23S17_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    // MCP23S17 SPI
    // PD11--SS   PD15--SCK   PD13--MOSI
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11 | GPIO_Pin_15 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    SPI_CS_DISABLE;
    SPI_SCK_LOW;
    SPI_MOSI_HIGH;

    byteWrite(IOCON, ADDR_ENABLE);  //使能ICON.HAEN,ICON.BANK=0  GPIOB(高位) GPIOA(低位)
    wordWrite(IODIRA, 0);           // GPIOA和GPIOB置为输出
    wordWrite(GPPIOA, (LED << 8) + DigitalTube);
}

void SPI_WriteByte(u8 Data)
{
    u8 i;

    // SPI_CS_ENABLE;
    // _delay_ms(1);

    for (i = 0; i < 8; i++)
    {
        SPI_SCK_LOW;
        _delay_ms(1);

        if (Data & 0x80)
        {
            SPI_MOSI_HIGH;
        }
        else
        {
            SPI_MOSI_LOW;
        }
        _delay_ms(1);

        SPI_SCK_HIGH;
        _delay_ms(1);

        Data = Data << 1;
    }

    // SPI_SCK_LOW;
    // _delay_ms(1);
    // SPI_CS_DISABLE;
    // _delay_ms(1);
}

// GENERIC BYTE WRITE - will write a byte to a register, arguments are register address and the value to write
void byteWrite(u8 reg, u8 value)
{
    SPI_CS_ENABLE;
    _delay_ms(1);

    // Accept the register and byte
    SPI_WriteByte((OPCODEW) | (MCP23S17Address << 1));  // Send the MCP23S17 opcode, chip address, and write bit
    SPI_WriteByte(reg);                                 // Send the register we want to write
    SPI_WriteByte(value);                               // Send the byte

    SPI_CS_DISABLE;
    _delay_ms(1);
}

// GENERIC WORD WRITE - will write a word to a register pair, LSB to first register, MSB to next higher value register
void wordWrite(u8 reg, u16 word)
{
    SPI_CS_ENABLE;
    _delay_ms(1);

    // Accept the start register and word
    SPI_WriteByte(OPCODEW | (MCP23S17Address << 1));  // Send the MCP23S17 opcode, chip address, and write bit
    SPI_WriteByte(reg);                               // Send the register we want to write
    SPI_WriteByte((u8)(word));                        // Send the low byte (register address pointer will auto-increment after write)
    SPI_WriteByte((u8)(word >> 8));                   // Shift the high byte down to the low byte location and send

    SPI_CS_DISABLE;
    _delay_ms(1);
}

// MODE SETTING FUNCTIONS - BY PIN AND BY WORD
void pinMode(u8 pin, u8 mode)
{
    if (pin<1 | pin> 16) return;  // If the pin value is not valid (1-16) return, do nothing and return
    if (mode == INPUT)
    {                                 // Determine the mode before changing the bit state in the mode cache
        ModeCache |= 1 << (pin - 1);  // Since input = "HIGH", OR in a 1 in the appropriate place
    }
    else
    {
        ModeCache &= ~(1 << (pin - 1));  // If not, the mode must be output, so and in a 0 in the appropriate place
    }
    wordWrite(IODIRA, ModeCache);  // Call the generic word writer with start register and the mode cache
}

void pinsMode(u16 mode)
{                             // Accept the word?
    wordWrite(IODIRA, mode);  // Call the the generic word writer with start register and the mode cache

    ModeCache = mode;
}
// THE FOLLOWING WRITE FUNCTIONS ARE NEARLY IDENTICAL TO THE FIRST AND ARE NOT INDIVIDUALLY COMMENTED
// WEAK PULL-UP SETTING FUNCTIONS - BY WORD AND BY PIN
void pullupMode(u8 pin, u8 mode)
{
    if (pin<1 | pin> 16) return;
    if (mode == ON)
    {
        PullUpCache |= 1 << (pin - 1);
    }
    else
    {
        PullUpCache &= ~(1 << (pin - 1));
    }
    wordWrite(GPPUA, PullUpCache);
}

void pullupModes(u16 mode)
{
    wordWrite(GPPUA, mode);

    PullUpCache = mode;
}
// WRITE FUNCTIONS - BY WORD AND BY PIN
void digitalWritePin(uint8_t pin, uint8_t value)
{
    if (pin<1 | pin> 16) return;
    if (value)
    {
        OutPutCache |= 1 << (pin - 1);
    }
    else
    {
        OutPutCache &= ~(1 << (pin - 1));
    }
    wordWrite(GPPIOA, OutPutCache);
}

void digitalWritePins(u16 value)
{
    OutPutCache = value;
    wordWrite(GPPIOA, OutPutCache);
}

void WriteDigitalTube(int num)
{
    u8 DigitalTubeTable[] = {
        0x3F,  //"0"
        0x06,  //"1"
        0x5B,  //"2"
        0x4F,  //"3"
        0x66,  //"4"
        0x6D,  //"5"
        0x7D,  //"6"
        0x07,  //"7"
        0x7F,  //"8"
        0x6F,  //"9"
        0x77,  //"A"
        0x7C,  //"B"
        0x39,  //"C"
        0x5E,  //"D"
        0x79,  //"E"
        0x71,  //"F"
        0x76,  //"H"
        0x38,  //"L"
        0x37,  //"n"
        0x3E,  //"u"
        0x73,  //"P"
        0x5C,  //"o"
        0x40,  //"-"
        0x00   //熄灭
    };

    if (num < 24)
    {
        DigitalTube = DigitalTubeTable[num];

        wordWrite(GPPIOA, (LED << 8) + DigitalTube);
    }
}

/*
** LED1 ---  自动按键LED
** LED2 ---  对讲按键LED
** LED3 ---  报警按键LED
** LED4 ---  监听按键LED
** LED5 ---  主控按键LED
** LED6 ---  静音按键LED
** LED7 ---  广播LED
*/
void WriteLED(u8 pin, u8 mode)
{
    if (pin<1 | pin> 8) return;
    if (mode == ON)
    {
        LED &= ~(1 << (pin - 1));
    }
    else
    {
        LED |= 1 << (pin - 1);
    }

    wordWrite(GPPIOA, (LED << 8) + DigitalTube);
}
