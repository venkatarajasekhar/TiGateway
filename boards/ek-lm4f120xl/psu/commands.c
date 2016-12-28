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
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "inc/timer_operations.h"
#include "driverlib/hibernate.h"
#include "inc/uart_comm.h"
#include "inc/commands.h"
#include "inc/adc_run.h"
#include "inc/lcd.h"
#include "inc/i2c_comm.h"
#include "inc/utils.h"
#include "inc/pwm_gen.h"

volatile unsigned char DISABLE_PSU_FLAG;
volatile unsigned char NOT_WRONG_INFO = TRUE;
volatile unsigned char COMMAND_NOT_OK = FALSE;
volatile unsigned char CRITICAL;

//*****************************************************************************
//
// Structure used by user to access pwm command parameters
//
//*****************************************************************************
typedef struct	CmdPWM
{
	unsigned char containerCmd;
	unsigned int PeriodValue[SIZEOFPWMFIELD];
	unsigned int DutyCycleValue[SIZEOFPWMFIELD];
	unsigned int NumericValue[SIZEOFPWMFIELD];
}	AssembledCommandPWM;

AssembledCommandPWM	PWMCommand;

//*****************************************************************************
//
// Structure used by user to access I2C command parameters
//
//*****************************************************************************
typedef struct	CmdI2C
{
	unsigned char containerCmd;
	unsigned char I2CCmdType;	
	unsigned int VoltageValue[SIZEOFI2CFIELD];
	unsigned int ReservedValue[SIZEOFI2CFIELD];
}	AssembledCommandI2C;

AssembledCommandI2C	I2CCommand;

//*****************************************************************************
//
// MCU reboot rutine
//
//*****************************************************************************
int CMD_reboot (void)
{
	SysCtlReset();
	SysCtlDelay(SysCtlClockGet());
	
	return 0;
}

//*****************************************************************************
//
// MCU hibernate routine
//
//*****************************************************************************
void SystemHybernate(void)
{
	PSUStatusOnline(BLUE_LED);

	HibernateEnableExpClk(SysCtlClockGet());
	HibernateGPIORetentionEnable();
	SysCtlDelay(64000000);
	HibernateWakeSet(HIBERNATE_WAKE_PIN);
	PSUStatusOnline(FALSE);
	HibernateRequest();

	//stay here and wait for the wakeup pin event
	while(1)
	{
	}
}

//*****************************************************************************
//
// Stop all timers function
//
//*****************************************************************************
void DisableAllTimers(void)
{
	TimerDisable(TIMER1_BASE, TIMER_A);
	TimerDisable(TIMER2_BASE, TIMER_A);
	TimerDisable(TIMER3_BASE, TIMER_A);	
}



//*****************************************************************************
//
// Function for disabling the output stage of PSU
//
//*****************************************************************************
void DisableStagePin(void)
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);		    
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, FALSE);
}

//*****************************************************************************
//
// Function for handling emergency stop command
//
//*****************************************************************************
void EmergencyStop(void)
{
	if(!CRITICAL)
	{
		DisablePwmPin();
		PSUStatusOnline(FALSE);
	}
	else
	{
		DisablePwmPin();		
		UARTSendNoFIFOLimitation( (unsigned char*) "!!Emergency STOP Triggered!! Reboot board to enable!\n", 54);
		PSUStatusOnline(FALSE);
		while(1)
			;
	}
}

//*****************************************************************************
//
//Dummy function that blinks the status led with timer syncronisation
//
//*****************************************************************************
void BlinkWithTimer(void)
{
	StartTimer1A((unsigned long*)SysCtlClockGet());
	//StartTimer2A((unsigned long*)SysCtlClockGet());
	StartTimer3A((unsigned long*)SysCtlClockGet());
}

//*****************************************************************************
//
//Fuction used to generate a cofigurable PWM signal (duty cycle, period) 
//
//*****************************************************************************
void GeneratePWM(void)
{		
	unsigned long ulPeriod, dutyCycle;
	FillStructurePWM(&ulPeriod, &dutyCycle);
	
	if(!DISABLE_PSU_FLAG) 
	{
		UARTSendNoFIFOLimitation( (unsigned char*) "\nParameters accepted SUCCESS!\n", 32);
		// Configure timer
		SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
		TimerConfigure(TIMER0_BASE, (TIMER_CFG_SPLIT_PAIR |TIMER_CFG_A_PWM));
		TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod - 1);
		TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycle); 				// PWM
		TimerEnable(TIMER0_BASE, TIMER_A);
		
		// Configure PB6 as T0CCP0
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		GPIOPinConfigure(GPIO_PB6_T0CCP0);
		GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
	}
	else
	{
		EmergencyStop();
	}
}

void GetAdcVoltage (void)
{
	volatile unsigned long var1=40000;	
	volatile unsigned long reference, sample;
	volatile unsigned long IncreasingLargeOffset;
	volatile unsigned long DecreasingLargeOffset;
	volatile unsigned long IncreasingSmallOffset;
	volatile unsigned long DecreasingSmallOffset;
	
	reference = 2100;
	IncreasingLargeOffset = reference - 400;
	DecreasingLargeOffset = reference + 400;
	IncreasingSmallOffset = reference - 200;	
	DecreasingSmallOffset = reference + 200;

	
	while ( GetVoltage() != reference )
	{				
		if((GetVoltage() < IncreasingSmallOffset) && (GetVoltage() < IncreasingLargeOffset) && (GetVoltage() < reference))
		{
			var1=var1-250;
			TimerDisable(TIMER0_BASE, TIMER_A);
			// Configure timer
			SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
			TimerConfigure(TIMER0_BASE, (TIMER_CFG_SPLIT_PAIR |TIMER_CFG_A_PWM));
			TimerLoadSet(TIMER0_BASE, TIMER_A, 39999);
			TimerMatchSet(TIMER0_BASE, TIMER_A, var1); 				// PWM
			TimerEnable(TIMER0_BASE, TIMER_A);
			// Configure PB6 as T0CCP0
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
			GPIOPinConfigure(GPIO_PB6_T0CCP0);
			GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);	
			SysCtlDelay(SysCtlClockGet()/30);		
			//sample = GetVoltage();	

		}else
			if((GetVoltage() > IncreasingSmallOffset) && (GetVoltage() > IncreasingLargeOffset) && (GetVoltage() <= reference))
				{
					var1=var1-100;
					TimerDisable(TIMER0_BASE, TIMER_A);
					// Configure timer
					SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
					TimerConfigure(TIMER0_BASE, (TIMER_CFG_SPLIT_PAIR |TIMER_CFG_A_PWM));
					TimerLoadSet(TIMER0_BASE, TIMER_A, 39999);
					TimerMatchSet(TIMER0_BASE, TIMER_A, var1); 				// PWM
					TimerEnable(TIMER0_BASE, TIMER_A);		
					// Configure PB6 as T0CCP0
					SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
					GPIOPinConfigure(GPIO_PB6_T0CCP0);
					GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
					SysCtlDelay(SysCtlClockGet()/10);			
				//	sample = GetVoltage();	
				}
						

	}
}

//*****************************************************************************
//
// Function that transfers speciffic general command data to the PWM structure 
//
//*****************************************************************************
void FillStructurePWM(unsigned long* period, unsigned long* dutyCycle)
{
	unsigned char index1, index2, numeric; 
	
	PWMCommand.containerCmd = container[0];
	
	for(index1 = 0; index1 < SIZEOFPWMFIELD; index1++)
	{
		AsciiToHex(container[index1 + OFFSETCMDPWM], &numeric) ;
		PWMCommand.PeriodValue[index1] = numeric;
		
		AsciiToHex(container[index1 + SIZEOFPWMFIELD + OFFSETCMDPWM], &numeric) ;		
		PWMCommand.DutyCycleValue[index1] = numeric;
	}
			
	for (index2 = 0; index2 < SIZE_OF_CONTAINER -(2 *SIZEOFPWMFIELD + OFFSETCMDPWM); index2++ )
	{
		AsciiToHex(container[index2 + 2*SIZEOFPWMFIELD + OFFSETCMDPWM], &numeric) ;	
		PWMCommand.NumericValue[index2] = numeric;
	}

	*period = exp10(4) * PWMCommand.PeriodValue[0] + exp10(3) * PWMCommand.PeriodValue[1] + exp10(2) * PWMCommand.PeriodValue[2] + exp10(1) * PWMCommand.PeriodValue[3] + exp10(0) * PWMCommand.PeriodValue[4];
	*dutyCycle = exp10(4) * PWMCommand.DutyCycleValue[0] + exp10(3) * PWMCommand.DutyCycleValue[1] + exp10(2) * PWMCommand.DutyCycleValue[2] + exp10(1) * PWMCommand.DutyCycleValue[3] + exp10(0) * PWMCommand.DutyCycleValue[4];

}

//*****************************************************************************
//
// Function that transfers speciffic general command data to the I2C structure
//
//*****************************************************************************
void FillStructureI2C(unsigned int i2cBufArg[], unsigned int* reservedValue, unsigned int size)
{
	unsigned char index, index1, numeric; 
	unsigned int i2cBuffer[SIZEOFI2CFIELD + SIZEOFI2CCOMMAND];
	
	I2CCommand.containerCmd = container[0];
	I2CCommand.I2CCmdType = container[1];
	
	for(index = 0; index < SIZEOFI2CFIELD; index++)
	{
		AsciiToHex(container[index + OFFSETCMDI2C], &numeric) ;
		I2CCommand.VoltageValue[index] = numeric;
		
		AsciiToHex(container[index + SIZEOFI2CFIELD + OFFSETCMDI2C], &numeric) ;		
		I2CCommand.ReservedValue[index] = numeric;
		
		i2cBuffer[index + 1] = I2CCommand.VoltageValue[index];
	}
	
	AsciiToHex(container[1], &numeric) ;
	i2cBuffer[0] = numeric;
	
	for(index1 = 0; index1 < size; index1++)
	{
		i2cBufArg[index1] = i2cBuffer[index1];
	}

	*reservedValue = exp10(3) * I2CCommand.ReservedValue[0] + exp10(2) * I2CCommand.ReservedValue[1] + exp10(1) * I2CCommand.ReservedValue[2] + exp10(0) * I2CCommand.ReservedValue[3];
}

//*****************************************************************************
//
// We are sending the command information to the user terminal
//
//*****************************************************************************
void SendCommandInformation(void)
{

	UARTSendNoFIFOLimitation( (unsigned char*) "\nDigital PSU version 0.1 CopyRight CND\n", 39);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "Following is the command list for your input: \n", 48);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press L for manufacturing information \n", 42);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press H to enter hibernation mode\n", 37);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press R to reboot system\n", 28);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press D to disable output mode\n", 34);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press A to activate output stage\n", 36);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press E following parameters to generate pwm output!\n", 56);	
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press T for timers start\n", 28);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press Y for timers stop\n", 27);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press S for immediate STOP\n", 30);
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press C to start ADC read\n", 29);	
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press I to set DAC voltage\n", 30);	
	SysCtlDelay(SysCtlClockGet() / (5 * 3));
	UARTSendNoFIFOLimitation( (unsigned char*) "> Press P to disable DAC voltage\n", 34);	
	SysCtlDelay(SysCtlClockGet() / (5 * 3));	
}

//*****************************************************************************
//
// Function that conpares the input from the keyboard to a predefined entry 
//
//*****************************************************************************
char verifyComand(char cmd)
{
		ThinkMeter(DELAY_BETWEEN_PSU_OPERATIONS);

		switch (cmd)
		{	
			case 'a': 
			{
				DISABLE_PSU_FLAG = FALSE;				
				UARTSendNoFIFOLimitation( (unsigned char*) "\nWarning! Output stage ENABLED\r\n", 33); 
				PSUStatusOnline(RED_LED);
				break;
			}			
			case 'l': 
			{
				UARTSendNoFIFOLimitation( (unsigned char*) "\nPSU made with LM317\r\n", 23); 
				//lcdMessage(">>PSU Booting <<", "Connect to VCP !");
				lcdMessage("PSU made with ", "LM317!");
				break;
			}
			case 'r': 
			{
				UARTSendNoFIFOLimitation( (unsigned char*) "\nRebooting control unit!\r\n", 27);
				CMD_reboot();
				EraseContainer();
				break;
			}
			case 'e': 
			{
				DISABLE_PSU_FLAG = FALSE;
				if(NOT_WRONG_INFO)
				UARTSendNoFIFOLimitation( (unsigned char*) "\nTrying to modify output config!\r\n", 35);		
				GeneratePWM();
				EraseContainer();
				break;
			}
			case 'd': 
			{
				CRITICAL = FALSE;
				DISABLE_PSU_FLAG = TRUE;
				UARTSendNoFIFOLimitation( (unsigned char*) "\nWarning! Output DISABLED!\r\n", 29);
				GeneratePWM();
				EraseContainer();
				break;
			}			
			case 'm': 
			{
				SendCommandInformation();
				break;
			}
			case 's': 
			{
				CRITICAL= TRUE;
				DISABLE_PSU_FLAG = TRUE;
				EmergencyStop();
				break;
			}
			case 't': 
			{
				UARTSendNoFIFOLimitation( (unsigned char*) "\nTimers enabled\r\n", 18);
				BlinkWithTimer();
				break;
			}
			case 'y': 
			{
				UARTSendNoFIFOLimitation( (unsigned char*) "\nTimers disabled\r\n", 18);
				DisableAllTimers();
				break;
			}			
			case 'h': 
			{
				UARTSendNoFIFOLimitation( (unsigned char*) "\nEntering Hibernation!\r\n", 25);
				SystemHybernate();
				break;
			}				
			case 'c': 
			{
				UARTSendNoFIFOLimitation( (unsigned char*) "\nADC conversion started!\r\n", 27);
				//UARTSendNoFIFOLimitation( (unsigned char*) GetAdcVoltage(), 2);				
				GetAdcVoltage();
				break;
			}
			case 'i':
			{
                UARTSendNoFIFOLimitation( (unsigned char*) "\nSetting DAC output voltage!\r\n", 31);	
				GenerateVoltage();
				EraseContainer();
				break;				
			}	
 			case 'o':
 			{
                UARTSendNoFIFOLimitation( (unsigned char*) "\nTemperature reading mode!\n", 27);	
								voltage = GetVoltage();
				
                UARTSendNoFIFOLimitation( (unsigned char*)GetVoltage(), 10);
                break;				
 			}			
// 			case 'p':
// 			{
// 			  UARTSendNoFIFOLimitation( (unsigned char*) "\nDisabling DAC output voltage!\n", 33);	
// 				break;				
//			}				
			default: 
				UARTSendNoFIFOLimitation( (unsigned char*) "\nNot a valid comand! Please try again! Press M for menu !\r\n", 60);
				EraseContainer();
				break;
		}

		return 0;
}
