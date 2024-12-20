#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "dzwiek.c"
//Marcin Czapulak
#define SS PB2

// inicjalizacja SPI
void spi_init()
{
  // ustaw piny MOSI, SCK i ~SS jako wyjścia
  DDRB |= _BV(DDB3) | _BV(DDB5) | _BV(DDB2);
  // włącz SPI w trybie master z zegarem 250 kHz
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
}

// transfer jednego bajtu
void spi_transfer(uint8_t data)
{
  PORTB &= ~_BV(SS);
  // rozpocznij transmisję '3' to flagi ..11
  SPDR = (3 << 4) | (data >> 4);
  // czekaj na ukończenie transmisji
  while (!(SPSR & _BV(SPIF)));
  SPDR = (data & 0x0F) << 4;
  // czekaj na ukończenie transmisji
  while (!(SPSR & _BV(SPIF)));
  // wyczyść flagę przerwania
  SPSR |= _BV(SPIF);
  PORTB |= _BV(SS);
}

int main()
{
  // zainicjalizuj SPI
  spi_init();
  unsigned int v = 0;
  while(v < dzwiek_raw_len) {
    spi_transfer(pgm_read_byte(&dzwiek_raw[v]));
    v++;
    _delay_ms(10);
  }
}

