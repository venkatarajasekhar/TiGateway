#ifndef __LCD_H__
#define __LCD_H__

#define RS(X)     GPIO_PORTF_DATA_R = ((GPIO_PORTF_DATA_R & ~(1<<1)) | (X<<1))
#define EN(X)     GPIO_PORTF_DATA_R = ((GPIO_PORTF_DATA_R & ~(1<<2)) | (X<<2))
#define LCD_STROBE do{EN(1);EN(0);}while(0)
#define databits GPIO_PORTD_DATA_R  // P1.7 - D7, ....., P1.4 - D4
#define LINE1 cmd(0x80)
#define LINE2 cmd(0xc0)

void data(unsigned char c);
void cmd(unsigned char c);
void pseudo_8bit_cmd(unsigned char c);
void clearLcd(void);
void LcdInit(void);
void string(char *p);
void lcdMessage(char* msgLine1, char* msgLine2 );

#endif
