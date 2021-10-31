// https://git.dejvino.cz/dejvino/xt-keyboard-adapter/src/branch/master/XtKeyboard.ino
// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html

int clockPin = 3;
int dataPin = 2;
volatile byte data = B0;
int test = 0;
volatile int counter = 0;
int numbits = 9;
unsigned long repeatTimeout = 0;
unsigned long repeatTimeoutValue = 100;
const int keyReleaseBit = 0x80;
char m[255];

void processByte(byte data) {
  bool isRelease = data & keyReleaseBit;
  int key = data - (isRelease ? keyReleaseBit : 0);

  Serial.print("Scan: <" + String(data) + "> " + "Key: <" + String(key) + "> " + "<" + String(m[key]) + "> ");

  if (isRelease) {
    Serial.print("Release: " + String(key));
  } else {
    Serial.print("Press: " + String(key));
  }
  Serial.println(" Button:   " + String(m[key]) );
}
void setupKeyMapping() {
  m[1]='-';
  m[2]='1';
  m[3]='2';
  m[4]='3';
  m[5]='4';
  m[6]='5';
  m[7]='6';
  m[8]='7';
  m[9]='8';
  m[10]='9';
  m[11]='0';
  m[12]='-';
  m[13]='=';
  m[14]='-';
  m[15]='-';
  m[16]='q';
  m[17]='w';
  m[18]='e';
  m[19]='r';
  m[20]='t';
  m[21]='y';
  m[22]='u';
  m[23]='i';
  m[24]='o';
  m[25]='p';
  m[26]='[';
  m[27]=']';
  m[28]='-';
  m[29]='-';
  m[30]='a';
  m[31]='s';
  m[32]='d';
  m[33]='f';
  m[34]='g';
  m[35]='h';
  m[36]='j';
  m[37]='k';
  m[38]='l';
  m[39]=';';
  m[40]='\'';
  m[41]='-';
  m[42]='-';
  m[43]='\\';
  m[44]='z';
  m[45]='x';
  m[46]='c';
  m[47]='v';
  m[48]='b';
  m[49]='n';
  m[50]='m';
  m[51]=',';
  m[52]='.';
  m[53]='/';
  m[54]='-';
  m[55]='-';
  m[56]='-';
  m[57]='-';
  m[58]='-';
  m[59]='f';
  m[60]='f';
  m[61]='f';
  m[62]='f';
  m[63]='f';
  m[64]='f';
  m[65]='f';
  m[66]='f';
  m[67]='f';
  m[68]='f';
  m[69]='-';
  m[70]='-';
  m[71]='7';
  m[72]='8';
  m[73]='9';
  m[74]='-';
  m[75]='4';
  m[76]='5';
  m[77]='6';
  m[78]='+';
  m[79]='1';
  m[80]='2';
  m[81]='3';
  m[82]='0';
  m[83]='.';
  m[101]='-';
  m[102]='-';
  m[107]='-';
  m[108]='-';
  m[109]='-';
  m[110]='-';
  m[111]='-';
  m[112]='-';
}

void clockInterrupt() {
  int clockValue = digitalRead(clockPin);
  if (clockValue == LOW && test == 0 && counter < numbits) {
    test = 1;
    data = data >> 1;
    if (digitalRead(dataPin) == HIGH) {
      bitSet(data, 7);
    }
    counter++;
  }
  if (clockValue == HIGH && test == 1) {
    test = 0;
  }
}

void setup() {
  pinMode(dataPin, INPUT);
  pinMode(clockPin, INPUT_PULLUP);
  Serial.begin(115200);
  setupKeyMapping();
  attachInterrupt(digitalPinToInterrupt(clockPin), clockInterrupt, CHANGE);
}

void loop() {
  if (counter >= numbits) {
    processByte(data);
    data = B0;
    counter = 0;
    repeatTimeout = millis() + repeatTimeoutValue;
  }
}
