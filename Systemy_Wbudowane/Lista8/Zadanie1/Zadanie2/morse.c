#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

#define LED PD2
#define LED_DDR DDRD
#define LED_PORT PORTD

const char* morseCode(char letter) {
    static const char* morse[] = {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-",
        ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
    };

    if (letter >= 'a' && letter <= 'z') 
        return morse[letter - 'a'];
        
    if (letter >= 'A' && letter <= 'Z') 
        return morse[letter - 'A'];
    return "";
    
}

void uart_init() {
  UBRR0 = UBRR_VALUE;
  UCSR0A = 0;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart_transmit(char data, FILE *stream) {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
  return 0;
}

int uart_receive(FILE *stream) {
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

FILE uart_file;

void blink_short() {
    LED_PORT |= _BV(LED);
    _delay_ms(200);
    LED_PORT &= ~_BV(LED);
    _delay_ms(200);
}

void blink_long() {
    LED_PORT |= _BV(LED);
    _delay_ms(600);
    LED_PORT &= ~_BV(LED);
    _delay_ms(200);
}

void blink_morse(const char* morse) {
    while (*morse) {
        if (*morse == '.') {
            blink_short();
        } else if (*morse == '-') {
            blink_long();
        }
        _delay_ms(400);
        morse++;
    }
}

int main() {
  uart_init();
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;

  LED_DDR |= _BV(LED);

  while (1) {
    char input;
    scanf("%c", &input);
    const char* morse = morseCode(input);
    blink_morse(morse);
  }
}