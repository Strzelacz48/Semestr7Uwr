#include <avr/io.h>
#include <util/delay.h>
#include "hd44780.h"

//324279 Marcin Czapulak

// Rozmiary wyświetlacza
#define LCD_WIDTH 16

// Funkcja wgrywająca nowe znaki do CGRAM
void LCD_LoadCustomChars(void) {
    uint8_t customChars[6][8] = {
        {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000}, // 0 wypełnień
        {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000}, // 1 wypełnienie
        {0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000}, // 2 wypełnienia
        {0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100}, // 3 wypełnienia
        {0b11110, 0b11110, 0b11110, 0b11110, 0b11110, 0b11110, 0b11110, 0b11110}, // 4 wypełnienia
        {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111}  // 5 wypełnień
    };

    for (uint8_t i = 0; i < 6; i++) {
        LCD_WriteCommand(HD44780_CGRAM_SET | (i << 3)); // Ustaw adres CGRAM
        for (uint8_t j = 0; j < 8; j++) {
            LCD_WriteData(customChars[i][j]); // Wgraj dane znaku
        }
    }
}

// Funkcja wyświetlająca tekst "ProgressInProgress" w górnym wierszu
void LCD_DisplayStaticText(void) {
    LCD_GoTo(0, 0); // Ustaw kursor na początek pierwszego wiersza
    char text[] = "ProgresInProgres"; // Maksymalnie 16 znaków na wiersz
    for (uint8_t i = 0; i < 16; i++) {
        LCD_WriteData(text[i]); // Wyświetl tekst
    }
}

// Funkcja rysująca pasek postępu w dolnym wierszu
void LCD_DrawProgressBar(uint8_t progress) {
    uint8_t fullChars = progress / 5;     // Liczba pełnych znaków
    uint8_t remainder = progress % 5;    // Pozostałe wypełnienie

    // Wyświetlenie pełnych znaków
    LCD_GoTo(0, 1); // Ustaw kursor na początek drugiego wiersza
    for (uint8_t i = 0; i < fullChars; i++) {
        LCD_WriteData(5); // Pełny znak (5 wypełnień)
    }

    // Wyświetlenie częściowego znaku
    if (remainder > 0) {
        LCD_WriteData(remainder); // Częściowy znak (0-4 wypełnień)
        fullChars++;
    }

    // Wypełnienie reszty paska spacjami
    for (uint8_t i = fullChars; i < LCD_WIDTH; i++) {
        LCD_WriteData(' '); // Puste pole
    }
}

// Funkcja główna
int main() {
    // Inicjalizacja LCD
    LCD_Initialize();
    LCD_Clear();
    LCD_LoadCustomChars();

    uint8_t progress = 0;

    // Wyświetl stały tekst w górnym wierszu
    LCD_DisplayStaticText();

    // Pętla główna
    while (1) {
        LCD_DrawProgressBar(progress); // Rysuj pasek postępu
        progress = (progress + 1) % 81; // Zwiększ postęp (od 0 do 80)
        _delay_ms(100); // Opóźnienie
    }
}
