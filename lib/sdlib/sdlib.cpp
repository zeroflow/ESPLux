#include "sdlib.h"
#include <Arduino.h>
#include <Wire.h>
#include <SD.h>

boolean hasSD = false;
boolean isRecording = false;
unsigned long recording_start;
File loggingHandle;
int fileNr = 0;

boolean SDLib_Init(void){
  if (SD.begin(SS)){
     //Serial.println("SD Card initialized.");
     hasSD = true;

     __findFileNumber();
  }else{
    //Serial.println("SD Card FAILED!");
  }

  return hasSD;
}

void __findFileNumber(){
  fileNr = 0;
  char buf[50];
  Serial.println("Find next available File");

  String name = String("/data") + String(fileNr) + String(".csv");
  name.toCharArray(buf, 50);

  while (SD.exists(buf)){
    Serial.print("Exist ");
    Serial.println(name);
    fileNr++;
    name = String("/data") + String(fileNr) + String(".csv");
    name.toCharArray(buf, 50);
  }
}

int SDLib_getFileNumber(void){
  return fileNr;
}

boolean SDLib_readCalibration(float *lux_a, float *lumen_a, float *lux_b, float *lumen_b){
  boolean success = false;

  File f = SD.open("/cal.txt", FILE_READ);
  if (!f) {
    Serial.println("Cal failed, load default");

    *lux_a = 0.0;
    *lumen_a = 0.0;
    *lux_b = 100.0;
    *lumen_b = 100.0;

  }else{
    *lux_a = f.parseFloat();
    *lumen_a = f.parseFloat();
    *lux_b = f.parseFloat();
    *lumen_b = f.parseFloat();

    Serial.println("OK");

  }
    f.close();

    return success;
}

void SDLib_switchState(void){
  isRecording = !isRecording;

  if (isRecording){
    recording_start = millis();

    __findFileNumber();
    String filename = String("/data") + String(fileNr) + String(".csv");

    loggingHandle = SD.open(filename, FILE_WRITE);
  }else{
    loggingHandle.flush();
    loggingHandle.close();

    __findFileNumber();
  }
}

boolean SDLib_getRecordingStatus(void){
  return isRecording;
}

float __calculateDuration(void){
  unsigned long dur = millis() - recording_start;
  float minutes = dur / 1000.0 / 60.0;

  return minutes;
}

/* Return elapsed duration in minutes */
float SDLib_getRecordingDuration(void){
  float duration = 0;
  if (isRecording){
    duration = __calculateDuration();
  }
  return duration;
}

void SDLib_getRecordingDurationMinutes(int* min, int* sec){
  unsigned long dur = millis() - recording_start;

  *min = dur / 60000;
  *sec = (dur / 1000) - (*min * 60);

}

boolean SDLib_logMeasurement(float lumen, float temperature){
  if (!loggingHandle){
    return false;
  }

  String timestamp = String(__calculateDuration());
  String temp = String(temperature);
  String value = String(lumen);

  String data = timestamp + ";" + value + ";" + temp;
  data.replace(".", ",");

  loggingHandle.println(data);

  return true;
}
