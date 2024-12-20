#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define BUZZ PB5
#define BUZZ_DDR DDRB
#define BUZZ_PORT PORTB
#define NOTE_DURATION 80 // osemki
#define MUSIC_LENGTH 49
#define C 486
#define Cis 473
#define D 460
#define Dis 450
#define E 440
#define F 428 //eis
#define Fis 419
#define G 410
#define Gis 401
#define A 392
#define Ais 385
#define H 379
#define C_high 373 //his

//# podwyższenie o pół, Bmol w dół o pół
void delay_us(uint32_t delay){
 for(uint32_t i=0;i<delay;i++){
   _delay_us(1);
 }
}

void play_tone(uint32_t step,uint32_t  delay) {
   for (uint32_t i = 0; i < (uint32_t)1000 * (delay) / (step)/2 ; i++) { 
     BUZZ_PORT |= _BV(BUZZ); 
     delay_us(step); 
     BUZZ_PORT &= ~_BV(BUZZ); 
     delay_us(step); 
   }
   };

#define MUSIC_LENGTH2 117 // 49 + 16

static const uint32_t notes[MUSIC_LENGTH] PROGMEM = {
   F, C, D, C, D, C, Dis, Dis, C, Dis, C, Dis, C,
   G, C, G, C, G, C, G, G, C, G, C, G, C,
   F, C, F, C, F, C, F, F, C, F, C, F, C,
   Dis, C, Dis, C, Dis, D, Dis, D, C, Ais, G
};
static const uint32_t notes2[MUSIC_LENGTH2] PROGMEM = {
C,D,Dis,C,D,Dis,F,D,A,G,Gis,F,G,
Dis,F,C,C,D,Dis,C,D,Dis,F,D,A,G,
Gis,F,G,Dis,F,C,C,D,Dis,C,D,Dis,F,
D,A,G,Gis,F,G,Dis,F,C,C,D,Dis,C,
D,Dis,F,D,A,G,Gis,F,G,Dis,G,C,C,
D,Dis,C,D,Dis,F,D,A,G,Gis,F,G,Dis,
F,C,C,D,Dis,C,D,Dis,F,D,A,G,Gis,
F,G,Dis,F,C,C,D,Dis,C,D,Dis,F,D,
A,G,Gis,F,G,Dis,F,C,C,D,Dis,C,D
  };

static const uint32_t delays2[MUSIC_LENGTH2] PROGMEM = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1
};

static const uint32_t delays[MUSIC_LENGTH] PROGMEM = {
   2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
   2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
   2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
   2, 1, 2, 1, 2, 1, 1, 1, 2, 2, 2
};

int main() {
 BUZZ_DDR |= _BV(BUZZ);
 while (1) {
   for(uint8_t i=0;i<MUSIC_LENGTH;i++){
     play_tone(pgm_read_byte(&notes[i]), NOTE_DURATION*pgm_read_byte(&delays[i]));
   }
 }
}
