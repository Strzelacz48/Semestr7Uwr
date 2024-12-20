#include <avr/io.h>
#include <util/delay.h>

#define LED_DDR DDRD
#define LED_PORT PORTD

char num[] = {_BV(PD5)|_BV(PD4)|_BV(PD3)|_BV(PD2)|_BV(PD1)|_BV(PD0),
_BV(PD2)|_BV(PD0),
_BV(PD6)|_BV(PD4)|_BV(PD3)|_BV(PD1)|_BV(PD0),
_BV(PD6)|_BV(PD3)|_BV(PD2)|_BV(PD1)|_BV(PD0),
_BV(PD6)|_BV(PD5)|_BV(PD2)|_BV(PD0),
_BV(PD6)|_BV(PD5)|_BV(PD3)|_BV(PD2)|_BV(PD1),
_BV(PD6)|_BV(PD5)|_BV(PD4)|_BV(PD3)|_BV(PD2)|_BV(PD1),
_BV(PD2)|_BV(PD1)|_BV(PD0),
_BV(PD6)|_BV(PD5)|_BV(PD4)|_BV(PD3)|_BV(PD2)|_BV(PD1)|_BV(PD0),
_BV(PD6)|_BV(PD5)|_BV(PD3)|_BV(PD2)|_BV(PD1)|_BV(PD0)};

int main() {
  UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
  LED_DDR |= 0xff;
  DDRC = 0xff;
  PORTC = 0xff;
  uint8_t n1=0, n2=0;

  while (1) {
    // LED_PORT &= ~_BV(PD0);
    for(int i=0; i<500; i++)
    {
      LED_PORT = ~num[n1];
      PORTC = 1;
      _delay_ms(1);
      LED_PORT = ~num[n2]; 
      PORTC = 2;
      _delay_ms(1);
    }
    n1++;
    if(n1 == 10)
    {
      n1=0;
      n2++;
    }
    if(n2 == 6)
      n2=0;
  }
}