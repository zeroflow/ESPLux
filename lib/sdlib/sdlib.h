#ifndef SDLIB_H
#define SDLIB_H

#include <Arduino.h>
#include <SD.h>

extern boolean hasSD;

extern boolean SDLib_Init(void);
extern boolean SDLib_readCalibration(float *lux_a, float *lumen_a, float *lux_b, float *lumen_b);
extern boolean SDLib_getRecordingStatus();
extern float SDLib_getRecordingDuration(void);
extern void SDLib_getRecordingDurationMinutes(int* min, int* sec);
extern void SDLib_switchState(void);
extern boolean SDLib_logMeasurement(float lumen, float temperature);
extern int SDLib_getFileNumber(void);

void __findFileNumber();

#endif
