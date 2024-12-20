#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <util/delay.h>
#include <stdlib.h>  // Dodać tę bibliotekę, aby używać dtostrf

#define BAUD 9600                          // Baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // Wzór na ustawienie baudrate

// Definicje dla diody LED (na przykładzie PB0)
#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_PIN PB5

// Inicjalizacja UART
void uart_init() {
    UBRR0 = UBRR_VALUE;                    // Ustawienie baudrate
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);      // Włączenie odbiornika i nadajnika
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);    // Format 8n1
}

// Funkcja transmisji jednego znaku przez UART
int uart_transmit(char data, FILE *stream) {
    while (!(UCSR0A & _BV(UDRE0)));        // Czekaj aż bufor będzie pusty
    UDR0 = data;
    return 0;
}

// Funkcja odbioru jednego znaku przez UART
int uart_receive(FILE *stream) {
    while (!(UCSR0A & _BV(RXC0)));         // Czekaj aż znak dostępny
    return UDR0;
}

// Inicjalizacja ADC
void adc_init() {
    ADMUX = _BV(REFS0) | _BV(REFS1); // Napięcie referencyjne = AVcc, wejście = wewnętrzne napięcie 1.1V
    DIDR0 = _BV(ADC0D);              // Wyłączenie wejścia cyfrowego na ADC0
    ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
    ADCSRA |= _BV(ADEN);            // Włącz ADC
}

FILE uart_file;

int main() {
    // Inicjalizacja UART i ADC
    uart_init();
    adc_init();

    // Konfiguracja strumieni I/O
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    // Ustawienie pinu LED jako wyjścia
    //LED_DDR |= _BV(LED_PIN);

    // Pętla pomiaru napięcia zasilania
    while (1) {
        // Włącz diodę LED, zmierz napięcie, wyłącz diodę LED
        //LED_PORT |= _BV(LED_PIN);           // Włącz diodę LED
        _delay_ms(100);

        ADCSRA |= _BV(ADSC);                // Rozpocznij konwersję ADC
        while (!(ADCSRA & _BV(ADIF)));      // Czekaj na zakończenie konwersji
        ADCSRA |= _BV(ADIF);                // Wyczyść bit ADIF

        uint16_t adc_value = ADC;           // Odczytaj wartość ADC
        //LED_PORT &= ~_BV(LED_PIN);          // Wyłącz diodę LED

        // Przeliczanie napięcia zasilającego na podstawie pomiaru wewnętrznego napięcia 1.1V
        float vcc = (1.1 * 1024) / adc_value;

        // Konwersja float na string
        char buffer[10];
        dtostrf(vcc, 6, 2, buffer);  // 6 - długość minimalna, 2 - liczba miejsc po przecinku

        // Wypisz wartość napięcia zasilającego
        printf("Napięcie zasilające: %s V\r\n", buffer);
        
        _delay_ms(1000); // Odstęp między pomiarami
    }
}