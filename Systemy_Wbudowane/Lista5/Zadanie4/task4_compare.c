//Marcin Czapulak
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

void uart_init()
{
  UBRR0 = UBRR_VALUE;
  UCSR0A = 0;
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

FILE uart_file;

void timer_init()
{
  TCCR1B = _BV(CS11) | _BV(CS10);
  TCCR1B |= _BV(ICES1); 
}

volatile uint16_t last_capture = 0;  // Last Input Capture value
volatile uint16_t period_ticks = 0;  // Time interval between edges (in timer ticks)
volatile uint8_t new_measurement = 0;

ISR(TIMER1_CAPT_vect)
{
  uint16_t current_capture = ICR1;  // Read the captured timer value
  period_ticks = current_capture - last_capture;  // Calculate time interval
  last_capture = current_capture;  // Update the last capture value
  new_measurement = 1;             // Set flag for new measurement
}

void interrupt_init()
{
  TIMSK1 = _BV(ICIE1);
}

int main(void)
{
  timer_init();
  uart_init();
  interrupt_init();
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  sei();
  set_sleep_mode(SLEEP_MODE_IDLE);
  printf("Start\n");
  while(1)
  {
    sleep_mode();
    if (new_measurement)
    {
      new_measurement = 0;  // Clear the measurement flag

      // Calculate frequency: F = F_CPU / (Prescaler * period_ticks) (16 000 000.0 / (Prescaler * period_ticks) )
      if (period_ticks > 0)
      {
        //float frequency = (15625.0 / period_ticks); //_BV(CS12) | _BV(CS10);
        float frequency = (250000.0 / period_ticks); //_BV(CS11) | _BV(CS10);
        printf("Frequency: %.2f Hz\r\n", frequency);
      }
    }
  }
}