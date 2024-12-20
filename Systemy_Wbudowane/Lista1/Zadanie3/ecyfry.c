#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

#define a PD0
#define b PD1
#define c PD2
#define d PD3
#define e PD4
#define f PD5
#define g PD6
#define dp2 PD7
#define LED_DDR DDRD
#define LED_PORT PORTD

void display(int number){
  switch(number){
    case 0:
      //a,b,c,d,e,f
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(c) & ~_BV(d) & ~_BV(e) & ~_BV(f);
      break;
    case 1:
      //b,c
      LED_PORT = 0xFF & ~_BV(b) & ~_BV(c);
      break;
    case 2:
      //a,b,g,e,d
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(g) & ~_BV(d) & ~_BV(e);
      break;
    case 3:
      //a,b,g,c,d
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(g) & ~_BV(c) & ~_BV(d);
      break;
    case 4:
      //f,g,b,c
      LED_PORT = 0xFF & ~_BV(f) & ~_BV(g) & ~_BV(c) & ~_BV(b);
      break;
    case 5:
      //a,f,g,c,d
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(f) & ~_BV(c) & ~_BV(d) & ~_BV(g);
      break;
    case 6:
      //a,f,g,c,d,e
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(g) & ~_BV(c) & ~_BV(d) & ~_BV(e) & ~_BV(f);
      break;
    case 7:
      //a,b,c
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(c);
      break;
    case 8:
      //a,b,g,f,c,d,e
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(c) & ~_BV(d) & ~_BV(e) & ~_BV(f) & ~_BV(g);
      break;
    case 9:
      //a,b,g,f,c,d
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(c) & ~_BV(d) & ~_BV(f) & ~_BV(g);
      break;
      
  }
  return;
}
//_BV(x) to shift 1 o x
int main() {
  //
    LED_DDR = 0xFF;
  while (1) {

    for(int i = 0; i <= 9; i++){  
      LED_PORT = 0xFF;
        display(i);
        _delay_ms(1000);
    }
  }
}