
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

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

void adc_init()
{
  ADMUX   = _BV(REFS0); // referencja AVcc, wejście ADC0
  DIDR0   = _BV(ADC0D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADEN); // włącz ADC
}

uint16_t readVccVoltage(void) {
	ADCSRA |= _BV(ADSC);
	while( ADCSRA & _BV(ADSC) );		// 0<= result <=1023
	return ADC;
}

volatile uint16_t v1 = 0;
volatile uint16_t v2 = 0;

ISR(TIMER1_CAPT_vect)
{
  ADMUX |= _BV(MUX0) || _BV(MUX1) || _BV(MUX2);//7
  v1 = readVccVoltage();
  ADMUX &= 0xF0;
  TIMSK1 &= ~_BV(ICIE1);
}

ISR(TIMER1_OVF_vect)
{
  ADMUX |= _BV(MUX0) || _BV(MUX1) || _BV(MUX2);//7
  v2 = readVccVoltage();
  ADMUX &= 0xF0;
  TIMSK1 &= ~_BV(TOIE1);
}

void timer_init()
{
  TCCR1A = _BV(COM1A1);
  TCCR1B = _BV(CS11) | _BV(WGM13);
  OCR1A = 0;
  ICR1 = 3069;
  TIMSK1 |= _BV(ICIE1) | _BV(TOIE1);
}

FILE uart_file;

int main()
{
  uint8_t i = 0;
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  adc_init();
  timer_init();
  DDRB |= _BV(PB1);

  while(1) {
    OCR1A = readVccVoltage()*3;
    if(i == 5)
    {
      TIFR1&= 1;
      
      TIMSK1 |= _BV(ICIE1) | _BV(TOIE1);
      TIFR1 |= _BV(ICF1) | _BV(TOV1);
      sei();
      _delay_ms(50);
      cli();
    }
    else _delay_ms(50);
    i++;
    if(i == 7) i=1;
    printf("\r%4hhu miliwoltów  %4hhu miliwoltów ", (uint16_t)(5000 - v1 * 4.887585532746823), (uint16_t)(v2 * 4.887585532746823));
  }
}