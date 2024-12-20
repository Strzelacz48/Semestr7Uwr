// Marcin Czapulak
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PB4
#define BTN_PIN PINB
#define BTN_PORT PORTB

volatile int memory[61];
volatile int i = 0;
volatile int j = 0;

void remember(){
    memory[i] = BTN_PIN;
    i++;
    
    if (i == 61 || j > i){
        if (_BV(BTN) & memory[j]){
            LED_PORT &= ~_BV(LED);
            j++;
        }
        else{
            LED_PORT |= _BV(LED);
            j++;
        }
        if(j >= 61)
            j = 0;
    }
    if(i >= 61)
        i = 0;
    
    return;
}

// inicjalizacja licznika 2
void timer2_init()
{
  // preskaler 1024
  TCCR2B = _BV(CS20) | _BV(CS21) | _BV(CS22);
  // odmaskowanie przerwania przepełnienia licznika
  TIMSK2 |= _BV(TOIE2);
}

// procedura obsługi przerwania przepełnienia licznika
ISR(TIMER2_OVF_vect) {
    remember();
}

int main() {
  BTN_PORT |= _BV(BTN);
  LED_DDR |= _BV(LED);
  // zainicjalizuj licznik 2
  timer2_init();
  // ustaw tryb uśpienia na tryb bezczynności
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  // odmaskuj przerwania
  sei();
  // program testowy
  while (1) {
    sleep_mode();
}
}