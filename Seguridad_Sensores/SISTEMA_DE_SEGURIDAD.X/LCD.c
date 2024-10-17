#include <xc.h>
#include "LCD.h"
#include "CONFIG.h"



unsigned char character1[8] = {0x00, 0x1f, 0x0a, 0x0a, 0x0a, 0x0a, 0x09, 0x00}; /* Value for Pi */
unsigned char character2[8] = {0x11, 0x19, 0x1d, 0x1f, 0x1d, 0x19, 0x11, 0x00}; /* Value for Diode */
unsigned char character3[8] = {0x12, 0x14, 0x18, 0x10, 0x18, 0x15, 0x13, 0x17}; /* Value for Transistor */
unsigned char character4[8] = {0x0a, 0x0a, 0x0a, 0x1b, 0x0a, 0x0a, 0x0a,0x00};  /* Value for Capacitor */
unsigned char character5[8] = {0x00, 0x0e, 0x011, 0x11, 0x11, 0x0a, 0x1B, 0x00};/* Value for Ohm */




void LCD_Init(void)
{
    LCD_Port = 0;         /*PORT as Output Port*/
    __delay_ms(15);          /* 15 ms, Power-On delay*/
    LCD_Command(0x02);    /*send for initialization of LCD with nibble method */
    LCD_Command(0x28);    /*use 2 line and initialize 5*7 matrix in (4-bit mode)*/
    LCD_Command(0x01);    /*clear display screen*/
    LCD_Command(0x0c);    /*display on cursor off*/
    LCD_Command(0x06);    /*increment cursor (shift cursor to right)*/	
}

void LCD_Command(unsigned char cmd )
{
	ldata = (ldata & 0x0f) |(0xF0 & cmd);  /*Send higher nibble of command first to PORT*/ 
	RS = 0;  /*Command Register is selected i.e.RS=0*/ 
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/ 
	NOP();
	EN = 0;
	__delay_ms(1);
    ldata = (unsigned char)((ldata & 0x0f) | (cmd<<4));  /*Send lower nibble of command to PORT */
	EN = 1;
	NOP();
	EN = 0;
	__delay_ms(3);
}

void LCD_Char(unsigned char dat)
{
	ldata = (ldata & 0x0f) | (0xF0 & dat);  /*Send higher nibble of data first to PORT*/
	RS = 1;  /*Data Register is selected*/
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	__delay_ms(1);
    ldata = (unsigned char)((ldata & 0x0f) | (dat<<4));  /*Send lower nibble of data to PORT*/
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	__delay_ms(3);
}


void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{		
	  LCD_Char(*msg);
	  msg++;	
    }
}


void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row<=1)
    {
        location=(0x80) | ((pos) & 0x0f);  /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f);  /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
    

    LCD_String(msg);

}

void LCD_Clear(void)
{
   	LCD_Command(0x01);
    __delay_ms(3);
}


void LCD_Custom_Char(unsigned char loc,unsigned char *msg)
{
    unsigned char i;
    if(loc<8)
    {
     LCD_Command(0x40+(loc*8));     /* Command 0x40 and onwards forces the device to point CGRAM address */
      for(i=0;i<8;i++)             /* Write 8 byte for generation of 1 character */
           LCD_Char(msg[i]);
        
    }   
}

void Lmain(void)
{
    char i;
                           /*Initialize LCD*/    
    LCD_String_xy(1,0,"");/*Display string for respective symbol*/
    LCD_Custom_Char(0,character1);       /*Write custom character to CGRAM 0x00 memory location*/
    LCD_Custom_Char(1,character2);       /*Write custom character to CGRAM 0x01 memory location*/
    LCD_Custom_Char(2,character3);       /*Write custom character to CGRAM 0x02 memory location*/
    LCD_Custom_Char(3,character4);       /*Write custom character to CGRAM 0x03 memory location*/
    LCD_Custom_Char(4,character5);       /*Write custom character to CGRAM 0x04 memory location*/
    
    for(i=0;i<5;i++)
    {    
        LCD_Command(0xc0|(i*3));     /*Display characters from c0(2nd row) location */
        LCD_Char(i);                 /*To display custom character send address as data to point stored 
                                       character */
    }  
}