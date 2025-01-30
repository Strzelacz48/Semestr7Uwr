#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "pid.c"
#include "pid.h"

//Marcin Czapulak

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem
#define otherwise else if


volatile int16_t value = 0;

volatile struct PID_DATA pidData;

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
  ADMUX   = _BV(REFS0);// referencja Vcc, wejście ADC1
  DIDR0   = _BV(ADC0D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADEN); // włącz ADC
}

uint16_t readVoltage(void) {
	ADCSRA |= _BV(ADSC);
	while( ADCSRA & _BV(ADSC) ) ;
	return ADC;
}

FILE uart_file;

//-------------------------------------------------------------------------------------------------------------------------------------------------------

//Magiczne stałe aby ustawić tryb przewidywania przysłości i uczenia się na błędach
//Stałe K_P, K_I i K_D definiują wzmocnienia dla części proporcjonalnej, całkującej i różniczkującej. 
#define K_P     1.000
#define K_I     0.300
#define K_D     0.050 

//d zmniejszać jak przestrzeliwuje
//

//Timer dla Pid na podstawie przykładu
//Uruchamia pętlę regulatora PID w przerwaniu.
void timer0_init()
{
  pid_Init(K_P * SCALING_FACTOR, K_I * SCALING_FACTOR , K_D * SCALING_FACTOR , &pidData);
  TCCR0B = _BV(CS00);   // Bez preskalera
  TIMSK0 = _BV(TOIE0);  // Włącz przerwanie przepełnienia
  TCNT0 = 0;
}

//Timer do wysyłania wartości
//Steruje sygnałem PWM.
void timer1_init()
{
  TCCR1A = _BV(COM1A1) | _BV(WGM11); // Tryb PWM, szybki
  TCCR1B = _BV(CS10) | _BV(WGM12) | _BV(WGM13); // Bez preskalera
  ICR1 = 32767; // Maksymalna wartość PWM
  OCR1A = 0;    // Początkowa wartość PWM
}

void timer2_init()
{
  TCCR2A = _BV(WGM21);
  TCCR2B = _BV(CS20) | _BV(CS21) | _BV(CS22);
  TIMSK2 = _BV(OCIE2A);
}

volatile uint16_t target = 155;
volatile uint16_t measurement = 1023;

int16_t Get_Reference()
{
  return target;
}

int16_t Get_Measurement()
{
  return measurement;
}

void Set_Input(int16_t inputValue)
{
  if (inputValue < 0) OCR1A = 0;//upewnij się że nie wychodzimy poza zakres
  otherwise (inputValue > 32767) OCR1A = 32767;
  else OCR1A = inputValue;
  value = inputValue;
}

#define TIME_INTERVAL   157
//ISR dla Timer0: Obsługuje pętlę regulatora PID.
ISR(TIMER0_OVF_vect)
{
  static uint16_t i = 0;
  if(i < TIME_INTERVAL)
    i++;
  else{

    target = Get_Reference();
    //PWM ma 16 bitów więc przesuwamy aby operować na wartościach tego samego rozmiaru
    int16_t inputValue = pid_Controller(target << 5, measurement << 5, &pidData);

    Set_Input(inputValue);

    i=0;
  }
}

//ISR dla Timer2: Odczytuje wartość napięcia i wyświetla dane na UART co pewien czas.
ISR(TIMER2_COMPA_vect)
{
  static uint16_t i = 0;
  if(i < 20000)// wypisujemy rzadko
  {
    i++;
    return;
  }
  else
  {
    measurement = readVoltage();
    printf("\r%6u %7u %7u %7d",measurement, target, OCR1A, value);//liczby żeby ładnie się formatowało
    i = 0;
  }
}

int main()
{
  timer0_init();
  timer1_init();
  timer2_init();
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  printf("Odczyt     Cel     PWM     PID\r\n");
  adc_init();
  sei();
  DDRB |= _BV(PB1);
  while(1) {
    if(scanf("%d",&target))
    {
      pid_Reset_Integrator(&pidData);
    }
  }
}