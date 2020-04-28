#include <MsTimer2.h>

//   din 5 (connector)
//
//       2
//   5       4
//  3         1
//
// 3 > clock
// 5 > +5v
// 2 > data
// 4 > gnd
// 1> none
//
//arduino pin 2 > keyboard data  > pin 2
//arduino pin 3 > keyboard clock > pin 3


const int datalen = 18;
//structure is xxxxxxxxy_xxxxxxxxy
//xxxxxxxx 8 bit code of bytton
//y is random byte
//_ is delay
//* second copy of bit code may be corrupted

volatile unsigned long synch_data[datalen] = {};
volatile int synch_counter = 0;


static const byte *hex_letters[] = 
{
  0xEE,0x9E,0xDE,0xBE,0xFE,0x81,0xC1,0xA1,0xE1,0x91,
  0xCA,0xA0,0xE0,0x90,0xD0,0xB0,0xF0,0x88,0xC8,0xA8,0xE8,0x98,0xD8,0xB8,
  0xF8,0x84,0xC4,0xA4,0xE4,0x94,0xD4,0xB4,0xF4,0x8C,0xCC,0xAC,0xEC,0x9C,
  0xDC,0xBC,0xFC,0x82,0xC2,0xA2,0xE2,0x92,0xD2,0xB2,0xF2,0x8A,
  0xAA,0x9A,0xDA,0xBA,0xFA,0x86,0xC6,0xA6,0xE6,0x96,0xD6,0xB6,
  0x8E,0xCE,0xEA,0xAE,
  0xC0,0xB1,0xD1,
  0xA5,0xF9,0x85,0xC5,0xE9,0x99,0xD9,0xF1,0x89,0xC9,
  0xF6,0xA9,0xB9,0xE5
};

static const char *letters[] =
{
    "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10",
    "+","1","2","3","4","5","6","7","8","9","0","[","]","backspase",
    "tab","q","w","e","r","t","y","u","i","o","p","{","}","enter",
    "ctrl","a","s","d","f","g","h","j","k","l",";","'",
    "shift_l","z","x","c","v","b","n","m","<",">","/","shift_r",
    "alt","space",".","capslock",
    "esc","scrollock","numlock",
    "0","1","2","3","4","5","6","7","8","9",
    "PrtSc","-","+","del"
};

// byte to button name
String translate(byte val) {
    noInterrupts(); //interrupts off
    for (int i = 0; i < 83; i++) {
        if (val == hex_letters[i]) {
            interrupts(); //interrupts on
            return letters[i];
        }
    }
    interrupts(); //interrupts on
    return "none";
}

//if array contains not null elements
boolean isnone(unsigned long thedata[datalen]) {
    noInterrupts(); //interrupts off
    for (int i = 0; i < datalen; i++) {
        if (thedata[i] > 0) {
            interrupts(); //interrupts on
            return true;
        }
    }
    interrupts(); //interrupts on
    return false;
}

void by_timer() {
    noInterrupts(); //interrupts off
    if (isnone(synch_data)) {
    
        int index = 0;
        uint8_t acc = 0;
        for (int i = 0; i < 8; i++) {
            acc = acc << 1;
            if ((synch_data[index + i]) == 1) {
                acc |= 1;
            }
        }

        Serial.print(acc, HEX); //printing the first 8 bytes
        Serial.print(" ");
        Serial.println(translate(acc));
    }
    memset(synch_data, 0, sizeof(synch_data)); //clear print data
    synch_counter = 0; //clear counter
    interrupts(); //interrupts on
}

void by_synch_interrupt() {
    noInterrupts(); //interrupts off
    if (synch_counter < datalen) {
        synch_data[synch_counter] = digitalRead(2);
        synch_counter = synch_counter + 1;
        interrupts(); //interrupts on
        MsTimer2::start(); //call by_timer() if nothing happens in 300ms
    }
}

void setup() {
    Serial.begin(115200);
    MsTimer2::set(300, by_timer);
    MsTimer2::start();
    pinMode(2, INPUT_PULLUP);

    attachInterrupt(1, by_synch_interrupt, RISING);
}

void loop() {}
