#include "displaylib.h"

#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D
MicroOLED oled(PIN_RESET, DC_JUMPER); // Example I2C declaration

float buf[64];
int display_range = 500;

// Rate to add new values to the 64-value buffer for graphing
#define DISPLAY_RATE 5000
unsigned long last_update;

int line;

// 64x48 pixels

boolean DL_Init(void){
  oled.begin();
  oled.clear(ALL);  // Clear the display's memory (gets rid of artifacts)
  oled.clear(PAGE);
  oled.display();

  oled.setCursor(0,0);
  oled.setFontType(0);
  oled.print("LumenMeter");
  oled.display();

  for(int i=0;i<64;i++){
    buf[i] = NAN;
  }

  line++;
}

void DL_println(String text){
  if (line > 5){
    oled.clear(PAGE);
    line = 0;
  }
  oled.setCursor(0, line*8);
  oled.print(text);
  oled.display();
  line++;
}

void DL_StatusScreen(float lumen, float temperature){
  oled.clear(PAGE);

  oled.setCursor(0,0);
  oled.print((int)lumen);
  oled.print("/");
  oled.print((int)temperature);

  oled.setCursor(0, 10);
  if (SDLib_getRecordingStatus()){
    oled.print("REC ");
    int min, sec;

    SDLib_getRecordingDurationMinutes(&min, &sec);

    oled.print(min);
    oled.print(":");
    if (sec < 10){
      oled.print("0");
    }
    oled.print(sec);
  }else{
    oled.print("IDLE ");
    oled.print(SDLib_getFileNumber() -1 ); //Display last recorded file
  }

  oled.line(0, 23, 63, 23);

  if (millis() - last_update > DISPLAY_RATE){
    last_update = millis();

    bool step_up=false, step_down=true;
    for (int i=0;i<63;i++){
      buf[i] = buf[i+1];
      if (buf[i] > display_range) step_up = true;
      if (buf[i] > display_range - 500) step_down = false;
    }

    if (step_up){
      display_range += 500;
    }else if (step_down){
      display_range = max(500, display_range-500);
    }

    buf[63] = lumen;
  }

  for (int i=0;i<64;i++){
    if ( !isnan(buf[i]) ){
      int pxl_val = map(buf[i], 0, display_range, 0, 23);
      pxl_val = min(23, pxl_val);
      oled.pixel(i, 47-pxl_val);
    }
  }

  oled.display();
}
