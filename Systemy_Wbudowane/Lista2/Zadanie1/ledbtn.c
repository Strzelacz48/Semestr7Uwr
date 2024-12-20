#include <avr/io.h>
#include <util/delay.h>

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PB4
#define BTN_PIN PINB
#define BTN_PORT PORTB
int memory[100];
int i = 0;
int j = 0;

void remember(){
    _delay_ms(10);
    memory[i] = BTN_PIN;
    i++;
    
    if (i == 100 || j > i){
        if (_BV(BTN) & memory[j]){
            LED_PORT &= ~_BV(LED);
            j++;
        }
        else{
            LED_PORT |= _BV(LED);
            j++;
        }
        if(j >= 100)
            j = 0;
    }
    if(i >= 100)
        i = 0;
    
    return;
}
int main() {
  BTN_PORT |= _BV(BTN);
  LED_DDR |= _BV(LED);
  while (1) {
    remember();
  }
}