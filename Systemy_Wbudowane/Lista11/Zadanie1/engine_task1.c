#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

//324279 Marcin Czapulak

void ADC_init() {
    // Konfiguracja ADC
    ADMUX = (1 << REFS0) | 1; // Napięcie odniesienia na AVcc, ADC0
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Włącz ADC, preskaler 64
}

uint16_t ADC_read(uint8_t channel) {
    // Ustaw kanał
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    // Rozpocznij konwersję
    ADCSRA |= (1 << ADSC);
    // Czekaj na zakończenie
    while (ADCSRA & (1 << ADSC));
    
    return ADC;
}

void PWM_init() {
    // Konfiguracja timera 1 w trybie Fast PWM, 8-bit
    TCCR1A = (1 << WGM10) | (1 << COM1A1); // Fast PWM, wyjście OC1A
    TCCR1B = (1 << WGM12) | (1 << CS11);   // Preskaler 8, Fast PWM
    DDRB |= (1 << PB1); // Ustaw PB1 (OC1A) jako wyjście
}

int main(void) {
    ADC_init();
    PWM_init();

    while (1) {
        // Odczytaj wartość ADC z kanału 0
        uint16_t adc_value = ADC_read(0);
        // Mapuj wartość ADC (0-1023) na wartość wypełnienia PWM (0-255)
        uint8_t pwm_value = adc_value / 4;
        // Ustaw wartość PWM
        OCR1A = pwm_value;
        // Krótka przerwa dla stabilności
        _delay_ms(10);
    }
}

