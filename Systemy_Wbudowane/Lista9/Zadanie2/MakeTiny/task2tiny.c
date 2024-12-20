#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/sleep.h>

//Marcin Czapulak

#define LED PB2
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PA7
#define BTN_PIN PINA
#define BTN_PORT PORTA

uint8_t spi_transfer(uint8_t data)
{
    // załaduj dane do przesłania
    USIDR = data;
    // wyczyść flagę przerwania USI
    USISR = _BV(USIOIF);
    // póki transmisja nie została ukończona, wysyłaj impulsy zegara
    while (!(USISR & _BV(USIOIF))) {
        // wygeneruj pojedyncze zbocze zegarowe
        // zostanie wykonane 16 razy
        USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC);
    }
    // zwróć otrzymane dane
    return USIDR;
}

void spi_init()
{
    // ustaw piny MOSI i SCK jako wyjścia
    DDRA = _BV(DDA4) | _BV(DDA5);
    // ustaw USI w trybie trzyprzewodowym (SPI)
    USICR = _BV(USIWM0);
}

int main() {
    spi_init();
    BTN_PORT |= _BV(BTN);
    LED_DDR |= _BV(LED);
    uint8_t btn_state;
    while (1) {
        btn_state = spi_transfer(BTN_PIN & _BV(BTN));
        if (btn_state == 0){ 
            LED_PORT |= _BV(LED);
        }
        else {
            LED_PORT &= ~_BV(LED);
        }
        _delay_ms(10);
    }
}