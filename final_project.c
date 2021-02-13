//Credits: ?Microchip Developers Manual,? Microchip Technology, Inc. https://microchipdeveloper.com/8avr:read-input (accessed Nov. 09, 2020).

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<avr/io.h>

#define F_CPU 16000000UL
#define ISR_TIMER0_RELOAD (30)
#define DELAY_CONST (200)

static int8_t isr_count = ISR_TIMER0_RELOAD;
void delay(volatile int8_t );

int main (void) {
    
    DDRB |= (1<<5) | (1<<4) |(1<<2) | (1<<1);  //turn on pin 5, 4, 2, 1 for PORTB to be output
    DDRB &= ~(1<<7);  //turn off pin 7 of PORTB to be input (the button)
    DDRC |= (1<<1); //Turn on pin 1 of PORTC to be output
    DDRD |= (1<<5) | (1<<4) | (1<<3) | (1<<2) | (1<<1) | (1<<0); //turn on pin 5, 4, 3, 2, 1, 0 to be output
    TCCR0A |= (1<<WGM01);  //Set Timer0's timer mode to CTC
    OCR0A = 0xFF; ////Set the value that you want to count to 256
    TIMSK0 |= (1<<OCIE0A);  //Set up Timer0 to have an Interrupt Service Request on 'COMPA'
    asm("SEI");  //Enable Interrupts
    //Control Timer0's pre scale
    //CS00 CS01 CS02: 1 0 0: no pre scaler
    //CS00 CS01 CS02: 0 0 1: set pre scaler to 256 and start timer
    //CS00 CS01 CS02: 1 0 1: set pre scaler to 1024 and start timer
    TCCR0B |=    (1 << CS00)|(1 << CS02); //1024 uses 101 bit pattern
    
    while (1)
    {
        asm("NOP");
        break;
    }   
    
    buttonTest();
    //Return 0 in assembler
    asm("LDI R24, 0x00");
    asm("LDI R25, 0x00");
    asm("RET");
}

//Interrupt Service Routine for Timer0

ISR(TIMER0_COMPA_vect) {
    
    if (isr_count > 0) {
        isr_count = isr_count - 1; 
    } 
     
    else {
        //PORTB^= (1<<5)
        asm("IN R25, 0x05");
        asm("LDI R24, 0x20");
        asm("EOR R24, R25");
        asm("OUT 0x05, R24");
        isr_count = ISR_TIMER0_RELOAD;  
    }
}

void delay(volatile int8_t x) {
    
    while(x > 0) {
         x = x - 1;
         x = x + 1;
         x = x - 1;
         asm("NOP");
    }
     
}

void buttonTest() {
    while(1) {
        
        if (!(PINB & (1<<PINB7))) { 
            turnLEDOn();
            turnFanOn();
        }
        else {
            turnLEDOff();
            turnFanOff();
        }
    }
}

void turnLEDOn() {
    PORTB |= (1<<5) | (1<<4) | (1<<2) | (1<<1);
    PORTD = (1<<5) | (1<<4) | (1<<3) | (1<<2) | (1<<1) | (1<<0);
}

void turnLEDOff() {

    asm("CBI 0x05, 5");
    asm("CBI 0x05, 4");
    asm("CBI 0x05, 2");
    asm("CBI 0x05, 1");
    asm("CBI 0x0B, 5");
    asm("CBI 0x0B, 4");
    asm("CBI 0x0B, 3");
    asm("CBI 0x0B, 2");
    asm("CBI 0x0B, 1");
    asm("CBI 0x0B, 0");
    
    
}

 void turnFanOn() {
     asm("SBI 0x08, 1");
 }
 
 void turnFanOff() {
    asm("CBI 0x08, 1");
 }