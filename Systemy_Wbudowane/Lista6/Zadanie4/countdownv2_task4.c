#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)
//Marcin Czapulak

// Definicje pinów sterujących TLC5916
#define LE PB1  // Latch Enable (LE)
#define OE PB2  // Output Enable (OE)

// Inicjalizacja SPI
void spi_init()
{
    // Ustaw MOSI i SCK jako wyjścia
    DDRB |= _BV(PB3) | _BV(PB5) | _BV(LE) | _BV(OE);
    // Włącz SPI w trybie Master, ustaw SCK na fosc/16
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
}

// Wysyłanie bajtu przez SPI
void spi_transmit(uint8_t data)
{
    SPDR = data;                   // Załaduj dane do rejestru przesuwnego
    while (!(SPSR & _BV(SPIF)));   // Czekaj na zakończenie transmisji
}

// Ustawienie stanu LED przez TLC5916
void set_led(uint8_t segments)
{
    spi_transmit(segments);        // Prześlij dane o stanie segmentów
    PORTB |= _BV(LE);              // Ustaw LE na wysoki stan
    PORTB &= ~_BV(LE);             // Opadające zbocze LE
}

// Włącz lub wyłącz diody (OE)
void set_output_enable(uint8_t enable)
{
    if (enable)
        PORTB &= ~_BV(OE);         // Włącz diody LED (OE niski)
    else
        PORTB |= _BV(OE);          // Wyłącz diody LED (OE wysoki)
}

// Kod segmentów dla cyfr 0-9 (anoda wspólna)
const uint8_t digit_to_segment[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

int main()
{
    spi_init();                  // Inicjalizacja SPI
    DDRB |= _BV(LE) | _BV(OE);   // Ustaw LE i OE jako wyjścia
    set_output_enable(1);        // Włącz diody LED (OE niski)

    while (1)
    {
        for (uint8_t i = 0; i <= 9; i++) {
            set_led(digit_to_segment[i]); // Wyświetl cyfrę na wyświetlaczu
            _delay_ms(1000);             // Opóźnienie 1 sekunda
        }
    }
}
