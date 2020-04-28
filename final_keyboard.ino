#include <MsTimer2.h>
const int datalen = 18; //18

volatile unsigned long synch_data[datalen] = {};
volatile int synch_counter = 0;


static const byte *hex_letters[] = 
{0xEE,0x9E,0xDE,0xBE,0xFE,0x81,0xC1,0xA1,0xE1,0x91,
0xCA,0xA0,0xE0,0x90,0xD0,0xB0,0xF0,0x88,0xC8,0xA8,0xE8,0x98,0xD8,0xB8,
0xF8,0x84,0xC4,0xA4,0xE4,0x94,0xD4,0xB4,0xF4,0x8C,0xCC,0xAC,0xEC,0x9C,
0xDC,0xBC,0xFC,0x82,0xC2,0xA2,0xE2,0x92,0xD2,0xB2,0xF2,0x8A,
0xAA,0x9A,0xDA,0xBA,0xFA,0x86,0xC6,0xA6,0xE6,0x96,0xD6,0xB6,
0x8E,0xCE,0xEA,0xAE,
0xC0,0xB1,0xD1,
0xA5,0xF9,0x85,0xC5,0xE9,0x99,0xD9,0xF1,0x89,0xC9,
0xF6,0xA9,0xB9,0xE5};

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

String translate(byte val)
{
  noInterrupts(); //отключить прерывания
  for (int i=0; i<83; i++) {
   if (val == hex_letters[i]) 
     {  
      interrupts(); //включить прерывания
      return letters[i];
     }
   }
   interrupts(); //включить прерывания
   return "none";
 }

//есть ли что то кроме нулей?
boolean isnone(unsigned long thedata[datalen]) {
    noInterrupts(); //отключить прерывания
    for (int i = 0; i < datalen; i++) {
        if (thedata[i] > 0) {
            interrupts(); //включить прерывания
            return true;
        }
    }
    interrupts(); //включить прерывания
    return false;
}


void by_timer() {
    noInterrupts(); //отключить прерывания
    if (isnone(synch_data)) {

        int index = 0;
        uint8_t acc = 0;

//        while (index < datalen) {
            acc = 0;
            for (int i = 0; i < 8; i++) {
                acc = acc << 1;
                if ((synch_data[index + i]) == 1) {
                    acc |= 1;
                }
            }

            Serial.print(acc, HEX); //печать первых восьми байт это убирает девятый бит из обоих байтов и уменьшает ошибку
            Serial.print(" ");
            Serial.println(translate(acc));
            
         // 8 байт + стоповый  +  8 байт + стоповый  
    }
   
    memset(synch_data, 0, sizeof(synch_data)); //очистить вывод
    synch_counter = 0; //очистить счетчик
    interrupts(); //включить прерывания
}

void by_synch_interrupt() {
    noInterrupts(); //отключить прерывания
    if (synch_counter < datalen) {
        synch_data[synch_counter] = digitalRead(2);
        synch_counter = synch_counter + 1;
        interrupts(); //включить прерывания
        MsTimer2::start(); //если после последнего вызова прошло н секунд - вызовится by_timer()
    }
}

void setup() {
    Serial.begin(115200);
    MsTimer2::set(300, by_timer);
    MsTimer2::start();
    pinMode(2, INPUT_PULLUP);

    attachInterrupt(1, by_synch_interrupt, RISING); //CHANGE FALLING **RISING
}

void loop() {}
