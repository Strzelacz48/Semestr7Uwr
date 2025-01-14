#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define A1 PD2
#define A2 PD3

//324279 Marcin Czapulak

void ADC_init() {
    // Konfiguracja ADC
    ADMUX = (1 << REFS0); // Napięcie odniesienia na AVcc, ADC0
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

void timer_init()
{
    TCCR1A = _BV(WGM11) | _BV(COM1A1);
    TCCR1B = _BV(CS11)| _BV(WGM12) | _BV(WGM13);
    OCR1A = 0;
    ICR1 = 3069;
}

int main()
{
    ADC_init();
    timer_init();
    DDRD |= _BV(A1);
    DDRD |= _BV(A2);
    DDRB |= _BV(PB1);
    while(1) {
        int16_t adcVal = ADC_read(0)-512;
        if (adcVal >= 0)
        {
            PORTD &= ~_BV(A2);
            PORTD |= _BV(A1);
        }
        else
        {
            adcVal = -adcVal;
            adcVal -= 1;
            PORTD &= ~_BV(A1);
            PORTD |= _BV(A2);
        }
        OCR1A = adcVal*6;
        _delay_ms(50);
    }
}

