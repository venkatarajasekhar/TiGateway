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
#include "driverlib/hibernate.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/uart_comm.h"
#include "inc/adc_run.h"
#include "inc/lcd.h"
#include "inc/utils.h"


volatile unsigned long ulLoop;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// 
//
//*****************************************************************************
void InitGPRSUart(void)
{
	//
	// Enable processor interrupts.
	//
	ROM_IntMasterEnable();
    //
    // Set GPIO B0 and B1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    
    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), MODEM_BAUD_RATE,(UART_CONFIG_WLEN_8 \
                                | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    
    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART1);
    ROM_UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void InitDebugUart(void)
{
	//
	// Enable processor interrupts.
	//
	ROM_IntMasterEnable();
    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    
    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), BAUD_RATE,(UART_CONFIG_WLEN_8 \
                                | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    
    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

//*****************************************************************************
//
// Send a string to the Debug UART.
//
//*****************************************************************************
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
	//
	// Loop while there are more characters to send.
	//
	while(ulCount--)
	{
        //
		// Write the next character to the UART.
		//
		ROM_UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
	}
}

//*****************************************************************************
//
// Send a string to the Debug UART.
//
//*****************************************************************************
void UARTGPRSSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
	//
	// Loop while there are more characters to send.
	//
	while(ulCount--)
	{
        //
		// Write the next character to the UART.
		//
		ROM_UARTCharPutNonBlocking(UART1_BASE, *pucBuffer++);
	}
}

//*****************************************************************************
//
// Send a string to the Debug UART.
//
//*****************************************************************************
void UARTGPRSSendBlocking(const unsigned char *pucBuffer, unsigned long ulCount)
{
	//
	// Loop while there are more characters to send.
	//
	while(ulCount--)
	{
        //
		// Write the next character to the UART.
		//
        UARTCharPut(UART1_BASE, *pucBuffer++);
	}
}


//*****************************************************************************
//
//UART Debug send method
//
//*****************************************************************************
void UARTSendOneByte(unsigned char byte)
{
	ROM_UARTCharPutNonBlocking(UART0_BASE, byte);
}

//*****************************************************************************
//
// GPRS UART send method
//
//*****************************************************************************
void UARTGPRSSendOneByte(unsigned char byte)
{
	ROM_UARTCharPutNonBlocking(UART1_BASE, byte);
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
void UARTSendNoFIFOLimitation(unsigned char *pucBuffer, unsigned long ulCount)
{
	volatile unsigned int numberofsubstrings = 0;
	unsigned int index;

	if(ulCount > 16)
	{
		if(ulCount % MAX_FIFO_SIZE == 0)
        {
			// Extract the number of chunks needed for sending 
            numberofsubstrings = ulCount / MAX_FIFO_SIZE;
		}
        else 
        {
			if(ulCount > MAX_FIFO_SIZE)
            {
                numberofsubstrings = ulCount / MAX_FIFO_SIZE + 1;
            }
        }
	}
	else
    {
        // Data to be sent can be sent in one burst
		numberofsubstrings = 1;
    }
    
	for(index = 0; index < ulCount; index++)
	{
		SysCtlDelay(SysCtlClockGet() / (3000 * 3));	
		UARTSendOneByte(pucBuffer[index]);
	}
}	


//*****************************************************************************
//
// The UART debug interrupt handler.
//
//*****************************************************************************
void UARTIntHandler(void)
{
	unsigned long ulStatus;
	unsigned int index = 0; 

	//
    // Get the interrrupt status.
    //
    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);
    
    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ulStatus);
    
    //
    // Loop while there are characters in the receive FIFO.
    //
    
    while(ROM_UARTCharsAvail(UART0_BASE))
    { 
        flagUartReceive = TRUE;	
        container[index] = UARTCharGetNonBlocking (UART0_BASE); 
        BlinkTransfer(BLUE_LED);
        index++;
    }
}

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void GPRSUARTIntHandler(void)
{
	unsigned long ulStatus;
	unsigned int index = 0; 

	//
    // Get the interrrupt status.
    //
    ulStatus = ROM_UARTIntStatus(UART1_BASE, true);
    
    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART1_BASE, ulStatus);
    
    //
    // Loop while there are characters in the receive FIFO.
    //
    
    while(ROM_UARTCharsAvail(UART1_BASE))
    { 
        flagGPRSUartReceive = TRUE;	
        GPRScontainer[index] = UARTCharGetNonBlocking (UART1_BASE); 
        BlinkTransfer(BLUE_LED);
        index++;
    }
}
