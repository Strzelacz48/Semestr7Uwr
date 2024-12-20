#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define BUZZ PB5
#define BUZZ_DDR DDRB
#define BUZZ_PORT PORTB

// Makro do generowania tonu
#define TONE(step, delay) \
    for (uint16_t i = 0; i < (uint32_t)1000 * (delay) / (step) / 2; i++) { \
      BUZZ_PORT |= _BV(BUZZ); \
      custom_dely(delay); \
      BUZZ_PORT &= ~_BV(BUZZ); \
      custom_dely(delay); \
    }

// Nuty używane w melodii (częstotliwości w mikrosekundach)
#define NOTE_C4  1911/8
#define NOTE_D4  1703/8
#define NOTE_E4  1517/8
#define NOTE_F4  1432/8
#define NOTE_G4  1276/8
#define NOTE_A4  1136/8
#define NOTE_B4  1012/8
#define NOTE_C5  956/8

// Struktura przechowująca nutę
typedef struct {
    uint16_t frequency;
    uint16_t duration;
} Note;

// Melodia "Wlazł kotek na płotek" zapisana w pamięci programu (flash)
const Note melody[] PROGMEM = {
    {NOTE_C4, 500}, {NOTE_D4, 500}, {NOTE_E4, 500}, {NOTE_C4, 500},
    {NOTE_C4, 500}, {NOTE_D4, 500}, {NOTE_E4, 500}, {NOTE_C4, 500},
    {NOTE_E4, 500}, {NOTE_F4, 500}, {NOTE_G4, 1000},
    {NOTE_E4, 500}, {NOTE_F4, 500}, {NOTE_G4, 1000},
    {NOTE_G4, 250}, {NOTE_A4, 250}, {NOTE_G4, 250}, {NOTE_F4, 250}, {NOTE_E4, 500}, {NOTE_C4, 500},
    {NOTE_G4, 250}, {NOTE_A4, 250}, {NOTE_G4, 250}, {NOTE_F4, 250}, {NOTE_E4, 500}, {NOTE_C4, 500},
    {NOTE_C4, 500}, {NOTE_G4, 500}, {NOTE_C4, 1000},
};

void custom_dely(uint16_t duration){
    for(int i = 0; i < duration; i++)
        _delay_ms(1);
    return;
}

// Funkcja odtwarzająca nutę
void play_note(uint16_t frequency, uint16_t duration) {
    if (frequency == 0) {
        custom_dely(duration); // Pauza
    } else {
        TONE(frequency, duration); // Generowanie tonu
    }
}

int main() {
    BUZZ_DDR |= _BV(BUZZ); // Ustawienie pinu brzęczyka jako wyjścia

    while (1) {
        // Odtwarzanie melodii
        for (uint8_t i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
            Note current_note;
            memcpy_P(&current_note, &melody[i], sizeof(Note)); // Pobranie nuty z pamięci programu
            play_note(current_note.frequency, current_note.duration); // Odtworzenie nuty
        }
        _delay_ms(1000); // Pauza przed ponownym odtworzeniem melodii
    }
}
