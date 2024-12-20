#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>

//Marcin Czapulak

#define mainLED_TASK_PRIORITY 1
#define mainREAD_TASK_PRIORITY 2

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

static void vBlinkLed(void* pvParameters);
static void vRead(void* pvParameters);

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

QueueHandle_t tx_buffer;
QueueHandle_t rx_buffer;

ISR(USART_UDRE_vect)
{
    uint8_t a;
    if (xQueueReceiveFromISR(tx_buffer, &a, 0) == pdPASS)
    {
        UDR0 = a;
    }
    else
    {
        UCSR0B &= ~_BV(UDRIE0); // wyłącz przerwanie dla pustej kolejki (aby nie spamować)
    }
}

ISR(USART_RX_vect) {
    char received = UDR0;
    if (uxQueueMessagesWaitingFromISR(rx_buffer)<29){
        xQueueSendToBackFromISR(rx_buffer, &received, NULL);
    }
}

int uart_transmit(char c, FILE *stream) {
    xQueueSendToBack(tx_buffer, &c, portMAX_DELAY);
    UCSR0B |= _BV(UDRIE0);
    return 0;
}

int uart_receive(FILE *stream)
{
    uint8_t data;
    if (xQueueReceive(rx_buffer, &data, portMAX_DELAY) == pdPASS)
    {
        return data;
    }
    return _FDEV_EOF;
}


int main(void)
{
    tx_buffer = xQueueCreate(30, sizeof(uint8_t));
    rx_buffer = xQueueCreate(30, sizeof(uint8_t));
    // Create task.
    xTaskHandle blink_handle;
    xTaskHandle read_handle;

    xTaskCreate
        (
            vBlinkLed,
            "blink",
            configMINIMAL_STACK_SIZE,
            NULL,
            mainLED_TASK_PRIORITY,
            &blink_handle
        );

    xTaskCreate
        (
            vRead,
            "reader",
            configMINIMAL_STACK_SIZE,
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

static void vBlinkLed(void* pvParameters)
{
    DDRB |= _BV(PB5);

    while(1)
    {
        PORTB ^= _BV(PB5);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}


static void vRead(void* pvParameters)
{
    uart_init();
    stdin = stdout = stderr = &uart_file;

    char a = 'a';
    while(1)
    {
        printf("Wprowadź znak \r\n");
        scanf("%c",&a);
        printf("Odczytano : %c\r\n\n", a);
        vTaskDelay(200);
    }
}
