#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#define UNUSED(X)									    X = X

#define NOT_SUPPORTED								    11UL
#define DELAY_BETWEEN_PSU_OPERATIONS		            20UL
#define SIZEOFPWMFIELD									5UL	// This is the number of bytes used as input
#define SIZEOFI2CFIELD									4UL	// This is the number of bytes used as input

#define SIZEOFI2CCOMMAND								1UL
#define OFFSETCMDPWM								    1UL
#define OFFSETCMDI2C									2UL
#define SIZE_OF_I2C_CMD									6UL

extern void FillStructurePWM(unsigned long* period, unsigned long* dutyCycle);
extern void FillStructureI2C(unsigned int i2cBufLocation[], unsigned int* reservedValue, unsigned int size);

extern volatile unsigned char DISABLE_PSU_FLAG;
extern volatile unsigned char NOT_WRONG_INFO;
extern volatile unsigned char CRITICAL;
extern volatile unsigned char COMMAND_NOT_OK;

char verifyComand(char cmd);
void DisablePwmPin(void);
void GeneratePWM(void);
void DisablePeripherals(void);
void InitSystemPeripherals(void);
void EmergencyStop(void);
void SendCommandInformation(void);
void GetAdcVoltage(void);
void StartTimer1A(void* ReloadValue);
void StartTimer2A(void* ReloadValue);
void StartTimer3A(void* ReloadValue);

#endif
