#ifndef __UTILS_H__
#define __UTILS_H__

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/hibernate.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "inc/uart_comm.h"
#include "inc/commands.h"
#include "inc/adc_run.h"
#include "inc/i2c_comm.h"
#include "math.h"
#include "string.h"
#include "stdio.h"

#define NO_OF_BYTES_TO_ASSEMBLE                     4UL
#define SIZE_OF_NULL_CHAR							1UL

void EraseContainer(void);
void FlushContainer(void);

void EraseGPRSContainer(void);

void ThinkMeter(unsigned int delay);
void BlinkTransfer(unsigned color);
unsigned int exp10(unsigned int power);
void DecimalToHex(unsigned int value, char* result, char size);
void AsciiToHex( unsigned int data, unsigned char* val );
void HexToAscii( unsigned char data, unsigned char* val );
void PSUStatusOnline(unsigned char status);
unsigned int HexToThousand(unsigned int* buf);


#endif 
