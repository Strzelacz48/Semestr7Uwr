#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <util/delay.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

//Marcin Czapulak

// inicjalizacja UART
void uart_init()
{
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
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

// inicjalizacja ADC
void adc_init()
{
  ADMUX   = _BV(REFS0) | _BV(REFS1); // Napiecie referencyjne 1.1V, wejście ADC0
  DIDR0   = _BV(ADC0D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADEN); // włącz ADC
}

FILE uart_file;

int main()
{
  DDRB |= _BV(PB5);
  PORTB |= _BV(PB5);

  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  // zainicjalizuj ADC
  adc_init();
  // mierz napięcie
  printf("Zadanie 3 - termometr\r\n");
  printf("Napięcie referencyjne: 1.1V\r\n");
  printf("Zakres pomiarowy: 0..60 stopni Celsjusza\r\n");
  printf("Podaj temperaturę w stopniach Celsjusza: ");
  float temp;
  scanf("%f", &temp);
  printf("Zadana temperatura: %f\r\n", temp);
  while(1) {
    ADCSRA |= _BV(ADSC); // wykonaj konwersję
    while (!(ADCSRA & _BV(ADIF))); // czekaj na wynik
    ADCSRA |= _BV(ADIF); // wyczyść bit ADIF (pisząc 1!)
    uint16_t v = ADC; // weź zmierzoną wartość (0..1023)
    printf("Odczytano: %"PRIu16"\r\n", v);
    //Trzbea policzyć ile jest woltów a potem przeliczyć na temperaturę
    float U = ((float)v * 1.1) / 1024.0;
    float T = 100*U - 50;//(U - 0.5) * 100;
    printf("Temperatura: %f\r\n", T);
    _delay_ms(500);

    if (T >= temp) {
      PORTB &= ~_BV(PB5);
      while(T >= temp - 1) {
        ADCSRA |= _BV(ADSC); // wykonaj konwersję
        while (!(ADCSRA & _BV(ADIF))); // czekaj na wynik
        ADCSRA |= _BV(ADIF); // wyczyść bit ADIF (pisząc 1!)
        v = ADC; // weź zmierzoną wartość (0..1023)
        printf("Odczytano: %"PRIu16"\r\n", v);
        //Trzbea policzyć ile jest woltów a potem przeliczyć na temperaturę
        U = ((float)v * 1.1) / 1024.0;
        T = 100*U - 50;//(U - 0.5) * 100;
        printf("Temperatura: %f\r\n", T);
        _delay_ms(500);
      }
      PORTB |= _BV(PB5);
    }
  }
}

