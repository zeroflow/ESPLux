#ifndef DISPAYLIB_H
#define DISPLAYLIB_H

#include "SFE_MicroOLED.h"  // Include the SFE_MicroOLED library
#include "sdlib.h"

extern MicroOLED oled;

extern boolean DL_Init(void);
extern void DL_println(String text);
extern void DL_StatusScreen(float lumen, float temperature);

#endif
