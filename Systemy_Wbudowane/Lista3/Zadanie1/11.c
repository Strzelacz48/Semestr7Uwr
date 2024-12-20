#include <stdio.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define NTLEN    670
#define BASESTEP 180000
#define PAUSE    1000

#define BUZZ PB5
#define BUZZ_DDR DDRB
#define BUZZ_PORT PORTB

#define TONEh  BASESTEP/247
#define TONEc1  BASESTEP/262
#define TONEcis1 BASESTEP/277
#define TONEd1  BASESTEP/294
#define TONEdis1 BASESTEP/311
#define TONEe1  BASESTEP/330
#define TONEf1  BASESTEP/349
#define TONEfis1 BASESTEP/370
#define TONEg1  BASESTEP/392
#define TONEgis1 BASESTEP/415
#define TONEa1  BASESTEP/440
#define TONEais1 BASESTEP/466
#define TONEh1  BASESTEP/494
#define TONEc2  BASESTEP/523
#define TONEcis2 BASESTEP/554
#define TONEd2  BASESTEP/587
#define TONEdis2 BASESTEP/622
#define TONEe2  BASESTEP/659
#define TONEf2  BASESTEP/699
#define TONEfis2 BASESTEP/740
#define TONEg2  BASESTEP/784
#define TONEgis2 BASESTEP/831
#define TONEa2  BASESTEP/880
#define TONEais2 BASESTEP/932
#define TONEh2  BASESTEP/988
#define TONEc3  BASESTEP/1047
#define TONEcis3 BASESTEP/1109
#define TONEd3  BASESTEP/1175
#define TONEdis3 BASESTEP/1245
#define TONEe3  BASESTEP/1319
#define TONEf3  BASESTEP/1397
#define TONEfis3 BASESTEP/1480

#define NTC1 1
#define NTD1 3
#define NTE1 5
#define NTF1 6
#define NTG1 8
#define NTA1 10
#define NTH1 12
#define NTC2 13
#define NTD2 15
#define NTE2 17
#define NTF2 18
#define NTG2 20
#define NTA2 22
#define NTH2 24

const uint32_t stepl[32] = {
  TONEh, TONEc1, TONEcis1, TONEd1,
  TONEdis1, TONEe1, TONEf1, TONEfis1,
  TONEg1, TONEgis1, TONEa1, TONEais1,
  TONEh1, TONEc2, TONEcis2, TONEd2,
  TONEdis2, TONEe2, TONEf2, TONEfis2,
  TONEg2, TONEgis2, TONEa2, TONEais2,
  TONEh2, TONEc3, TONEcis3, TONEd3,
  TONEdis3, TONEe3, TONEf3, TONEfis3 };
  

#define custom_dely(i) \
  for (uint32_t j = 0; j < stepl[i] / 10; j++) \
    _delay_us(10);

#define TONE(step, delay) \
    for (uint16_t k = 0; k < (uint32_t)1000 * (delay) / (stepl[step]) / 2; k++) { \
      PORTB |= _BV(BUZZ); \
      custom_dely(step); \
      PORTB &= ~_BV(BUZZ); \
      custom_dely(step); \
    }

int main() {

  BUZZ_DDR |= _BV(BUZZ);
  BUZZ_PORT &= ~_BV(BUZZ);
  
  static const uint8_t msc[93] PROGMEM = {
    3+8*NTC2,		3+8*NTE2, 3+8*NTE2, 3+8*NTE2, 4+8*NTD2, 4+8*NTC2, 2+8*NTG2, 3+8*NTG2, 4+8*NTG2, 4+8*NTF2,
    3+8*NTE2, 3+8*NTE2, 3+8*NTE2, 4+8*NTD2, 4+8*NTC2, 2+8*NTG2, 3+8*NTG2, 4+8*NTG2, 4+8*NTF2,	3+8*NTE2, 4+8*NTF2, 4+8*NTG2, 3+8*NTF2, 3+8*NTE2, 3+8*NTD2, 3+8*NTH2, 3+8*NTG2, 3+8*NTC2,
    3+8*NTE2, 3+8*NTE2, 3+8*NTE2, 4+8*NTD2, 4+8*NTC2, 2+8*NTG2, 3+8*NTG2, 4+8*NTG2, 4+8*NTF2,	3+8*NTE2, 3+8*NTE2, 3+8*NTE2, 4+8*NTD2, 4+8*NTC2, 2+8*NTG2, 3+8*NTG2, 4+8*NTG2, 4+8*NTF2,
    3+8*NTE2, 4+8*NTF2, 4+8*NTG2, 3+8*NTF2, 3+8*NTE2, 2+8*NTD2, 3+8*NTD2, 3+8*NTE2,	3+8*NTG2, 4+8*NTF2, 4+8*NTE2, 3+8*NTF2, 3+8*NTG2, 3+8*NTA2, 2+8*NTG2, 3+8*NTE2,
    3+8*NTG2, 4+8*NTF2, 4+8*NTE2, 3+8*NTF2, 3+8*NTG2, 3+8*NTA2, 2+8*NTG2, 3+8*NTE2,	3+8*NTA2, 2+8*NTG2, 3+8*NTF2, 3+8*NTE2, 4+8*NTD2, 4+8*NTC2, 2+8*NTD2,
    2+8*NTC2, 3+8*NTC2, 3+8*NTE2, 3+8*NTG2, 4+8*NTF2, 4+8*NTE2, 3+8*NTF2, 3+8*NTG2,	3+8*NTA2, 2+8*NTG2, 3+8*NTE2, 3+8*NTG2, 4+8*NTF2, 4+8*NTE2, 3+8*NTF2, 3+8*NTG2
  };
  
  uint32_t a,b;
  while(1) {
    for (uint16_t i = 0; i < sizeof(msc); i++)
      if((pgm_read_byte(&msc[i])>>3)==31)
        _delay_ms(PAUSE);
      else{
        a = (uint32_t)NTLEN;
        b = (uint32_t)(((uint32_t)1)<<((uint32_t)(pgm_read_byte(&msc[i])%8)));
        a /= b;
        TONE(pgm_read_byte(&msc[i])>>3,a);
        }
    _delay_ms(5000);
  }
}

