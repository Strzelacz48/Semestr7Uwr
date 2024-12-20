#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)


#define LED_DDR DDRD
#define LED_PORT PORTD

//_BV(x) to shift 1 o x
int main() {
  //
    LED_DDR = 0xFF;

  while (1) {
    for(int i = 0; i < 8; i++){
        LED_PORT = _BV(i);
        _delay_ms(400);
    }
    for(int i = 7; i >= 0; i--){
        LED_PORT = _BV(i);
        _delay_ms(400);
    }
  }
}