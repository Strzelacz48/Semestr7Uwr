#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

//Marcin Czapulak

#define LED PB2
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PA7
#define BTN_PIN PINA
#define BTN_PORT PORTA


volatile int8_t remember[100];
volatile int8_t i=0;

ISR(TIM1_COMPA_vect)
{
    if (remember[i] == 0){
        LED_PORT |= _BV(LED);
    }
    else {
        LED_PORT &= ~_BV(LED);
    }
    remember[i] = BTN_PIN & _BV(BTN);
    i++;
    if(i>=100){
        i=0;
    }
}

//100 razy na sekunde bez preskalera
void timer_init()
{
    TCCR1B = _BV(CS10) | _BV(WGM12);
    OCR1A = 9999;//liczymy do 10 000 bo tiny liczy do 10 000 + 1
    TIMSK1 = _BV(OCIE1A);
}

int main() {
    timer_init();
    BTN_PORT |= _BV(BTN);
    LED_DDR |= _BV(LED);
    sei();
    set_sleep_mode(SLEEP_MODE_IDLE);
    while (1) {
        sleep_mode();
    }
}
