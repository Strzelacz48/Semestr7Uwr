#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
//Marcin Czapulak
// Ustawienia UART
#define BAUD 9600                           // Baud rate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem


void uart_init()
{
    UBRR0 = UBRR_VALUE;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0); // Włącz odbiornik, nadajnik i przerwanie odbioru
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// Przerwanie odbioru znaku przez UART
ISR(USART_RX_vect)
{
    char received = UDR0;   // Odczytaj odebrany znak
    while (!(UCSR0A & _BV(UDRE0))); // Poczekaj, aż bufor nadawania będzie pusty
    UDR0 = received;        // Wyślij odebrany znak z powrotem
}

int main(void)
{
    uart_init();            // Inicjalizacja UART
    sei();                  // Włącz globalne przerwania

    set_sleep_mode(SLEEP_MODE_IDLE);

    while (1)
    {
        sleep_mode();
    }
}
