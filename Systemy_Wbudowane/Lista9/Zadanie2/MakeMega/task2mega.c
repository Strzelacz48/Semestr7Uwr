#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>

//Marcin Czapulak

#define LED PC1
#define LED_DDR DDRC
#define LED_PORT PORTC

#define BTN PC3
#define BTN_PIN PINC
#define BTN_PORT PORTC

void spi_slave_init()
{
    DDRB |= _BV(DDB4);
    SPCR |= _BV(SPE) | _BV(SPIE);
}

ISR(SPI_STC_vect)
{
    uint8_t btn_state = SPDR;
    if (btn_state == 0){
        LED_PORT |= _BV(LED);
    }
    else {
        LED_PORT &= ~_BV(LED);
    }

    SPDR = (BTN_PIN & _BV(BTN))!=0;
}

int main(void)
{
    spi_slave_init();

    BTN_PORT |= _BV(BTN);
    LED_DDR |= _BV(LED);

    sei();
    set_sleep_mode(SLEEP_MODE_IDLE);
    while (1) {
        sleep_mode();
    }
}