#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN PB2        // Dioda zapala się na zbliżenie
#define IR_LED_PIN PB1     // Dioda IR, generująca sygnał
#define IR_DETECT_PIN PB0  // Wejście z detektora IR
#define CYCLE_DELAY 100    // Czas pomiędzy cyklami impulsów w ms

// Funkcja inicjalizująca timer dla częstotliwości 37,9 kHz
void init_timer() {
    TCCR0A |= _BV(WGM01);               // Tryb CTC
    TCCR0B |= _BV(CS00);                // Prescaler ustawiony na 1
    OCR0A = (F_CPU / 37900 / 2) - 1;      // Ustawienie na częstotliwość 37,9 kHz
}

// Funkcja generująca impuls 37,9 kHz przez 600 µs
void transmit_burst() {
    for (uint8_t i = 0; i < 8; i++) {     // 8 okresów 600 µs
        PORTB ^= _BV(IR_LED_PIN);       // Włączenie i wyłączenie diody IR
        _delay_us(13);                    // Pół okresu dla 37,9 kHz ~ 13 µs
    }
}

// Funkcja detekcji zbliżenia
uint8_t check_proximity() {
    uint8_t signal = PINB & _BV(IR_DETECT_PIN);
    return signal == 0;                   // Zwraca 1, jeśli jest odbicie
}

int main(void) {
    DDRB |= _BV(LED_PIN) | _BV(IR_LED_PIN);  // Ustawienie LED i IR_LED jako wyjścia
    DDRB &= ~_BV(IR_DETECT_PIN);               // IR_DETECT jako wejście

    init_timer();

    while (1) {
        // Wysłać sześć impulsów 600 us, z przerwami 600 us
        for (uint8_t i = 0; i < 6; i++) {
            transmit_burst();    // Sygnał
            _delay_us(600);      // Przerwa pomiędzy impulsami
        }
        
        // Oczekiwanie 100 ms na zakończenie cyklu
        _delay_ms(CYCLE_DELAY);

        // Sprawdzanie detektora IR
        if (check_proximity()) {
            PORTB |= _BV(LED_PIN);  // Zapalenie LED przy wykryciu odbicia
        } else {
            PORTB &= ~_BV(LED_PIN); // Zgaszenie LED, gdy brak odbicia
        }
    }
}
