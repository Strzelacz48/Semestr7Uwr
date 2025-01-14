#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <avr/interrupt.h>
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
  // rozpocznij transmisję '3' to flagi ..11
  SPDR = (3 << 4) | (data >> 4);
  // czekaj na ukończenie transmisji
  while (!(SPSR & _BV(SPIF)));
  SPSR |= _BV(SPIF);
  SPDR = data << 4;
  // czekaj na ukończenie transmisji
  while (!(SPSR & _BV(SPIF)));
  // wyczyść flagę przerwania
  SPSR |= _BV(SPIF);
}

volatile uint16_t v = 0;
ISR(TIMER1_COMPA_vect)
{
  PORTB &= ~_BV(SS);
  spi_transfer(pgm_read_byte(&dzwiek_raw[++v]));
  PORTB |= _BV(SS);
  v%=dzwiek_raw_len;
}

void timer_init()
{
  TCCR1B = _BV(CS11) | _BV(WGM12);
  OCR1A = 249;//dźwięk 8KHz
  TIMSK1 = _BV(OCIE1A); // włącz przerwanie od porównania
}

int main()
{
  timer_init();
  spi_init();
  DDRB |= _BV(SS);
  sei();
  while(1) {

  }
}