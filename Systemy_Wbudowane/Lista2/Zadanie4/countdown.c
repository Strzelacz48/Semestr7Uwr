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
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(c) & ~_BV(d) & ~_BV(e) & ~_BV(f) & ~_BV(dp2);
      break;
    case 1:
      //b,c
      LED_PORT = 0xFF & ~_BV(b) & ~_BV(c) & ~_BV(dp2);
      break;
    case 2:
      //a,b,g,e,d
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(g) & ~_BV(d) & ~_BV(e) & ~_BV(dp2);
      break;
    case 3:
      //a,b,g,c,d
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(g) & ~_BV(c) & ~_BV(d) & ~_BV(dp2);
      break;
    case 4:
      //f,g,b,c
      LED_PORT = 0xFF & ~_BV(f) & ~_BV(g) & ~_BV(c) & ~_BV(b) & ~_BV(dp2);
      break;
    case 5:
      //a,f,g,c,d
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(f) & ~_BV(c) & ~_BV(d) & ~_BV(g) & ~_BV(dp2);
      break;
    case 6:
      //a,f,g,c,d,e
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(g) & ~_BV(c) & ~_BV(d) & ~_BV(e) & ~_BV(f) & ~_BV(dp2);
      break;
    case 7:
      //a,b,c
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(c) & ~_BV(dp2);
      break;
    case 8:
      //a,b,g,f,c,d,e
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(c) & ~_BV(d) & ~_BV(e) & ~_BV(f) & ~_BV(g) & ~_BV(dp2);
      break;
    case 9:
      //a,b,g,f,c,d
      LED_PORT = 0xFF & ~_BV(a) & ~_BV(b) & ~_BV(c) & ~_BV(d) & ~_BV(f) & ~_BV(g) & ~_BV(dp2);
      break;
      
  }
  return;
}
//_BV(x) to shift 1 o x
int main() {
  //
    LED_DDR = 0xFF;
    DDRC|=_BV(PC0)|_BV(PC1);
  while (1) {

    for(int i = 0; i <= 60; i++){ 
        for(int j = 0; j < 1000; j++){//Migamy na zmianę lewą i prawą cyfrą  
            if(j%2){//zapal cyfrę jedności
                PORTC|=_BV(PC0);
                PORTC&=~_BV(PC1);
                display(i%10);
            }
            else{//zapal cyfrę dziesiątek
                PORTC|=_BV(PC1);
                PORTC&=~_BV(PC0);
                display(i/10);
            }

            //display(i);
            _delay_ms(1);
        }
    }    
    //_delay_ms(1000);
  }
}