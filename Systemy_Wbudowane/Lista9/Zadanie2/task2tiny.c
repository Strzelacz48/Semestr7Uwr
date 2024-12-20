#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define LED PB2
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PA7
#define BTN_PIN PINA
#define BTN_PORT PORTA

#define SS PA3

void spi_init()
{
    // ustaw piny MOSI i SCK jako wyjścia
    DDRA = _BV(DDA4) | _BV(DDA5);
    // ustaw USI w trybie trzyprzewodowym (SPI)
    USICR = _BV(USIWM0);
}

uint8_t spi_transfer(uint8_t data)
{
    // załaduj dane do przesłania
    USIDR = data;
    // wyczyść flagę przerwania USI
    USISR = _BV(USIOIF);
    //=======================================================================================
    //TODO ustaw ss na niski
    PORTA &= ~_BV(SS);
    //=======================================================================================
    // póki transmisja nie została ukończona, wysyłaj impulsy zegara
    while (!(USISR & _BV(USIOIF))) {
        // wygeneruj pojedyncze zbocze zegarowe
        // zostanie wykonane 16 razy
        USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC);
    }
    // zwróć otrzymane dane
    //=======================================================================================
    //TODO ustaw ss na wysoki
    PORTA |= _BV(SS);
    //=======================================================================================
    return USIDR;
}

int main()
{
    spi_init();
    BTN_PORT |= _BV(BTN);
    LED_DDR |= _BV(LED);
    PORTA |= _BV(SS);
    uint8_t state;
    while(1) {
        state = spi_transfer(BTN_PIN & _BV(BTN));
        if (state == 0) LED_PORT |= _BV(LED);
        else LED_PORT &= ~_BV(LED);
        _delay_ms(10);
    }
}

