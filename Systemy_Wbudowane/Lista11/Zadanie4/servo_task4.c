#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

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
    TCCR1B = _BV(CS12) | _BV(WGM12) | _BV(WGM13);
    OCR1A = 63;
    ICR1 = 1249; // dokumentacja wypełnienie ma być od 63 - 125
}

int main()
{
    ADC_init();
    timer_init();
    //DDRD |= _BV(A1);
    //DDRD |= _BV(A2);
    DDRB |= _BV(PB1);
    while(1) {
        int32_t adcVal = ADC_read(0); // od (0 1023 )* 62/1023
        int32_t servoVal = 63 + adcVal * 62 / 1023;
        if (servoVal >= 125) servoVal = 125;
        if (servoVal <= 63) servoVal = 63;
        OCR1A = servoVal;
        _delay_ms(100);
    }
}

