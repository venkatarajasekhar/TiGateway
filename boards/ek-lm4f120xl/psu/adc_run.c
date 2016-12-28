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

#define ADC0_ARRAY_SIZE     8UL
#define ADC1_ARRAY_SIZE     4UL

unsigned long ulADC0Value[ADC0_ARRAY_SIZE];
unsigned long ulADC1Value[ADC1_ARRAY_SIZE];
volatile unsigned long ulTempAvg;
volatile unsigned long ulVoltageAvg;
volatile unsigned long ulTempValueC;
volatile unsigned int voltage;

//*****************************************************************************
//
// 
//
//*****************************************************************************
void ConfigureVoltageAdcModule (void)
{
	
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_1MSPS); // 250
	ADCSequenceDisable(ADC0_BASE, 0);

    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  
	ADCSequenceEnable(ADC0_BASE, 0);

    ADCIntClear(ADC0_BASE, 0);
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void ConfigureTemperatureAdcModule (void)
{

    SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS); // 250
 
	ADCSequenceDisable(ADC1_BASE, 1);

    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
  
	ADCSequenceEnable(ADC1_BASE, 1);

    ADCIntClear(ADC1_BASE, 1);
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
unsigned long GetTemperature (void)
{  
	int i;
    unsigned long adc_sum = 0;
    
	ADCIntClear(ADC1_BASE, 1);
    ADCProcessorTrigger(ADC1_BASE, 1);
  
	while(!ADCIntStatus(ADC1_BASE, 1, false))
		;	
	
	ADCSequenceDataGet(ADC1_BASE, 1, &ulADC1Value[0]);
    
	    for (i = 0; i < ADC1_ARRAY_SIZE; i++)
    {
        adc_sum += ulADC1Value[i];
    }
    ulTempAvg = (adc_sum + 2) / ADC1_ARRAY_SIZE;
    ulTempValueC = (1475 - ((2475 * ulTempAvg)) / 4096)/10;
    
    return ulTempValueC;

}

//*****************************************************************************
//
// 
//
//*****************************************************************************
unsigned int GetVoltage (void)
{  
    int i;
    unsigned int adc_sum = 0;
    
    ADCIntClear(ADC0_BASE, 0);
    ADCProcessorTrigger(ADC0_BASE, 0);
  
	while(!ADCIntStatus(ADC0_BASE, 0, false))
        ;
    
    ADCSequenceDataGet(ADC0_BASE, 0, &ulADC0Value[0]);
    
    for (i = 0; i < ADC0_ARRAY_SIZE; i++)
    {
        adc_sum += ulADC0Value[i];
    }
        
	ulVoltageAvg = adc_sum / ADC0_ARRAY_SIZE;

    voltage = (ulVoltageAvg * 3287) / 4095;

    return voltage;
}
