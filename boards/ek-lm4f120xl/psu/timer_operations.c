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
#include "inc/timer_operations.h"
#include "inc/uart_comm.h"
#include "inc/commands.h"
#include "inc/adc_run.h"
#include "inc/i2c_comm.h"
#include "inc/utils.h"


volatile char TIMER1A_OVERFLOW;
volatile char TIMER2A_OVERFLOW;
volatile char TIMER3A_OVERFLOW;

//*****************************************************************************
//
// The interrupt handler for the second timer interrupt.
//
//*****************************************************************************
void Timer1IntHandler(void)
{	
	PSUStatusOnline(RED_LED);
	
	//
	// Clear the timer interrupt.
	//
	ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    
	//
  // Toggle the flag
  //	
	TIMER1A_OVERFLOW = TRUE;
	SysCtlDelay(SysCtlClockGet() / (10 * 3));
	
	PSUStatusOnline(FALSE);	
}

//*****************************************************************************
//
// The interrupt handler for the second timer interrupt.
//
//*****************************************************************************
void Timer2IntHandler(void)
{	
	PSUStatusOnline(GREEN_LED);
	
	//
	// Clear the timer interrupt.
	//
	ROM_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    
	//
  // Toggle the flag
  //	
	TIMER2A_OVERFLOW = TRUE;
	
	SysCtlDelay(SysCtlClockGet() / (10 * 3));
	
	PSUStatusOnline(FALSE);	
}

//*****************************************************************************
//
// The interrupt handler for the second timer interrupt.
//
//*****************************************************************************
void Timer3IntHandler(void)
{	
	PSUStatusOnline(BLUE_LED);
	 
	//
	// Clear the timer interrupt.
	//
	ROM_TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
    
	//
  // Toggle the flag
  //	
	TIMER3A_OVERFLOW = TRUE;
	
	SysCtlDelay(SysCtlClockGet() / (10 * 3));
	
	PSUStatusOnline(FALSE);	
}

//*****************************************************************************
//
//
//
//*****************************************************************************
void StartTimer1A(void* ReloadValue)
{
	ROM_IntMasterDisable();
	ROM_IntMasterEnable();
	TIMER1A_OVERFLOW = FALSE;
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);   // 32 bits Timer
	ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, (unsigned long)ReloadValue);
  TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1IntHandler);    // Registering  isr       
  ROM_TimerEnable(TIMER1_BASE, TIMER_A); 
  ROM_IntEnable(INT_TIMER1A); 
	ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);  	
}

//*****************************************************************************
//
//
//
//*****************************************************************************
void StartTimer2A(void* ReloadValue)
{
	ROM_IntMasterDisable();
	ROM_IntMasterEnable();
	TIMER2A_OVERFLOW = FALSE;
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);   // 32 bits Timer
	ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, (unsigned long) ReloadValue);
  TimerIntRegister(TIMER2_BASE, TIMER_A, Timer2IntHandler);    // Registering  isr       
  ROM_TimerEnable(TIMER2_BASE, TIMER_A); 
  ROM_IntEnable(INT_TIMER2A); 
	ROM_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);  	
}

//*****************************************************************************
//
//
//
//*****************************************************************************
void StartTimer3A(void* ReloadValue)
{
	ROM_IntMasterDisable();
	ROM_IntMasterEnable();
	TIMER3A_OVERFLOW = FALSE;
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
  ROM_TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);   // 32 bits Timer
	ROM_TimerLoadSet(TIMER3_BASE, TIMER_A, (unsigned long) ReloadValue);
  TimerIntRegister(TIMER3_BASE, TIMER_A, Timer3IntHandler);    // Registering  isr       
  ROM_TimerEnable(TIMER3_BASE, TIMER_A); 
  ROM_IntEnable(INT_TIMER3A); 
	ROM_TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);  	
}

