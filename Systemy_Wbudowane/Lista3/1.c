#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define BUZ PB5
#define BUZ_DDR DDRB
#define BUZ_PORT PORTB

#define  a3f    208     // 208 Hz
#define  b3f    233     // 233 Hz
#define  b3     247     // 247 Hz
#define  c4     261     // 261 Hz MIDDLE C
#define  c4s    277     // 277 Hz
#define  e4f    311     // 311 Hz
#define  f4     349     // 349 Hz
#define  a4f    415     // 415 Hz
#define  b4f    466     // 466 Hz
#define  b4     493     //  493 Hz
#define  c5     523     // 523 Hz
#define  c5s    554     // 554  Hz
#define  e5f    622     // 622 Hz
#define  f5     698     // 698 Hz
#define  f5s    740     // 740 Hz
#define  a5f    831     // 831 Hz

#define LENGTH 1000000.0/2 //assume frequency
#define NOTE_LENGTH 0.1//in seconds

#define N 284


static const uint8_t notes[N] PROGMEM = {
    204,237,109,205,206,48,47,46,45,204,237,109,145,72,232,104,204,237,109,205,206,48,47,46,45,204,237,109,145,72,232,104,81,37,37,37,37,70,49,36,33,160,49,33,33,36,101,32,72,40,166,49,33,33,36,37,33,37,70,49,36,33,33,96,49,33,33,36,69,32,32,38,38,38,39,134,165,38,39,37,38,38,38,39,70,64,81,33,36,37,96,49,38,39,102,41,41,40,40,110,110,205,41,41,40,40,109,109,108,43,73,44,44,44,44,108,109,107,41,72,72,72,141,236,44,41,41,40,40,110,110,205,41,41,40,40,112,107,108,43,73,44,44,44,44,108,109,107,41,72,81,72,141,236,44,145,81,37,37,37,37,70,49,36,33,160,49,33,33,36,101,32,72,40,166,49,33,33,36,37,33,37,70,49,36,33,33,96,49,33,33,36,69,32,32,38,38,38,39,134,165,38,39,37,38,38,38,39,70,64,81,33,36,37,96,49,38,39,102,41,41,40,40,110,110,205,41,41,40,40,109,109,108,43,73,44,44,44,44,108,109,107,41,72,72,72,141,236,44,41,41,40,40,110,110,205,41,41,40,40,112,107,108,43,73,44,44,44,44,108,109,107,41,72,81,72,141,236,44,145,241,241,241,241
};

void A3F()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * a3f; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / a3f);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / a3f);
    }
}

void B3F()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * b3f; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / b3f);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / b3f);
    }
}

void B3()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * b3; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / b3);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / b3);
    }
}

void C4()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * c4; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / c4);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / c4);
    }
}

void C4S()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * c4s; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / c4s);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / c4s);
    }
}

void E4F()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * e4f; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / e4f);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / e4f);
    }
}

void F4()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * f4; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / f4);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / f4);
    }
}

void A4F()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * a4f; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / a4f);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / a4f);
    }
}

void B4F()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * b4f; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / b4f);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / b4f);
    }
}

void B4()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * b4; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / b4);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / b4);
    }
}

void C5()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * c5; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / c5);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / c5);
    }
}

void C5S()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * c5s; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / c5s);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / c5s);
    }
}

void E5F()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * e5f; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / e5f);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / e5f);
    }
}

void F5()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * f5; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / f5);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / f5);
    }
}

void F5S()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * f5s; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / f5s);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / f5s);
    }
}

void A5F()
{
    for (uint16_t i = 0; i < NOTE_LENGTH * a5f; i++)
    {
        BUZ_PORT |= _BV(BUZ);
        _delay_us(LENGTH / a5f);
        BUZ_PORT &= ~_BV(BUZ);
        _delay_us(LENGTH / a5f);
    }
}



void space()
{
    _delay_ms(NOTE_LENGTH*1000);
}

void (*funcs[])() = {
    A3F,B3F,B3,C4,C4S,E4F,F4,A4F,B4F,B4,C5,C5S,E5F,F5,F5S,A5F,space
};

uint16_t index = 0;

void decode()
{
    uint8_t value = pgm_read_byte(&notes[index]);
    uint8_t note = value & 0b00011111;
    for (uint8_t i = 0; i < value >> 5; i++)
    {
        funcs[note]();
        _delay_us(10);
    }
}

int main() {
  BUZ_DDR |= _BV(BUZ);
  while (1) {
    decode();
    index++;
    if(index > N) index = 0;
  }
}
