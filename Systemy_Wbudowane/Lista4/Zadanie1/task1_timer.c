#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

#include <stdint.h>

void perform_operations_int8(volatile int8_t a, volatile int8_t b) {
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int8_t wynik1 = a + b;
    uint16_t time_end = TCNT1; // wartość licznika po czekaniu
    printf("\nZmierzony czas dodawania: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int8_t wynik2 = a * b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas mnozenia: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int8_t wynik3 = a / b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas dzielenia: %"PRIu16" cykli\r\n", time_end);
    printf("int8_t: %"PRIi8" + %"PRIi8" = %"PRIi8", %"PRIi8" * %"PRIi8" = %"PRIi8", %"PRIi8" / %"PRIi8" = %"PRIi8"\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

void perform_operations_int16(volatile int16_t a, volatile int16_t b) {
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int16_t wynik1 = a + b;
    uint16_t time_end = TCNT1; // wartość licznika po czekaniu
    printf("\nZmierzony czas dodawania: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int16_t wynik2 = a * b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas mnozenia: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int16_t wynik3 = a / b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas dzielenia: %"PRIu16" cykli\r\n", time_end);
    printf("int16_t: %"PRIi16" + %"PRIi16" = %"PRIi16", %"PRIi16" * %"PRIi16" = %"PRIi16", %"PRIi16" / %"PRIi16" = %"PRIi16"\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

void perform_operations_int32(volatile int32_t a, volatile int32_t b) {
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int32_t wynik1 = a + b;
    uint16_t time_end = TCNT1; // wartość licznika po czekaniu
    printf("\nZmierzony czas dodawania: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int32_t wynik2 = a * b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas mnozenia: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int32_t wynik3 = a / b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas dzielenia: %"PRIu16" cykli\r\n", time_end);
    printf("int32_t: %"PRIi32" + %"PRIi32" = %"PRIi32", %"PRIi32" * %"PRIi32" = %"PRIi32", %"PRIi32" / %"PRIi32" = %"PRIi32"\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

void perform_operations_int64(volatile int64_t a, volatile int64_t b) {
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int64_t wynik1 = a + b;
    uint16_t time_end = TCNT1; // wartość licznika po czekaniu
    printf("\nZmierzony czas dodawania: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int64_t wynik2 = a * b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas mnozenia: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile int64_t wynik3 = a / b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas dzielenia: %"PRIu16" cykli\r\n", time_end);
    //printf("int64_t: %"PRIi64" + %"PRIi64" = %"PRIi64", %"PRIi64" * %"PRIi64" = %"PRIi64", %"PRIi64" / %"PRIi64" = %"PRIi64"\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

void perform_operations_float(volatile float a, volatile float b) {
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile float wynik1 = a + b;
    uint16_t time_end = TCNT1; // wartość licznika po czekaniu
    printf("\nZmierzony czas dodawania: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile float wynik2 = a * b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas mnozenia: %"PRIu16" cykli\r\n", time_end);
    TCNT1 = 0; // wartość licznika przed czekaniem
    volatile float wynik3 = a / b;
    time_end = TCNT1; // wartość licznika po czekaniu
    printf("Zmierzony czas dzielenia: %"PRIu16" cykli\r\n", time_end);
    printf("float: %f + %f = %f, %f * %f = %f, %f / %f = %f\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

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

void timer1_init()
{
  // ustaw tryb licznika
  // WGM1  = 0000 -- normal
  // CS1   = 001  -- prescaler 1
  TCCR1B = _BV(CS10);
}
FILE uart_file;

int main()
{
    // zainicjalizuj UART
    uart_init();
    // skonfiguruj strumienie wejścia/wyjścia
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;
    // zainicjalizuj licznik
    timer1_init();
    // program testowy
    volatile int8_t a8, b8;
    volatile int16_t a16, b16;
    volatile int32_t a32, b32, a64tmp, b64tmp;
    volatile int64_t a64, b64;
    volatile float af, bf;
    
    printf("\nEnter two int8_t numbers: ");
    scanf("%hhd %hhd", &a8, &b8);
    perform_operations_int8(a8, b8);

    printf("\nEnter two int16_t numbers: ");
    scanf("%hd %hd", &a16, &b16);
    perform_operations_int16(a16, b16);

    printf("\nEnter two int32_t numbers: ");
    scanf("%"SCNd32" %"SCNd32"", &a32, &b32);
    perform_operations_int32(a32, b32);

    printf("\nEnter two int64_t numbers: ");
    scanf("%"SCNd32" %"SCNd32, &a64tmp, &b64tmp);
    a64 = a64tmp;
    b64 = b64tmp;
    perform_operations_int64(a64, b64);

    printf("\nEnter two float numbers: ");
    scanf("%f %f", &af, &bf);
    perform_operations_float(af, bf);
}