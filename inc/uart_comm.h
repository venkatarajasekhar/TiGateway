#ifndef __UART_ECHO_H__
#define __UART_ECHO_H__

#include "stdbool.h"

#ifndef TRUE
    #define TRUE					true
#endif

#ifndef FALSE
    #define FALSE					false
#endif


// UART communication parameter
#define BAUD_RATE					38400UL
#define MODEM_BAUD_RATE             9600UL

//Uart RX FIFO size in bytes
#define SIZE_OF_CONTAINER		    128UL // This is the size of the container array
#define MAX_FIFO_SIZE				128UL

//Define port associations to the onboard leds
#define RED_LED   					GPIO_PIN_1
#define BLUE_LED  					GPIO_PIN_2
#define GREEN_LED 					GPIO_PIN_3
#define ALL_LEDS					GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3



//global array that holds the input array
extern unsigned char container[SIZE_OF_CONTAINER];
extern unsigned char GPRScontainer[SIZE_OF_CONTAINER];
extern volatile bool flagUartReceive;
extern volatile bool flagGPRSUartReceive;

/*ISR and initial clock configuration for UART modules*/
void	InitDebugUart(void);
void	InitGPRSUart(void);

/*Debug serial console print methods*/
void 	UARTSendOneByte(unsigned char byte);
void	UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);
void	UARTSendNoFIFOLimitation(unsigned char *pucBuffer, unsigned long ulCount);

/*GPRS communication methods using custom uart peripheral*/
void 	UARTGPRSSendOneByte(unsigned char byte);
void	UARTGPRSSend(const unsigned char *pucBuffer, unsigned long ulCount);
void	UARTGPRSSendBlocking(const unsigned char *pucBuffer, unsigned long ulCount);


#endif 
