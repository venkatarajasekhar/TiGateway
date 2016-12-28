#ifndef __I2C_COMM_H__
#define __I2C_COMM_H__

#define DAC_SLAVE_ADRESS														0x02
#define START_THE_NEXT_TRANSMISSION_CMD							0xC1
#define STOP_THE_NEXT_TRANSMISSION_CMD							0xC0
#define DUMMY_START																	0xD0
#define NULL_CHARACTER															0x00
#define MILIVOLT_DISPLAY_SIZE												9
#define SPACE_CHAR_SIZE															1												
#define SPACE_CHAR																	0x20



void GenerateVoltage(void);
void DoDifferenceReading(unsigned int commandValue, unsigned int feedbackValue);
void I2CWriteSlaveDac(char slaveAdress, unsigned int* i2cCmdBuf, char length);
void I2CReadSlaveDac(char slaveAdress, unsigned int* i2cRespBuf, char length);
void ConfigureI2CModule(void);

#endif 
