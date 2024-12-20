//Marcin Czapulak
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <math.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem
#define NUM_SAMPLES 50                     // liczba próbek do policzenia wariancji

void uart_init()
{
    UBRR0 = UBRR_VALUE;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart_transmit(char data, FILE *stream)
{
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = data;
    return 0;
}

int uart_receive(FILE *stream)
{
    while (!(UCSR0A & _BV(RXC0)));
    return UDR0;
}

// oczekiwanie na zakończenie transmisji
void uart_wait()
{
  while(!(UCSR0A & _BV(TXC0)));
}

void adc_init()
{
    ADMUX = _BV(REFS0) | 0b1110;  // Referencja AVcc, pomiar wewnętrznego napięcia 1.1V
    ADCSRA = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2) | _BV(ADEN); // Preskaler 128, włączenie ADC
    ADCSRA |= _BV(ADIE);
}

volatile uint16_t adc_result;
volatile uint8_t measurement_ready = 0;

ISR(ADC_vect)
{
    adc_result = ADC;
    // measurement_ready = 1;  // Ustawienie flagi po zakończeniu konwersji
}

uint16_t read_adc_waiting()
{
    ADCSRA |= _BV(ADSC);            // Start konwersji
    while (ADCSRA & _BV(ADSC));      // Aktywne czekanie na zakończenie konwersji
    return ADC;
}

uint16_t read_adc_noise_reduction()
{
    // measurement_ready = 0;
    // ADCSRA |= _BV(ADSC);             // Start konwersji
    sleep_mode();                    // Przejście w tryb uśpienia (Noise Reduction)
    // while (!measurement_ready);      // Oczekiwanie na zakończenie pomiaru (ustawienie flagi w ISR)
    return adc_result;
}

double calculate_variance(uint16_t samples[], uint8_t num_samples)
{
    double mean = 0;
    for (uint8_t i = 0; i < num_samples; i++) {
        mean += samples[i];
    }
    mean /= num_samples;

    double variance = 0;
    for (uint8_t i = 0; i < num_samples; i++) {
        variance += (samples[i] - mean) * (samples[i] - mean);
    }
    return variance / (num_samples - 1);
}

FILE uart_file;

int main(void)
{
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;
    adc_init();
    printf("BEGIN!\r\n"); // Magiczny printf aby adc pierwszy wynik byl wiarygodny
    set_sleep_mode(SLEEP_MODE_ADC);  // Ustawienie trybu uśpienia ADC Noise Reduction

    uint16_t samples_waiting[NUM_SAMPLES];
    uint16_t samples_noise_reduction[NUM_SAMPLES];


    // Pomiar z aktywnym oczekiwaniem
    for (uint8_t i = 0; i < NUM_SAMPLES; i++) {
        samples_waiting[i] = read_adc_waiting();
        _delay_ms(50);
    }
    double variance_waiting = calculate_variance(samples_waiting, NUM_SAMPLES);
    printf("Wariancja (czekanie): %.2f\r\n", variance_waiting);
    uart_wait();
    _delay_ms(400);

    sei();  // Włączenie globalnych przerwań
    // Pomiar z redukcją szumów (Noise Reduction)
    for (uint8_t i = 0; i < NUM_SAMPLES; i++) {
        //printf("In a loop\r\n");
        samples_noise_reduction[i] = read_adc_noise_reduction();
        _delay_ms(50);
    }
    double variance_noise_reduction = calculate_variance(samples_noise_reduction, NUM_SAMPLES);
    printf("Wariancja (redukcja szumów): %.2f\r\n", variance_noise_reduction);

}
