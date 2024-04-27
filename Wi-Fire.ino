//#include <SoftwareSerial.h>
//SoftwareSerial Serial3(7, 6);
//Excuir este bloco para o Mega

#define LoRa Serial3
// #define LoRa Serial

#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/

#define LED_R 32
#define LED_G 36
#define LED_B 34

#define LED_ON 0

#define B_B 24
#define B_R 26
#define B_G 28
#define B_Y 30

#define B_ON 1

#define BuZZ (1)
#define BEEPING (BuZZ && 1)

#if BuZZ
#define buzzPin 22              //Pin that the buzzer is connected
// #define buzzPin 13              //Pin that the buzzer is connected
#define buzzCmd LOW             //Buzzer is on in high state
#endif // BuZZ


#define aciona A0
#define ON 0
#define holdIGN 10



int data = ' ';
int holder = ' ';
int estado = 0;
int pingCheck = 0;

class Helpful {
private:
  unsigned long conEach = 0;
  unsigned long timerEach = 0;
  unsigned long lapseT = 0;
  unsigned long T0 = 0;
  unsigned long endT = 0;
  bool forTstate = 0;
  bool one = 0;
  unsigned long count = 0;
  float maxi = 0, mini = 0;
public:
  bool mem;
  void begin();
  unsigned long counter();
  unsigned long getCount();
  unsigned long counterReset();
  bool eachN(unsigned int N);
  bool eachT(float T);
  void forT(float T);
  bool forT();
  float lapse();
  float sinceBegin();
  bool oneTime();
  void oneTimeReset();
  void comparer(float n);
  float getMax();
  float getMin();
};

Helpful ajuda;


#define holdT .1
#if BEEPING
Helpful beeper;
#endif // BEEPING
#define SYSTEM_n 5

void rgb(bool r, bool g, bool b){
  digitalWrite(LED_R, r);
  digitalWrite(LED_G, g);
  digitalWrite(LED_B, b);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  Serial.begin(9600);
  Serial3.begin(9600);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  // pinMode(A8,OUTPUT);
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);
  // digitalWrite(A8,LOW);
  pinMode(aciona, OUTPUT);
  digitalWrite(aciona, !ON);
  pinMode(B_G, INPUT);
  pinMode(B_R, INPUT);
  pinMode(B_B, INPUT);
  pinMode(B_Y, INPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  ajuda.begin();

#if BuZZ
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, !buzzCmd);
#endif // BuZZ
  
  Serial.println("Branco");
  rgb(LED_ON,LED_ON,LED_ON);
  delay(100);
  Serial.println("Vermelho");
  rgb(LED_ON,!LED_ON,!LED_ON);
  delay(100);
  Serial.println("Amarelo");
  rgb(LED_ON,LED_ON,!LED_ON);
  delay(100);
  Serial.println("Verde");
  rgb(!LED_ON,LED_ON,!LED_ON);
  delay(100);
  Serial.println("Ciano");
  rgb(!LED_ON,LED_ON,LED_ON);
  delay(100);
  Serial.println("Azul");
  rgb(!LED_ON,!LED_ON,LED_ON);
  delay(100);
  Serial.println("Magenta");
  rgb(LED_ON,!LED_ON,LED_ON);
  delay(100);
  Serial.println("Preto");
  rgb(!LED_ON,!LED_ON,!LED_ON);
  delay(100);

  beep();
}

void loop() {
  if (LoRa.available()) {
    while (LoRa.available()) {
      data = LoRa.read();
      Serial.write(data);
      if (data == 'S')  // Aciona a ignição
      {
        Serial.println("Ign!");
        ajuda.forT(holdIGN);
        holder = data;
        pingCheck = -5;
      } else if (data == '*')  // Retorna estado do sinal
      {
        LoRa.print("T = ");
        LoRa.println(ajuda.sinceBegin());
      } else if (data == 'T')  // Retorna estado do sinal
      {
        pingCheck = 3;
      }
    }
  }

  if (Serial.available()) {
    LoRa.write(Serial.read());
    digitalWrite(LED_BUILTIN, HIGH);
  } else digitalWrite(LED_BUILTIN, LOW);

  /* Botão Vermelho */
  if (digitalRead(B_R) == B_ON) {
    Serial.println("B_R");
    rgb(LED_ON,LED_ON,LED_ON);
    while (digitalRead(B_R) == B_ON);
    switch(estado){
      case 5: 
        LoRa.write('S');
      default: estado = 0;
    }
  }
  
  /* Botão Azul */
  if (digitalRead(B_B) == B_ON) {
    Serial.println("B_B");
    rgb(LED_ON,LED_ON,LED_ON);
    while (digitalRead(B_B) == B_ON);
    switch(estado){
      case 0:
      case 2:
      case 3:
        estado++; break;
      default: estado = 0;
    }
  }
  
  /* Botão Verde */
  if (digitalRead(B_G) == B_ON) {
    Serial.println("B_G");
    rgb(LED_ON,LED_ON,LED_ON);
    while (digitalRead(B_G) == B_ON);
    switch(estado){
      case 1:
      case 4:
        estado++; break;
      default: estado = 0;
    }
  }
  
  /* Botão Amarelo */
  if (digitalRead(B_Y) == B_ON) {
    Serial.println("B_Y");
    rgb(LED_ON,LED_ON,LED_ON);
    while (digitalRead(B_Y) == B_ON);
    LoRa.print("*");
    Serial.println("*");
  }

  // Serial.println(estado);
  switch(estado){
    case 1:
    case 2:
    case 3:
    case 4:
      rgb(LED_ON,LED_ON,!LED_ON);
      break;
    case 5:
      pingCheck = 2;
      rgb(LED_ON,!LED_ON,!LED_ON);
      break;
    default:
      if(pingCheck > 0) rgb(!LED_ON,LED_ON,!LED_ON);
      else rgb(!LED_ON,!LED_ON,LED_ON);
    
  }
  
  if (!ajuda.forT()) holder = ' ';
  digitalWrite(aciona, holder == 'S' ? ON : !ON);
  // digitalWrite(IGN_1, holder == 'S' ? ON : !ON);
  // digitalWrite(IGN_2, holder == 'S' ? ON : !ON);
  // digitalWrite(IGN_3, holder == 'S' ? ON : !ON);
  // digitalWrite(IGN_4, holder == 'S' ? ON : !ON);
  // digitalWrite(buzzPin, holder == 'S' ? ON : !ON);
  // if(holder == 'S') Serial.println("ON");
  
  if(pingCheck != 0){
    if(beep(pingCheck > 0?3:1)) pingCheck=pingCheck+((pingCheck > 0) ? (-1) : (1));
  }
  else beep();

}

///Coleaao de utilitarios
void Helpful::begin() {
  T0 = micros();
}
unsigned long Helpful::counter() {
  return count++;
}
unsigned long Helpful::getCount() {
  return count;
}
unsigned long Helpful::counterReset() {
  unsigned long C = count;
  count = 0;
  return C;
}
bool Helpful::eachN(unsigned int N) {
  conEach++;
  return (N == 0) ? false : conEach % N == 0;
}
bool Helpful::eachT(float T) {
  if (micros() - timerEach > (unsigned long)(T * 1000000.0)) {
    timerEach = micros();
    return true;
  }
  return false;
}
void Helpful::forT(float T) {
  endT = micros() + (long)(T * 1000000.0);
  forTstate = 1;
}
bool Helpful::forT() {
  if (micros() > endT && forTstate) {
    endT = 0;
    return forTstate = 0;
  }
  return forTstate;
}
float Helpful::lapse() {
  unsigned long tnow = micros(), t = tnow - lapseT;
  lapseT = tnow;
  return (float)t / 1000000.0;
}
float Helpful::sinceBegin() {
  return float(micros() - T0) / 1000000.0;
}
bool Helpful::oneTime() {
  if (!one) {
    one = 1;
    return true;
  }
  return false;
}
void Helpful::oneTimeReset() {
  if (one) one = 0;
}
void Helpful::comparer(float n) {
  maxi = (n > maxi) ? n : maxi;
  mini = (n < mini) ? n : mini;
}
float Helpful::getMax() {
  return maxi;
}
float Helpful::getMin() {
  return mini;
}

#if BEEPING
inline bool beep(unsigned int N)
{
  if (beeper.eachT(holdT*SYSTEM_n * 4) || beeper.oneTime())
  {
    beeper.mem = buzzCmd;
    beeper.counterReset();
    beeper.forT(holdT);
    return true;
  }
  if (beeper.getCount() < (N + 1) * 2) if (!beeper.forT())
  {
    digitalWrite(buzzPin, beeper.mem);
    beeper.mem = !beeper.mem;
    beeper.counter();
    beeper.forT(holdT);
  }
  return false;
}

inline bool beep()
{
  digitalWrite(buzzPin, !buzzCmd);
  beeper.counterReset();
  return false;
}
#endif // BEEPING