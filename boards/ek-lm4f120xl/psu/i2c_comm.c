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
#include "inc/utils.h"
#include "inc/i2c_comm.h"


//*****************************************************************************
//
// Utility funtion that compares feedback value to the command and returns status
//
//*****************************************************************************
void DoDifferenceReading(unsigned int commandValue, unsigned int feedbackValue)
{
	unsigned int index, temp = 0;

	char hexArray[NO_OF_BYTES_TO_ASSEMBLE] = {0};
	//unsigned char asciiArray[NO_OF_BYTES_TO_ASSEMBLE + SPACE_CHAR_SIZE + MILIVOLT_DISPLAY_SIZE + SIZE_OF_NULL_CHAR];
	unsigned char asciiArray[16];	

	DecimalToHex(feedbackValue, &hexArray[0], NO_OF_BYTES_TO_ASSEMBLE);
	
	for(index = 0; index < NO_OF_BYTES_TO_ASSEMBLE; index++)
	{
		HexToAscii( hexArray[NO_OF_BYTES_TO_ASSEMBLE - index - 1], &asciiArray[index] );
	}
	
	asciiArray[4] = 0x20;	asciiArray[5] = 0x6D;	asciiArray[6] = 0x69;	asciiArray[7] = 0x6C;	asciiArray[8] = 0x69;
	asciiArray[9] = 0x56;	asciiArray[10] = 0x6F;	asciiArray[11] = 0x6C;	asciiArray[12] = 0x74;	asciiArray[13] = 0x73;
	asciiArray[14] = 0x20;	asciiArray[15] = 0x20;
	
	if(feedbackValue >= commandValue)
		temp = feedbackValue - commandValue;
	else 
		temp = commandValue - feedbackValue;
	
	clearLcd();
	if(feedbackValue > 30)
	{
		if( temp <= 1) 
		{
			lcdMessage("DAC output OK!:)",(char*)&asciiArray[0]);	
		}
		else		
		{
			lcdMessage("DAC Error! :(", "Retry Command!");
		}
	}
	else
	{
		lcdMessage("DAC Disabled! :P", "****************");
	}
}


//*****************************************************************************
//
// Utility funtion for GPIO function intialisation
//
//*****************************************************************************
void ConfigureI2CModule(void)
{
	//
	// Select the I2C function for these pins.  This function will also
	// configure the GPIO pins for I2C operation, setting them to
	// open-drain operation with weak pull-ups.  Consult the data sheet
	// to see which functions are allocated per pin.
	//
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);	//	special I2CSCL treatment for M4F devices
	ROM_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
	
	//
	// Configure the pin muxing for I2C0 functions on port B2 and B3.
	// This step is not necessary if your part does not support pin muxing.
	//
	ROM_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	ROM_GPIOPinConfigure(GPIO_PB3_I2C0SDA);
		
	//
	// Enable and initialize the I2C0 master module.
	//
	ROM_I2CMasterInitExpClk(I2C0_MASTER_BASE, SysCtlClockGet(), false);

}

//*****************************************************************************
//
// Function used to call DAC controller board and receive output feedback
//
//*****************************************************************************
void GenerateVoltage(void)
{
	unsigned int i2cBuf[SIZEOFI2CFIELD + SIZEOFI2CCOMMAND];
	unsigned int i2cRecBuf[SIZEOFI2CFIELD];	
	unsigned int reservedField, i2cBufCmdSetpointNumericValue, i2cBufActualNumericValue;
	
	FillStructureI2C(&i2cBuf[0], &reservedField, SIZEOFI2CFIELD + SIZEOFI2CCOMMAND);	
			
	I2CWriteSlaveDac(DAC_SLAVE_ADRESS, &i2cBuf[0], SIZEOFI2CFIELD + SIZEOFI2CCOMMAND);
	I2CReadSlaveDac(DAC_SLAVE_ADRESS, &i2cRecBuf[0], SIZEOFI2CFIELD + SIZEOFI2CCOMMAND);

	i2cBufCmdSetpointNumericValue = HexToThousand(&i2cBuf[1]);
	i2cBufActualNumericValue = HexToThousand(&i2cRecBuf[0]);
	
	DoDifferenceReading(i2cBufCmdSetpointNumericValue, i2cBufActualNumericValue);
}

//*****************************************************************************
//
// Function that sends an input command string to the DAC controller board
//
//*****************************************************************************
void I2CWriteSlaveDac(char slaveAdress, unsigned int* i2cCmdBuf, char length)
{
	int var = 1, index = 1;
	unsigned char i2cBufferValues[SIZEOFI2CFIELD + SIZEOFI2CCOMMAND];
	
	for(index = 0; index < length; index++)
	{
		i2cBufferValues[index] = *i2cCmdBuf++;	
	}	

	
	ROM_I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, slaveAdress, false); // Write
	
	ROM_I2CMasterDataPut(I2C0_MASTER_BASE, i2cBufferValues[0]);  
	ROM_I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(ROM_I2CMasterBusy(I2C0_MASTER_BASE)) 
			;
	
	while( var < length )					
	{
		ROM_I2CMasterDataPut(I2C0_MASTER_BASE, i2cBufferValues[var] );  
		ROM_I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT); 
		while(ROM_I2CMasterBusy(I2C0_MASTER_BASE))
			;	
		var++;
	}					

	ROM_I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(ROM_I2CMasterBusy(I2C0_MASTER_BASE)) 
			;
	
}

//*****************************************************************************
//
// 
//
//*****************************************************************************
/*static unsigned long WaitI2CDone( unsigned int long ulBase){
    // Wait until done transmitting
    while( I2CMasterBusy(I2C3_MASTER_BASE));
    // Return I2C error code
    return I2CMasterErr( I2C3_MASTER_BASE);
}
*/
//*****************************************************************************
//
// Function used to read the input stream from the DAC controller board.
//
//*****************************************************************************
void I2CReadSlaveDac(char slaveAdress, unsigned int* i2cRespBuf, char length)
{
	unsigned long numberOfIncomingBytes;
		
	ROM_I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, slaveAdress, true); // Read 
 	
 	ROM_I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
 	while(ROM_I2CMasterBusy(I2C0_MASTER_BASE)) 
 			;
 	
 	numberOfIncomingBytes = ROM_I2CMasterDataGet(I2C0_MASTER_BASE);  
	
	while(numberOfIncomingBytes--)
	{
		ROM_I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		while(ROM_I2CMasterBusy(I2C0_MASTER_BASE)) 
 			;		
		*i2cRespBuf++ = ROM_I2CMasterDataGet(I2C0_MASTER_BASE);
	}

 	ROM_I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
	while(ROM_I2CMasterBusy(I2C0_MASTER_BASE)) 
 			;	

	
}

