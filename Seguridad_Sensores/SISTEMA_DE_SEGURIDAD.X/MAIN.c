#include <xc.h>
#include <pic16f887.h>  // Header file PIC16f887 definitions
#include <stdio.h>
#include <string.h>
#include "CONFIG.h"
#include "LCD.h"
#include "KEYPAD.h"
#include "adc_conversion.h"

// Definición de los pines de los LEDs
#define LED_VERDE PORTEbits.RE0
#define LED_AMARILLO PORTEbits.RE1
#define LED_ROJO PORTEbits.RE2

const char password[5] ={'0','9','2','6',0};
char pass_user[5];
unsigned char idx = 0;
int intentos = 0;    
int celsius;

void delay_ms_variable(unsigned int ms) {
    while (ms--) {
        __delay_ms(1);  // Llamada a __delay_ms con un valor constante
    }
}

// Prototipos de funciones
void blink_led(unsigned char led, unsigned int on_time, unsigned int off_time, unsigned int duration) {
    unsigned int elapsed_time = 0;

    while (elapsed_time < duration) {
        if (led == 'V') {
            LED_VERDE = 1;  // Enciende LED verde
        } else if (led == 'A') {
            LED_AMARILLO = 1;   // Enciende LED amarillo
        } else if (led == 'R') {
            LED_ROJO = 1;   // Enciende LED rojo
        }
        delay_ms_variable(on_time);

        // Apagar LED
        if (led == 'V') {
            LED_VERDE = 0;
        } else if (led == 'A') {
            LED_AMARILLO = 0;
        } else if (led == 'R') {
            LED_ROJO = 0;
        }
        delay_ms_variable(off_time);

        elapsed_time += on_time + off_time;
    }
}

void secuencia1(void){
volatile unsigned int var3 = 0xA0,var4 = 0x01,var5 = 0;
for (int i = 0; i < 2; i++) {
        var5 = var3 | var4;
        PORTD = (unsigned char)(var5);  
        __delay_ms(500);                

        // Desplazamiento de var1 y var2
        var3 = var3 >> 1; // Desplaza var1 a la derecha
        var4 = var4 << 1; // Desplaza var2 a la izquierda

        // Nueva operación OR entre var1 y var2
        var5 = var3 | var4;
        PORTD = (unsigned char)(var5); // Se coloca el resultado en PORTD
        __delay_ms(500);                // Retardo de 500 ms

        // Última operación OR y desplazamiento
        var5 = var3 | var4;
        var3 = var3 << 1; // Desplaza var1 a la izquierda
        var4 = var4 << 1; // Desplaza var2 nuevamente a la izquierda
    }
}

void secuencia5(void){
volatile unsigned int var1=0xA0,var2=0x01,var12 = 0;
    for (int i = 0; i < 2; i++) {
        var12 = var1 | var2;
        PORTD = (unsigned char)(var12);
        __delay_ms(500);
        var1 = var1 >> 1;
        var2 = var2 << 1;
        var12 = var1 | var2;
        PORTD = (unsigned char)(var12);
        __delay_ms(500);
        var12 = var1 | var2;
        var1 = var1 << 1;
        var2 = var2 << 1;
    }
       volatile unsigned int var6=0xA0,var7=0x08,var8 = 0;
    for (int i = 0; i < 2; i++){
        var8 = var6 | var7;
        PORTD = (unsigned char)(var8);
        __delay_ms(500);
        var6 = var6 >> 1;
        var7 = var7 >> 1;
        var8 = var6 | var7;
        PORTD = (unsigned char)(var8);
        __delay_ms(500);
        var8 = var6 | var6;
        var6 = var6 << 1;
        var7 = var7 >> 1;
        
    }
}



void main(void) {

    char StringTemperature[32];
    char key = '0';
    OSCCON = 0x71; //Configura oscilador interno (FOSC = 8Mhz)
     
    LCD_Init(); //Inicializa el LCD
    LCD_String_xy(0,0,"BIENVENIDO");
    __delay_ms(2000);
    LCD_Clear();

    keypad_init(); //Inicializa el teclado
    
    adc_init();  // Inicializa el ADC
    
    TRISE = 0b00000000;   // Configura salidas de los leds
    ANSEL = 0x00;
    PORTE= 0x00;
    ANSELH = 0x00;
    TRISD = 0b00000000;
    secuencia1();
    secuencia5();

    while (1) {
        LCD_String_xy(0,0,"Press a Key     ");
        LCD_Command(0xc0);
        do{
            key = keypad_getkey();
            if(key != 0){
                LCD_String_xy(2,idx,"*");
                pass_user[idx++] = key;
                //LCD_Character();
            }
            __delay_ms(100);
        }while(idx < 5);
        
        
        
        if(strncmp(pass_user,password,4)==0){
            LCD_Clear();
            LCD_String_xy(0,0,"Clave Correcta");
            Lmain();
            blink_led('V', 500, 500, 3000);  // Parpadear LED verde por 3 segundos
            intentos = 0;
            __delay_ms(700);
            while (1) {
                LCD_String_xy(0,0,"AMBIENTAL");
                LCD_Command(0xC0);
                unsigned int temperatura = adc_read(0);
                unsigned int luz = adc_read(1);

                //celsius = (temperatura*4.88);
                //celsius = (celsius/10.00);
                //sprintf(StringTemperature,"TEMP %.2f %cC  ", celsius,0xdf); /*convert integer value to ASCII string */

                int value_adc = 1023 - (int)temperatura; /* Calcular valor del sensor */
                celsius = (int)(value_adc * 0.04058); /* Convertir a temperatura */
                sprintf(StringTemperature, "TEMP: %d  L: %d", celsius, luz);  /*convert integer value to ASCII string */
                //LCD_String_xy(1,0,StringTemperature);
                LCD_String(StringTemperature);
                __delay_ms(2000);
                LCD_Clear();
            }
        }
        else{
            LCD_Clear();
            LCD_String_xy(0,0,"Clave Incorrecta"); 
            blink_led('A', 300, 700, 2000);  // Parpadear LED amrillo por 2 segundos
            intentos = intentos + 1;
            if(intentos ==3){
                LCD_Clear();
                LCD_String_xy(0,0,"sistema bloqueado");
                blink_led('R', 300, 500, 8000);  // Parpadear LED rojo por 8 segundos
                intentos = 0;  // Reiniciar intentos después del bloqueo
                
            }
        }
        __delay_ms(2000);
        LCD_Clear();
        idx = 0;
}
}