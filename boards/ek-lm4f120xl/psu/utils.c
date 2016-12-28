
#include "inc/utils.h"
//*****************************************************************************
//
// Function for busy delaying between different user commands
//
//*****************************************************************************
extern void ThinkMeter(unsigned int delay)
{
	while(delay--)
	{
		BlinkTransfer(RED_LED);
		ROM_UARTCharPutNonBlocking(UART0_BASE, '>');
		SysCtlDelay(SysCtlClockGet() / (500 * 3));
	}
	
	ROM_UARTCharPutNonBlocking(UART0_BASE, '\n');
}

//*****************************************************************************
//
// Led indicator to indicate that the power is on
//
//*****************************************************************************
void PSUStatusOnline(unsigned char LED)
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
		
	if(LED)
    {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, LED); 
	}
    else
    {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, FALSE);
    }	
}

//*****************************************************************************
//
// Simple LED status blink utility function
//
//*****************************************************************************
void BlinkTransfer(unsigned color)
{
	//
	// Enable the GPIO pins for the LED (PF2).  
	//
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	
	//
	// Blink the LED to show a character transfer is occuring.
	//
	
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, color); 
			
	//
	// Delay for 1 millisecond.  Each SysCtlDelay is about 3 clocks.
	//
	SysCtlDelay(SysCtlClockGet() / (1000 * 3));
				
	//
	// Turn off the LED
	//
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0); 	
}

//*****************************************************************************
//
// Function that passes the ASCII uart terminal chars to an hex value.
//
//*****************************************************************************
void AsciiToHex( unsigned int data, unsigned char* val )
{
	switch(data)
	{
		case 0x30: *val = 0; break;
		case 0x31: *val = 1; break;
		case 0x32: *val = 2; break;
		case 0x33: *val = 3; break;
		case 0x34: *val = 4; break;
		case 0x35: *val = 5; break;
		case 0x36: *val = 6; break;
		case 0x37: *val = 7; break; 
		case 0x38: *val = 8; break;
		case 0x39: *val = 9; break;
			
		case 0x61: *val = 0x0A; break;
		case 0x62: *val = 0x0B; break;		
		case 0x63: *val = 0x0C; break;
		case 0x64: *val = 0x0D; break;		
		case 0x65: *val = 0x0E; break;		
		case 0x66: *val = 0x0F; break;		
		
		default: 
		{	
			DISABLE_PSU_FLAG = TRUE;
			COMMAND_NOT_OK = TRUE;
			EmergencyStop();
			UARTSendNoFIFOLimitation( (unsigned char*) "\nOutput DISABLED!\n", 20); 			
			UARTSendNoFIFOLimitation( (unsigned char*) "\nWrong input!\n", 16); 
			break;
		}
	}
}

//*****************************************************************************
//
// Function that passes a HEX value to an ASCII character.
//
//*****************************************************************************
void HexToAscii( unsigned char data, unsigned char* val )
{
	switch(data)
	{
		case 0: *val = 0x30; break;
		case 1: *val = 0x31; break;
		case 2: *val = 0x32; break;
		case 3: *val = 0x33; break;
		case 4: *val = 0x34; break;
		case 5: *val = 0x35; break;
		case 6: *val = 0x36; break;
		case 7: *val = 0x37; break; 
		case 8: *val = 0x38; break;
		case 9: *val = 0x39; break;
		
		case 0x0A: *val = 0x61; break;
		case 0x0B: *val = 0x62; break;		
		case 0x0C: *val = 0x63; break;
		case 0x0D: *val = 0x64; break;		
		case 0x0E: *val = 0x65; break;		
		case 0x0F: *val = 0x66; break;
						
	}
}

//*****************************************************************************
//
// Function that receives a hex string input and generates a decimal value
//
//*****************************************************************************
unsigned int HexToThousand(unsigned int* buf)
{
	unsigned int index;
	unsigned int returnValue = 0;
	
	for(index = 0; index < NO_OF_BYTES_TO_ASSEMBLE; index++)
	{
		returnValue += (*buf++) * exp10(NO_OF_BYTES_TO_ASSEMBLE - index - 1);
	}
		return returnValue;	
}

//**********************************************************************************************************************************************************
//
//Function that turns a numerical value to chunks of chars used for assembly of a hex command
//
//**********************************************************************************************************************************************************   
void DecimalToHex(unsigned int value, char* result, char size)
{    
    while(size--)
    {
     *result++ = value % 10;  
     value = value / 10;  
    }
} 
   
//*****************************************************************************
//
// Exponential function for 10
//
//*****************************************************************************
unsigned int exp10(unsigned int power)
{
	int index;
	unsigned int value = 1;
	
	for(index = 0; index < power; index++)
		value = value * 10;
	
	return value;
}

//*****************************************************************************
//
// Function for dumping over uart the input command buffer
//
//*****************************************************************************

extern void FlushContainer(void)
{
	int size = SIZE_OF_CONTAINER;
	int index = 0;
	
	while(size--)
	{
		SysCtlDelay(SysCtlClockGet() / (100 * 3));	
		UARTSendOneByte(container[index]);
		SysCtlDelay(SysCtlClockGet() / (100 * 3));	
		index++;
	}
	
	UARTSend((unsigned char *)"\n", 2);
}

//*****************************************************************************
//
// Function for clearing the input command buffer
//
//*****************************************************************************
extern void EraseContainer()
{
    int index = 0;
	for(; index < SIZE_OF_CONTAINER; index++)
		container[index] = '0';
}

//*****************************************************************************
//
// Function for clearing the input buffer received from GPRS
//
//*****************************************************************************
extern void EraseGPRSContainer()
{
    int index = 0;
	for(; index < SIZE_OF_CONTAINER; index++)
		container[index] = '*';
}

