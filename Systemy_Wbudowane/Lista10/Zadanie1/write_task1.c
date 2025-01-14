#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>
#include "hd44780.h"

//324279 Marcin Czapulak

#define LCD_WIDTH 16
#define LCD_HEIGHT 2

int hd44780_transmit(char data, FILE *stream)
{
  LCD_WriteData(data);
  return 0;
}

FILE hd44780_file;

// Bufor na aktualną zawartość wyświetlacza
char lcd_buffer[LCD_HEIGHT][LCD_WIDTH];

// Pozycja kursora
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

// Funkcja inicjalizująca UART
void UART_Init(uint16_t ubrr_value) {
    // Ustawienie prędkości transmisji
    UBRR0H = (ubrr_value >> 8);
    UBRR0L = ubrr_value;
    // Włączenie transmisji i odbioru
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Ustawienie formatu ramki: 8 bitów danych, 1 bit stopu
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Funkcja odbierająca znak przez UART
char UART_Receive(void) {
    while (!(UCSR0A & (1 << RXC0))); // Czekaj na odebranie danych
    return UDR0;
}

// Funkcja aktualizująca wyświetlacz na podstawie bufora
void LCD_UpdateBuffer(void) {
    for (uint8_t y = 0; y < LCD_HEIGHT; y++) {
        LCD_GoTo(0, y); // Przejdź do początku wiersza
        for (uint8_t x = 0; x < LCD_WIDTH; x++) {
            LCD_WriteData(lcd_buffer[y][x]);
        }
    }
    // Ustaw kursor na aktualnej pozycji
    LCD_GoTo(cursor_x, cursor_y);
}

// Funkcja przesuwająca zawartość drugiego wiersza do pierwszego
void LCD_Scroll(void) {
    for (uint8_t x = 0; x < LCD_WIDTH; x++) {
        lcd_buffer[0][x] = lcd_buffer[1][x];
        lcd_buffer[1][x] = ' ';
    }
    LCD_UpdateBuffer();
}

int main() {
    UART_Init(103); // Dla 9600 bps przy zegarze 16 MHz
    LCD_Initialize();
    LCD_Clear();
    LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_ON | HD44780_CURSOR_BLINK); // Włącz LCD, kursor i mruganie kursora
    
    // skonfiguruj strumienie wyjściowe
    fdev_setup_stream(&hd44780_file, hd44780_transmit, NULL, _FDEV_SETUP_WRITE);
    stdout = stderr = &hd44780_file;

    // Inicjalizacja bufora
    for (uint8_t y = 0; y < LCD_HEIGHT; y++) {
        for (uint8_t x = 0; x < LCD_WIDTH; x++) {
            lcd_buffer[y][x] = ' ';
        }
    }
    LCD_UpdateBuffer();

    while (1) {
        char c = UART_Receive();

        // Obsługa entera
        if (c == '\r') {
            cursor_x = 0;
            cursor_y = (cursor_y + 1) % LCD_HEIGHT;
            if (cursor_y == 0) {
                LCD_Scroll();
            }
            LCD_UpdateBuffer();
            continue;
        }

        lcd_buffer[cursor_y][cursor_x] = c;

        LCD_UpdateBuffer();

        // Przesuń kursor
        cursor_x++;
        if (cursor_x >= LCD_WIDTH) {
            cursor_x = 0;
            cursor_y = (cursor_y + 1) % LCD_HEIGHT;
            if (cursor_y == 0) {
                LCD_Scroll();
            }
            LCD_UpdateBuffer();
        }
    }
}
