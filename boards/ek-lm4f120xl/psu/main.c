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
#include "GPRS_comm.h"
#include "scheduler.h"

volatile bool flagUartReceive = FALSE;
volatile bool flagGPRSUartReceive = FALSE;
unsigned char container[SIZE_OF_CONTAINER];
unsigned char GPRScontainer[SIZE_OF_CONTAINER];


int running_led = 0;


//*****************************************************************************
//
// Initialisation function that configures all application used peripherals
//
//*****************************************************************************
void InitSystemPeripherals(void)
{
	//
	// Enable lazy stacking for interrupt handlers.  This allows floating-point
	// instructions to be used within interrupt handlers, but at the expense of
	// extra stack usage.
	//
	ROM_FPUEnable();
	ROM_FPULazyStackingEnable();
	
	// 16 MHz system clock
	// Set the clocking to run directly from the crystal.
	//
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	
	// 40 MHz system clock
	//ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	
	//
	// Enable the peripherals.
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);	
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
		
	ConfigureTemperatureAdcModule();
	ConfigureVoltageAdcModule();
	
	// Configure PB6 as PWM output
	GPIOPinConfigure(GPIO_PB6_T0CCP0);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);	
	
	ConfigureI2CModule();

	//LCD control enable 
	GPIO_PORTD_DIR_R = 0xff;
	GPIO_PORTD_DEN_R = 0xff;
	GPIO_PORTF_DIR_R = 0xff;
	GPIO_PORTF_DEN_R = 0xff;
}

tTaskResult blinkRedLed(void)
{
    running_led = 1;
    return eTaskExecOK;
}

tTaskResult blinkBlueLed(void)
{
    running_led = 2;
    return eTaskExecOK;
}

//*****************************************************************************
//
// MAIN
//
//*****************************************************************************
int main(void)
{
    /*
    initScheduler(5);
    //Push task to task pool created in heap
    registerTask(   "blue_led_blink", 0, 
                    blinkBlueLed,
                    eTaskPrioLow,
                    0, 
                    NULL);
   
    registerTask(   "red_led_blink", 0, 
                    blinkRedLed,
                    eTaskPrioLow,
                    0, 
                    NULL);
    */
	EraseContainer();
    EraseGPRSContainer();
	
	DisablePwmPin();
	
	InitSystemPeripherals();
	
	InitDebugUart();
    InitGPRSUart();
	LcdInit();
    InitGPRSModule();
  
    
    
	
	lcdMessage(">>PSU Booted <<", "Connect to VCP!");
	
	while(1)
	{

		if(flagUartReceive)	
		{
			verifyComand(container[0]);
			flagUartReceive = FALSE;
		}else if (flagGPRSUartReceive)	
        {
            checkModemData();
            flagGPRSUartReceive = FALSE;
        }
        else
        {
            
        }
	}

}
