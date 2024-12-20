#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include "i2c.c"

//Marcin Czapulak

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

// inicjalizacja UART
void uart_init()
{
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
  // wyczyść rejestr UCSR0A
  UCSR0A = 0;
  // włącz odbiornik i nadajnik
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  // ustaw format 8n1
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// transmisja jednego znaku
int uart_transmit(char data, FILE *stream)
{
  // czekaj aż transmiter gotowy
  while(!(UCSR0A & _BV(UDRE0)));
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

FILE uart_file;

const uint8_t eeprom_addr = 0xa0;// z dokumentacji

uint8_t read_addr(uint8_t addr)
{
  i2cStart();
  i2cSend(eeprom_addr | ((addr & 0x100) >> 7));
  i2cSend(addr & 0xff);
  i2cStart();
  i2cSend(eeprom_addr | 0x1 | ((addr & 0x100) >> 7));
  uint8_t data = i2cReadNoAck();
  i2cStop();
  return data;
}

void write_addr(uint8_t addr, uint8_t data)
{
  i2cStart();
  i2cSend(eeprom_addr | ((addr & 0x100) >> 7));
  i2cSend(addr & 0xff);
  i2cSend(data);
  i2cStop();
  _delay_ms(10);
}


int main()
{
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  // zainicjalizuj I2C
  i2cInit();
  // program testowy
  char command;
  while(1) {
    printf("ready\r\n");
    scanf("%c",&command);
    if (command == 'r')
    {
      printf("read\r\n");
      uint16_t addr;
      scanf("%d",&addr);
      printf("odczytano: %d\r\n",addr);
      printf("odczytano %.3x: %d\r\n",addr,read_addr(addr));
    }
    else if (command == 'w')
    {
      printf("write\r\n");
      uint16_t addr;
      uint8_t data;
      scanf("%d %d",&addr, &data);
      printf("odczytano: %d, %d\r\n",addr, data);
      write_addr(addr, data);
      printf("zapisano %.3x: %d\r\n",addr,read_addr(addr));
    }

    printf("\r\n");
  }
}

