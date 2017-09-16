#include "TSL2561.h"
#include "Adafruit_MLX90614.h"
#include "util.h"
#include "sdlib.h"
#include "displaylib.h"

/*

         RST         TX
         A0          RX
         D0          D1 --> SCL (green)
CLK  <-- D5          D2 <-> SDA (red)
MISO --> D6          D3
MOSI <-- D7          D4 <-- BTN
CS   <-- D8         GND
         3V3         5V



*/

#define PIN_BTN D4

TSL2561 tsl(TSL2561_ADDR_FLOAT);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float lux_a, lux_b;
float lumen_a, lumen_b;

unsigned long last_measurement, last_display;

#define MEASURE_RATE 500
#define DISPLAY_RATE 500

float lumen, temperature;

void setup(void) {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Booting...");

  pinMode(D4, INPUT_PULLUP);

  bool init;

  DL_Init();

  Serial.print("Init SD card ");
  init = SDLib_Init();
  if (init){
    Serial.println("OK");
    DL_println("SD Card OK");
  }else{
    DL_println("SD FAIL");
    Serial.println("failed");
    while (1);
  }

  SDLib_readCalibration(&lux_a, &lumen_a, &lux_b, &lumen_b);

  Serial.println("Calibration:");
  Serial.println(lux_a);
  Serial.println(lumen_a);
  Serial.println(lux_b);
  Serial.println(lumen_b);

  if (tsl.begin()) {
    Serial.println("Found sensor");
    DL_println("TSL OK");
  } else {
    Serial.println("No sensor?");
    DL_println("TSL FAIL");
    while (1) delay(100);
  }

  mlx.begin();

  //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
  //tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)

  delay(1000);

  oled.clear(PAGE);
  oled.display();
}

float readLightSensor(void){
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);

  float lm = mapf((float)x, lux_a, lux_b, lumen_a, lumen_b);

  if (lm < 0.0) lm = 0.0;



  return lm;
}

float readTempertureSensor(void){
   float temp = mlx.readObjectTempC();

   return temp;
}

void loop(void) {
  if (millis() - last_measurement > MEASURE_RATE){
    last_measurement = millis();

    lumen = readLightSensor();
    temperature = readTempertureSensor();
    Serial.print("Lux: ");
    Serial.print(lumen);

    Serial.print(" @ ");
    Serial.print(temperature);
    Serial.println("*C");

    if (SDLib_getRecordingStatus()){
      boolean status = SDLib_logMeasurement(lumen, temperature);

      if (!status){
        oled.clear(PAGE);
        DL_println("LOG ERROR");
        while(true); delay(100);
      }
    }
  }

  if (millis() - last_display > DISPLAY_RATE){
    DL_StatusScreen(lumen, temperature);
  }

  if (digitalRead(D4) == LOW){
    delay(1);
    if (digitalRead(D4) == LOW){
      while (digitalRead(D4) == LOW);

      SDLib_switchState();
    }
  }
}
