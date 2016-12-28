#include "inc/lm4f120h5qr.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "inc/lcd.h"


void data(unsigned char c)
{
    RS(1);
    SysCtlDelay(400);  
    databits = c >> 4;
    LCD_STROBE;
    databits = c;
    LCD_STROBE;
}
 
void cmd(unsigned char c)
{
    RS(0);
    SysCtlDelay(400); 
    databits = c >> 4;
    LCD_STROBE;
    databits = c;
    LCD_STROBE;
}
 
void pseudo_8bit_cmd(unsigned char c)
{
    RS(0);
    SysCtlDelay(15000);
    databits = (c);
    LCD_STROBE;
}
void clearLcd(void)
{
    cmd(0x01);
    SysCtlDelay(30000);
}
 
void LcdInit()
{
    pseudo_8bit_cmd(0x30); //this command is like 8 bit mode command
    pseudo_8bit_cmd(0x30); //lcd expect 8bit mode commands at first
    pseudo_8bit_cmd(0x30); //for more details, check any 16x2 lcd spec
    pseudo_8bit_cmd(0x20);
    cmd(0x28);             //4 bit mode command started, set two line
    cmd(0x0c);             // Make cursorinvisible
    clearLcd();               // Clear screen
    cmd(0x6);              // Set entry Mode(auto increment of cursor)
}
 
void string(char *p)
{
    while(*p) data(*p++);
}

void lcdMessage(char* msgLine1, char* msgLine2 )
{

	clearLcd();
	SysCtlDelay(30000);
	//SysCtlDelay(SysCtlClockGet() / (10));	
	LINE1;
	string(msgLine1);
	LINE2;
	string(msgLine2);
	RS(0);
	
}
