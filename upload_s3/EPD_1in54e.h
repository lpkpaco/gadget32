/*****************************************************************************
* | File      	:   EPD_1IN54E.h
******************************************************************************/
#ifndef __EPD_1IN54E_H_
#define __EPD_1IN54E_H_

#include "DEV_Config.h"

#define EPD_1IN54E_WIDTH        240
#define EPD_1IN54E_HEIGHT       240
#define EPD_1IN54E_FRAME_BYTES  ((EPD_1IN54E_WIDTH * EPD_1IN54E_HEIGHT) / 2)

#define EPD_1IN54E_BLACK        0x0
#define EPD_1IN54E_WHITE        0x1
#define EPD_1IN54E_YELLOW       0x2
#define EPD_1IN54E_RED          0x3
#define EPD_1IN54E_BLUE         0x5
#define EPD_1IN54E_GREEN        0x6

void EPD_1IN54E_Init(void);
void EPD_1IN54E_Clear(UBYTE color);
void EPD_1IN54E_Display(const UBYTE *Image);
void EPD_1IN54E_Sleep(void);

#endif
