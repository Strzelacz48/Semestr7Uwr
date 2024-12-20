#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/atomic.h>

#define BAUD 9600                          // Baud rate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // Zgodnie ze wzorem
#define BUFFER_SIZE 64
//Marcin Czapulak

// Bufor cykliczny
typedef struct {
    char data[BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} circular_buffer_t;

volatile circular_buffer_t tx_buffer = {{0}, 0, 0};
volatile circular_buffer_t rx_buffer = {{0}, 0, 0};

// Funkcja pomocnicza do sprawdzania, czy bufor jest pełny
static inline uint8_t buffer_is_full(volatile circular_buffer_t *buffer)
{
    return ((buffer->head + 1) % BUFFER_SIZE) == buffer->tail;
}

// Funkcja pomocnicza do sprawdzania, czy bufor jest pusty
static inline uint8_t buffer_is_empty(volatile circular_buffer_t *buffer)
{
    return buffer->head == buffer->tail;
}

// Funkcja pomocnicza do dodawania elementu do bufora
static inline void buffer_push(volatile circular_buffer_t *buffer, char c)
{
    buffer->data[buffer->head] = c;
    buffer->head = (buffer->head + 1) % BUFFER_SIZE;
}

// Funkcja pomocnicza do odczytu elementu z bufora
static inline char buffer_pop(volatile circular_buffer_t *buffer)
{
    char c = buffer->data[buffer->tail];
    buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
    return c;
}

// Inicjalizacja UART
void uart_init()
{
    // ustaw baudrate
    UBRR0 = UBRR_VALUE;
    // włącz odbiornik, nadajnik i przerwania RX oraz UDRE
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0) | _BV(UDRIE0);
    // ustaw format 8n1
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// Przerwanie USART RX (odbiór danych)
ISR(USART_RX_vect)
{
    char c = UDR0; // Odczytaj odebrany znak
    if (!buffer_is_full(&rx_buffer)) {
        buffer_push(&rx_buffer, c); // Dodaj znak do bufora odbiorczego
    }
}

// Przerwanie USART UDRE (gotowość do nadawania)
ISR(USART_UDRE_vect)
{
    if (!buffer_is_empty(&tx_buffer)) {
        UDR0 = buffer_pop(&tx_buffer); // Wyślij znak z bufora nadawczego
    } else {
        UCSR0B &= ~_BV(UDRIE0); // Wyłącz przerwanie UDRE, jeśli bufor pusty
    }
}

// Transmisja jednego znaku
int uart_transmit(char data, FILE *stream)
{
    // Czekaj, aż będzie miejsce w buforze nadawczym
    while (buffer_is_full(&tx_buffer));

    // Sekcja krytyczna: dodaj znak do bufora nadawczego
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        buffer_push(&tx_buffer, data);
    }

    // Włącz przerwanie UDRE
    UCSR0B |= _BV(UDRIE0);

    return 0;
}

// Odbiór jednego znaku
int uart_receive(FILE *stream)
{
    // Czekaj, aż coś pojawi się w buforze odbiorczym
    while (buffer_is_empty(&rx_buffer));

    // Sekcja krytyczna: odczytaj znak z bufora odbiorczego
    char c;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        c = buffer_pop(&rx_buffer);
    }

    return c;
}

FILE uart_file;

int main(void)
{
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    sei(); // Włącz globalne przerwania


    while (1) {
        char c = getchar(); // Odczytaj znak
        putchar(c);         // Odeslij znak (echo)
    }
}
