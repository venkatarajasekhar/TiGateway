#ifndef __ADC_RUN_H__
#define __ADC_RUN_H__

extern volatile unsigned long ulTempValueC;
extern volatile unsigned long ulVoltageAvg;
extern volatile unsigned int voltage;

void ConfigureTemperatureAdcModule (void);
void ConfigureVoltageAdcModule (void);
unsigned long GetTemperature (void);
unsigned int GetVoltage (void);


#endif 
