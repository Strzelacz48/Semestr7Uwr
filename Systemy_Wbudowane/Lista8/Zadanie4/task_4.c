#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>

//Marcin Czapulak

#define mainREAD_TASK_PRIORITY 1

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

static void vReadPoten(void* pvParameters);
static void vReadTemp(void* pvParameters);
static void vReadLight(void* pvParameters);

int uart_transmit(char c, FILE *stream);
int uart_receive(FILE *stream);
void uart_init(void);

FILE uart_file = FDEV_SETUP_STREAM(uart_transmit, uart_receive, _FDEV_SETUP_RW);

// inicjalizacja UART
void uart_init(void)
{
    UBRR0 = UBRR_VALUE;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// transmisja jednego znaku
int uart_transmit(char data, FILE *stream)
{
  // czekaj aż transmiter gotowy
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
  return 0;
}

// odczyt jednego znaku
int uart_receive(FILE *stream)
{
  // czekaj aż znak dostępny
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}
//SemaphoreHandle_t xSemaphore;
//ADC

volatile uint16_t resistance = 0;
//volatile uint8_t start_conversion = 0;
volatile int done = 0;
volatile int Semaphore = 0;
volatile int SemaphorePrint = 0;

// Initialize ADC
void adc_init()
{
  ADMUX = _BV(REFS0); // Reference AVcc, input ADC0 (PC0)
  DIDR0 = _BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D); // Disable digital input on ADC0
  // Set ADC clock prescaler to 128 for 125kHz ADC clock (16MHz / 128)
  ADCSRA = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);
  ADCSRA |= _BV(ADEN); // Enable ADC
  ADCSRA |= _BV(ADIE); // Enable ADC interrupt
}

//ADCSRA |= _BV(ADSC);


// ADC conversion complete interrupt service routine
ISR(ADC_vect)
{
    resistance = ADC;
    done = 1;
}




int main(void)
{
    uart_init();
    stdin = stdout = stderr = &uart_file;
    adc_init();
    //xSemaphore = xSemaphoreCreateMutex();

    // Create task.
    xTaskHandle blink_handle;
    xTaskHandle read_handle;

    xTaskCreate
        (
            vReadPoten,
            "readerPoten",
            configMINIMAL_STACK_SIZE * 2,
            NULL,
            mainREAD_TASK_PRIORITY,
            &read_handle
        );

    xTaskCreate
        (
            vReadTemp,
            "readerTemp",
            configMINIMAL_STACK_SIZE * 2,
            NULL,
            mainREAD_TASK_PRIORITY,
            &read_handle
        );
    xTaskCreate
        (
            vReadLight,
            "readerLight",
            configMINIMAL_STACK_SIZE * 2,
            NULL,
            mainREAD_TASK_PRIORITY,
            &read_handle
        );

    // Start scheduler.
    vTaskStartScheduler();

    return 0;
}


void vApplicationIdleHook(void)
{

}

static uint16_t readADC(uint8_t mux){
    //while(xSemaphoreTake( xSemaphore, ( TickType_t ) 6) != pdTRUE);
    while(Semaphore);
    Semaphore = 1;
    ADMUX |= mux;
    ADCSRA |= _BV(ADSC);
    //printf("A");
    while (!done);
    uint16_t result = resistance;
    //printf("a");
    done = 0;
    ADMUX ^= mux;
    //xSemaphoreGive( xSemaphore );
    Semaphore = 0;
    return result;
}

static void vReadPoten(void* pvParameters){
    uint8_t mux = 0;
    while(1)
    {
        uint16_t result = readADC(mux);
        while(SemaphorePrint);
        SemaphorePrint = 1;
        printf("odczytano potencjometr : %d\r\n\n", result);
        SemaphorePrint = 0;
        vTaskDelay(4 * 210);
    }
}

static void vReadTemp(void* pvParameters){
    uint8_t mux = 1;
    while(1)
    {
        uint16_t result = readADC(mux);
        while(SemaphorePrint);
        SemaphorePrint = 1;
        printf("odczytano termometr : %d\r\n\n", result);
        SemaphorePrint = 0;
        vTaskDelay(3 * 370);
    }
}

static void vReadLight(void* pvParameters){
    uint8_t mux = 2;
    while(1)
    {
        uint16_t result = readADC(mux);
        while(SemaphorePrint);
        SemaphorePrint = 1;
        printf("odczytano światełko : %d\r\n\n", result);
        SemaphorePrint = 0;
        vTaskDelay(2 * 420);
    }
}
