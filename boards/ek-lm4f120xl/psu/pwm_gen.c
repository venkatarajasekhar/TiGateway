#include "inc/lm4f120h5qr.h"
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
#include "math.h"
#include "inc/lcd.h"
#include "inc/hw_i2c.h"
#include "driverlib/i2c.h"
#include "inc/i2c_comm.h"
#include "inc/utils.h"
#include "inc/pwm_gen.h"


//*****************************************************************************
//
// Function for disabling pwm output GPIO
//
//*****************************************************************************
void DisablePwmPin(void)
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);		    
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_6);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, FALSE);
}
