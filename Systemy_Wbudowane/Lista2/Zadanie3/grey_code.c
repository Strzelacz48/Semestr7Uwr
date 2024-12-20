#include <avr/io.h>
#include <util/delay.h>

// Definicje pinów
#define RESET_BTN_PIN    PB4
#define NEXT_BTN_PIN     PC2
#define PREV_BTN_PIN     PC3

// Maski bitowe do kontroli portów
#define RESET_BTN_MASK   _BV(RESET_BTN_PIN)
#define NEXT_BTN_MASK    _BV(NEXT_BTN_PIN)
#define PREV_BTN_MASK    _BV(PREV_BTN_PIN)

// Debounce settings
#define DEBOUNCE_DELAY   50

typedef unsigned int uint;

// Funkcja konwertująca liczbę binarną na kod Graya (Przesunięcie bitowe i operacja XOR)
uint8_t BinaryToGray(uint8_t num) {
    return num ^ (num >> 1);
}

uint8_t current_index = 0;

// Funkcja inicjalizująca porty
void init_ports(void) {
    // Ustawienie PD0-PD7 jako wyjścia dla diod LED
    DDRD = 0xFF;
    PORTD = 0x00; // Początkowo wszystkie diody są wyłączone

    // Ustawienie przycisków jako wejścia z podciąganiem
    DDRB &= ~RESET_BTN_MASK; // PB4 jako wejście
    PORTB |= RESET_BTN_MASK; // Włącz pull-up na PB4

    DDRC &= ~(NEXT_BTN_MASK | PREV_BTN_MASK); // PC2 i PC3 jako wejścia
    PORTC |= NEXT_BTN_MASK | PREV_BTN_MASK;   // Włącz pull-up na PC2 i PC3
}

// Funkcja debouncingu dla pojedynczego przycisku z kontrolą puszczenia
uint8_t debounce(volatile uint8_t *port, uint8_t pin_mask) {
    if (!(*port & pin_mask)) {  // Sprawdź, czy przycisk jest wciśnięty
        _delay_ms(DEBOUNCE_DELAY);  // Opóźnienie dla debounce
        if (!(*port & pin_mask)) {  // Sprawdź ponownie, czy przycisk jest wciśnięty
            while (!(*port & pin_mask)); // Czekaj, aż przycisk zostanie puszczony
            _delay_ms(DEBOUNCE_DELAY); // Kolejny debounce po puszczeniu
            return 1; // Przyciśnięcie potwierdzone
        }
    }
    return 0; // Brak wciśnięcia
}

// Funkcja resetująca stan diod LED
void reset_leds(void) {
    current_index = 0;
    PORTD = BinaryToGray(current_index);  // Ustaw kod Graya na diodach
}

// Funkcja zmieniająca na następny kod Graya
void next_gray_code(void) {
    current_index = (current_index == 255) ? 0 : (current_index + 1);
    PORTD = BinaryToGray(current_index);  // Ustaw kod Graya na diodach
}

// Funkcja zmieniająca na poprzedni kod Graya
void prev_gray_code(void) {
    current_index = (current_index == 0) ? 255 : (current_index - 1);
    PORTD = BinaryToGray(current_index);  // Ustaw kod Graya na diodach
}

int main(void) {
    init_ports();
    reset_leds();

    while (1) {
        // Obsługa przycisku resetującego
        if (debounce(&PINB, RESET_BTN_MASK)) {
            reset_leds();
        }
        // Obsługa przycisku zmieniającego na następny kod Graya
        if (debounce(&PINC, NEXT_BTN_MASK)) {
            next_gray_code();
        }
        // Obsługa przycisku zmieniającego na poprzedni kod Graya
        if (debounce(&PINC, PREV_BTN_MASK)) {
            prev_gray_code();
        }
    }

    return 0;
}
