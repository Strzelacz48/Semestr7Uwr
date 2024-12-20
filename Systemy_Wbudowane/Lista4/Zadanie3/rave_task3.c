#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define RED_LED PB2
#define GREEN_LED PB1
#define BLUE_LED PB3
#define LED_DDR DDRB
#define LED_PORT PORTB
#define MAX_LIGHTNESS 100
#define SATURATION 255

uint8_t lightness_length = 30;
uint8_t lightness[] = {
    1, 2, 3, 4, 5, 7, 9, 11, 14, 
    18, 23, 29, 37, 47, 59, 74, 
    93, 116, 145, 160, 176, 189, 
    203, 215, 226, 235, 242, 247, 
    251, 255
};

void timer1_init() {
    // Timer1 w trybie Fast PWM dla PB2 (OC1B - czerwony) i PB1 (OC1A - zielony)
    TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);      // non-inverting, Fast PWM
    TCCR1B = _BV(WGM12) | _BV(WGM13) | _BV(CS10);         // prescaler = 1
    ICR1 = 255;                                           // Top = 255 dla 8-bit PWM
    DDRB |= _BV(RED_LED) | _BV(GREEN_LED);                // Ustaw PB1 i PB2 jako wyjścia
}

void timer2_init() {
    // Timer2 w trybie Fast PWM dla PB3 (OC2A - niebieski)
    TCCR2A = _BV(COM2A1) | _BV(WGM20) | _BV(WGM21);       // non-inverting, Fast PWM
    TCCR2B = _BV(CS20);                                   // prescaler = 1
    DDRB |= _BV(BLUE_LED);                                // Ustaw PB3 jako wyjście
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    OCR1A = g;    // Zielony kanał
    OCR1B = r;    // Czerwony kanał
    OCR2A = b;    // Niebieski kanał
}

void hsv_to_rgb(uint16_t hue, uint8_t sat, uint8_t val, uint8_t* r, uint8_t* g, uint8_t* b) {
    uint8_t region = hue / 60;
    uint16_t remainder = (hue - (region * 60)) * 255 / 60;

    uint8_t p = (val * (255 - sat)) / 255;
    uint8_t q = (val * (255 - (sat * remainder) / 255)) / 255;
    uint8_t t = (val * (255 - (sat * (255 - remainder)) / 255)) / 255;

    switch (region) {
        case 0:
            *r = val; *g = t; *b = p;
            break;
        case 1:
            *r = q; *g = val; *b = p;
            break;
        case 2:
            *r = p; *g = val; *b = t;
            break;
        case 3:
            *r = p; *g = q; *b = val;
            break;
        case 4:
            *r = t; *g = p; *b = val;
            break;
        default:
            *r = val; *g = p; *b = q;
            break;
    }
}

void set_color() {
    uint16_t hue = 0;
    uint8_t r, g, b;
    hue = rand() % 360; // Losowy hue od 0 do 359

    for (uint8_t i = 0; i < lightness_length; i++) {
        hsv_to_rgb(hue, lightness[i], SATURATION, &r, &g, &b);  // Saturation 100%, Value 70%
        set_rgb(r, g, b);                       // Ustaw diodę RGB na obliczony kolor
        _delay_ms(35);
    }
    for (uint8_t i = lightness_length; i > 0; i--) {
        hsv_to_rgb(hue, lightness[i - 1], SATURATION, &r, &g, &b);  // Saturation 100%, Value 70%
        set_rgb(r, g, b);                       // Ustaw diodę RGB na obliczony kolor
        _delay_ms(35);
    }
}

int main() {
    LED_DDR |= _BV(RED_LED) | _BV(GREEN_LED) | _BV(BLUE_LED);
    timer1_init();
    timer2_init();

    while (1) {
        set_color();
    }
} 