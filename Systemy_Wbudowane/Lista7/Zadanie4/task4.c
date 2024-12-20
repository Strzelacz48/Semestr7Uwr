#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/delay.h>

//Marcin Czapulak
#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

#define SS DDD6
#define SCK DDD7
#define MISO DDB4
#define MOSI DDD5

// inicjalizacja UART
void uart_init()
{
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
  // wyczyść rejestr UCSR0A
  UCSR0A = 0;
  // włącz odbiornik i nadajnik
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  // ustaw format 8n1
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// transmisja jednego znaku
int uart_transmit(char data, FILE *stream)
{
  // czekaj aż transmiter gotowy
  while(!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
  return 0;
}

// odczyt jednego znaku
int uart_receive(FILE *stream)
{
  // czekaj aż znak dostępny
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

FILE uart_file;

// inicjalizacja SPI
void spi_slave_init()
{
    // ustaw pin MISO jako wyjście
    DDRB |= _BV(MISO);
    // włącz SPI w trybie slave z zegarem 125 kHz
    SPCR = _BV(SPE);
}

void spi_master_init()
{
    DDRD |= _BV(MOSI) | _BV(SCK) | _BV(SS);
    PORTD |= _BV(SS);
}

// transfer jednego bajtu
uint8_t spi_receive()
{
    // czekaj na ukończenie transmisji
    // while (!(SPSR & _BV(SPIF))); //czekanie jest opcjonalne
    // wyczyść flagę przerwania
    // SPSR |= _BV(SPIF); //czyszczenie bitu przerwania jest opcjonalne
    // zwróć otrzymane dane
    return SPDR;
}

uint8_t spi_transmit(uint8_t data)
{
    PORTD &= ~_BV(SS);
    PORTD &= ~_BV(SCK);
    uint8_t incoming_data = 0;
    for (int8_t i = 7; i >= 0; i--)
    {
        if ((data & (1 << i)) != 0)
        {
            PORTD |= _BV(MOSI);
        }
        else
        {
            PORTD &= ~_BV(MOSI);
        }
        incoming_data |= ((PIND & _BV(MISO))!=0) << i;
        PORTD |= _BV(SCK);
        PORTD &= ~_BV(SCK);
    }
    PORTD |= _BV(SCK);
    PORTD |= _BV(SS);
    return incoming_data;
}

int main()
{
    // zainicjalizuj UART
    uart_init();
    // skonfiguruj strumienie wejścia/wyjścia
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;
    // zainicjalizuj SPI
    spi_slave_init();
    spi_master_init();
    // program testujący połączenie
    uint8_t v = 0;
    while(1) {
        SPDR = v;
        uint8_t incoming_data = spi_transmit(v);
        uint8_t w = spi_receive();
        printf("Wysłano: %"PRId8" Odczytano master: %"PRId8" Odczytano slave: %"PRId8"\r\n", v, w, incoming_data);
        v++;
        _delay_ms(100);
    }
}
