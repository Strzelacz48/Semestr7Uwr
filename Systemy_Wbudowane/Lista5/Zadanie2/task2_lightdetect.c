//Marcin Czapulak
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>

#define BAUD 9600                         // Baud rate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)  // zgodnie ze wzorem

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
  while (!(UCSR0A & _BV(UDRE0)));
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

// oczekiwanie na zakończenie transmisji
void uart_wait()
{
  while(!(UCSR0A & _BV(TXC0)));
}

volatile uint16_t resistance = 0;
volatile uint8_t start_conversion = 0;

// Initialize ADC
void adc_init()
{
  ADMUX = _BV(REFS0); // Reference AVcc, input ADC0 (PC0)
  DIDR0 = _BV(ADC0D); // Disable digital input on ADC0
  // Set ADC clock prescaler to 128 for 125kHz ADC clock (16MHz / 128)
  ADCSRA = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);
  ADCSRA |= _BV(ADEN); // Enable ADC
  ADCSRA |= _BV(ADIE); // Enable ADC interrupt
}

// Initialize external interrupt for button on PD2 (INT0)
void interrupt_init()
{
    // Configure INT0 for falling edge trigger
    EICRA = _BV(ISC01); // Trigger on falling edge
    EIMSK = _BV(INT0);  // Enable external interrupt INT0

    // Configure PD2 as input with pull-up for button
    DDRD &= ~_BV(PD2);   // Set PD2 as input
    PORTD |= _BV(PD2);   // Enable pull-up on PD2
}

FILE uart_file;

// INT0 interrupt service routine - triggers ADC conversion
ISR(INT0_vect)
{
    // Start ADC conversion
    ADCSRA |= _BV(ADSC);
}

// ADC conversion complete interrupt service routine
ISR(ADC_vect)
{
    uint16_t adc_value = ADC;
    
    resistance = 10000 * (adc_value / (1023.0 - ADC));
}

int main(void)
{
    
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    // Initialize ADC and pin change interrupt
    adc_init();
    interrupt_init();

    // Enable global interrupts
    sei();

    while(1)
    {
        // Print the resistance value over UART
        printf("ADC : %u\r\n",ADC);
        printf("opór: %u\r\n", resistance);
        uart_wait();

        // Small delay to avoid overwhelming output
        _delay_ms(80);
    }
}
