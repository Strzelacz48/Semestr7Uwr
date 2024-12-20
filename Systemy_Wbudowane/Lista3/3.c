#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

uint16_t exp_table[64] = {0,1,1,2,2,2,3,3,3,4,5,6,7,8,9,10,12,14,17,19,23,27,31,36,43,50,58,68,79,93,109,127,148,174,203,237,277,324,379,443,518,606,708,828,968,1131,1323,1547,1808,2114,2471,2889,3378,3949,4617,5398,6311,7378,8626,10084,11790,13784,16115,18840};

void uart_init()
{
  UBRR0 = UBRR_VALUE;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart_transmit(char data, FILE *stream)
{
  while(!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
  return 0;
}

int uart_receive(FILE *stream)
{
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

void adc_init()
{
  ADMUX   = _BV(REFS0); // referencja AVcc, wejście ADC0
  DIDR0   = _BV(ADC0D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADEN); // włącz ADC
  //ADCSRA |= _BV(ADATE);
}

uint16_t readVoltage(void) {
	ADCSRA |= _BV(ADSC);
	while( ADCSRA & _BV( ADSC) );

	return ADC ;

}

FILE uart_file;

int main(void)
{
  adc_init();
  DDRB |= _BV(PORTB5);
  uart_init();
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  while(1)
  {
    uint16_t vccx10 = readVoltage();
    uint16_t ex = exp_table[vccx10 >> 4];
    for(uint8_t j = 0; j < 10; j++)
    {
      PORTB |= _BV(PORTB5);
      for (uint16_t i=0; i<ex;i++)
      {
        _delay_us(0.01);
      }
      PORTB &= ~_BV(PORTB5);
      for (uint16_t i = ex; i<=18840; i++)
      {
        _delay_us(0.01);
      }
    }
	}
}
