#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PB4
#define BTN_PIN PINB
#define BTN_PORT PORTB

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // Wartość UBRR dla ustawienia prędkości transmisji

#define DOT_THRESHOLD 600       // Próg dla kropki w ms
#define DASH_THRESHOLD 3000     // Próg dla kreski w ms
#define CHAR_GAP 600
#define WORD_GAP 3000

// Deklaracja funkcji UART
void uart_init();
int uart_transmit(char data, FILE *stream);
int uart_receive(FILE *stream);

FILE uart_file;

void init() {
    // Konfiguracja przycisku jako wejścia z podciąganiem
    BTN_PORT |= _BV(BTN);
    // Konfiguracja diody LED jako wyjścia
    LED_DDR |= _BV(LED);
}

// Funkcja sprawdzająca stan przycisku
uint8_t is_button_pressed() {
    return !(BTN_PIN & _BV(BTN));
}

// Włączenie LED
void signal_led_on() {
    LED_PORT |= _BV(LED);
}

// Wyłączenie LED
void signal_led_off() {
    LED_PORT &= ~_BV(LED);
}

// Tabela tłumaczenia Morse'a na znaki ASCII
const char* morse_table[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",  // A-J
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",   // K-T
    "..-", "...-", ".--", "-..-", "-.--", "--.."                           // U-Z
};

// Funkcja tłumacząca kod Morse'a na znak ASCII
char morse_to_char(const char* morse) {
    for (int i = 0; i < 26; i++) {
        if (strcmp(morse, morse_table[i]) == 0) {
            return 'A' + i;
        }
    }
    return ' '; // Znak nieznany jezeli nie rozpoznamy znaku zakladamy ze chodzilo o spacje
}

int main() {
    
    uart_init();
    init();
    
    // Konfiguracja strumieni wejścia/wyjścia dla UART
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    char morse_code[10]; // Bufor do przechowywania kodu Morse'a
    int morse_index = 0;
    unsigned long press_time = 0;
    unsigned long release_time = 0;

    while (1) {
        if (is_button_pressed()) {
            if (press_time == 0) {
                press_time = 1;
                _delay_ms(1);
            } else {
                press_time += 1;
                _delay_ms(1);
            }

            // Sygnalizacja kreski po przekroczeniu DOT_THRESHOLD
            if (press_time > DOT_THRESHOLD) {
                signal_led_on();
            }
        } else {
            if (press_time > 0) { // Jeśli przycisk był wciśnięty
                if (press_time < DOT_THRESHOLD) {
                    morse_code[morse_index++] = '.';
                } else if (press_time < DASH_THRESHOLD) {
                    morse_code[morse_index++] = '-';
                }

                press_time = 0; // Reset czasu naciśnięcia
                release_time = 0;
                signal_led_off();
            } else if (morse_index > 0) { // Jeśli naciśnięcie się zakończyło
                release_time += 1;
                _delay_ms(1);

                if (release_time > WORD_GAP) {
                    printf(" "); // Dodaj spację między słowami
                    release_time = 0; // Resetuj czas przerwy
                    morse_index = 0;  // Zresetuj bufor kodu Morse'a
                } else if (release_time > CHAR_GAP && morse_index > 0) {
                    morse_code[morse_index] = '\0'; // Zakończ kod Morse'a
                    char decoded_char = morse_to_char(morse_code);
                    printf("%c", decoded_char);
                    morse_index = 0; // Zresetuj indeks Morse'a
                }
            }
        }
    }

    return 0;
}

// Inicjalizacja UART
void uart_init() {
    // Ustaw baudrate
    UBRR0 = UBRR_VALUE;
    // Wyczyść rejestr UCSR0A
    UCSR0A = 0;
    // Włącz odbiornik i nadajnik
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    // Ustaw format 8n1
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// Transmisja jednego znaku przez UART
int uart_transmit(char data, FILE *stream) {
    // Czekaj aż transmiter będzie gotowy
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = data;
    return 0;
}

// Odbiór jednego znaku przez UART
int uart_receive(FILE *stream) {
    // Czekaj aż znak będzie dostępny
    while (!(UCSR0A & _BV(RXC0)));
    return UDR0;
}
