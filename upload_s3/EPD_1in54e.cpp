/*****************************************************************************
* | File      	:   EPD_1in54e.cpp
******************************************************************************/
#include "EPD_1in54e.h"
#include "Debug.h"

static void EPD_1IN54E_SendCommand(UBYTE reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

static void EPD_1IN54E_SendData(UBYTE data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

static void EPD_1IN54E_SendData2(const UBYTE *buf, uint32_t Len)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_Write_nByte((UBYTE *)buf, Len);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}


static void EPD_1IN54E_ReadBusyH(void)
{
    Debug("e-Paper busy\r\n");
	while(!DEV_Digital_Read(EPD_BUSY_PIN)) {      //LOW: busy, HIGH: idle
        DEV_Delay_ms(10);
        // Debug("e-Paper busy release\r\n");
    }
	DEV_Delay_ms(20);
    Debug("e-Paper busy release\r\n");
}

static void EPD_1IN54E_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(20);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
    EPD_1IN54E_ReadBusyH();
    DEV_Delay_ms(10);
}

static void EPD_1IN54E_TurnOnDisplay(void)
{
    EPD_1IN54E_SendCommand(0x04);
    EPD_1IN54E_ReadBusyH();
    DEV_Delay_ms(10);

    EPD_1IN54E_SendCommand(0x12);
    EPD_1IN54E_SendData(0x00);
    DEV_Delay_ms(10);
    EPD_1IN54E_ReadBusyH();

    EPD_1IN54E_SendCommand(0x02);
    EPD_1IN54E_SendData(0x00);
    EPD_1IN54E_ReadBusyH();
    DEV_Delay_ms(20);
}

void EPD_1IN54E_Init(void)
{
    EPD_1IN54E_Reset();
    EPD_1IN54E_ReadBusyH();

    EPD_1IN54E_SendCommand(0xE9);
    EPD_1IN54E_SendData(0x01);
}

void EPD_1IN54E_Clear(UBYTE color)
{
    UWORD Width, Height;
    Width = (EPD_1IN54E_WIDTH % 2 == 0)? (EPD_1IN54E_WIDTH / 2 ): (EPD_1IN54E_WIDTH / 2 + 1);
    Height = EPD_1IN54E_HEIGHT;

    EPD_1IN54E_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_1IN54E_SendData((color<<4)|color);
        }
    }

    EPD_1IN54E_TurnOnDisplay();
}

void EPD_1IN54E_Display(const UBYTE *Image)
{
    UWORD Width, Height;
    Width = (EPD_1IN54E_WIDTH % 2 == 0)? (EPD_1IN54E_WIDTH / 2 ): (EPD_1IN54E_WIDTH / 2 + 1);
    Height = EPD_1IN54E_HEIGHT;

    EPD_1IN54E_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_1IN54E_SendData(Image[i + j * Width]);
        }
    }

    EPD_1IN54E_TurnOnDisplay();
}

void EPD_1IN54E_Sleep(void)
{
    EPD_1IN54E_SendCommand(0x07);
    EPD_1IN54E_SendData(0xA5);
}
