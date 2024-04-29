#ifdef ARDUINO_AVR_MEGA2560 // Arduino MEGA

#define SD_CS_PIN 53

HardwareSerial &LoRa = Serial3;

#else // Arduino UNO

#define SD_CS_PIN 10
#include <SoftwareSerial.h>
SoftwareSerial LoRa(2, 3);

#endif // ARDUINO_AVR_MEGA2560

#define PROPER_BOARD 1 // Quando utilizar a placa construída para o sistema

#define USE_RGB (1 && PROPER_BOARD)
#define USE_BUTTONS (1 && PROPER_BOARD)

#define BuZZ (1)
#define BEEPING (BuZZ && 1)



#if USE_RGB
#define LED_ON 0

#define LED_R 32
#define LED_G 36
#define LED_B 34
#endif // USE_RGB

#if USE_BUTTONS
#define B_ON 1

#define B_B 24
#define B_R 26
#define B_G 28
#define B_Y 30
#endif // USE_BUTTONS

#if BuZZ
#define buzzPin 22              //Pin that the buzzer is connected
// #define buzzPin 13              //Pin that the buzzer is connected
#define buzzCmd LOW             //Buzzer is on in high state
#endif // BuZZ


#if PROPER_BOARD

#define IGN A0

#else

#define IGN_1 36 /*act1*/
#define IGN_2 61 /*act2*/
#define IGN_3 46 /*act3*/
#define IGN_4 55 /*act4*/

#endif //PROPER_BOARD

#define ON_SIG 1
#define holdIGN 10

#define M0_LORA_PIN 12 // Pinos adicionais do LoRa
#define M1_LORA_PIN 11 // Pinos adicionais do LoRa
#define AUX_LORA_PIN A8 // Pinos adicionais do LoRa

int data = ' ';
int holder = ' ';
int pingCheck = 0;

#if USE_BUTTONS
enum Buttons{blue=1, red=2, green=3, yellow=4};
const Buttons password[] = {blue, green, blue, blue, green};
int passwordIndex = 0;
const int passwordLen = sizeof(password)/sizeof(password[0]);
#endif // USE_BUTTONS

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

#if USE_RGB
inline void rgb(bool r, bool g, bool b){
  digitalWrite(LED_R, r);
  digitalWrite(LED_G, g);
  digitalWrite(LED_B, b);
}
#endif // USE_RGB

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);

  LoRa.begin(9600);
  pinMode(M0_LORA_PIN,OUTPUT); digitalWrite(M0_LORA_PIN,LOW);
  pinMode(M1_LORA_PIN,OUTPUT); digitalWrite(M1_LORA_PIN,LOW);
  // pinMode(AUX_LORA_PIN,OUTPUT); digitalWrite(AUX_LORA_PIN,LOW);

#if PROPER_BOARD
  pinMode(IGN, OUTPUT); digitalWrite(IGN, !ON_SIG);
#else
  pinMode(IGN_1, OUTPUT); digitalWrite(IGN_1, !ON_SIG);
  pinMode(IGN_2, OUTPUT); digitalWrite(IGN_2, !ON_SIG);
  pinMode(IGN_3, OUTPUT); digitalWrite(IGN_3, !ON_SIG);
  pinMode(IGN_4, OUTPUT); digitalWrite(IGN_4, !ON_SIG);
#endif // PROPER_BOARD

#if USE_BUTTONS
  pinMode(B_G, INPUT);
  pinMode(B_R, INPUT);
  pinMode(B_B, INPUT);
  pinMode(B_Y, INPUT);
#endif //USE_BUTTONS

#if USE_RGB
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
#endif //USE_RGB

  ajuda.begin();

#if BuZZ
  pinMode(buzzPin, OUTPUT);
  digitalWrite(buzzPin, !buzzCmd);
#endif // BuZZ

#if USE_RGB  
  // Serial.println("Branco");
  rgb(LED_ON,LED_ON,LED_ON);
  delay(100);
  // Serial.println("Vermelho");
  rgb(LED_ON,!LED_ON,!LED_ON);
  delay(100);
  // Serial.println("Amarelo");
  rgb(LED_ON,LED_ON,!LED_ON);
  delay(100);
  // Serial.println("Verde");
  rgb(!LED_ON,LED_ON,!LED_ON);
  delay(100);
  // Serial.println("Ciano");
  rgb(!LED_ON,LED_ON,LED_ON);
  delay(100);
  // Serial.println("Azul");
  rgb(!LED_ON,!LED_ON,LED_ON);
  delay(100);
  // Serial.println("Magenta");
  rgb(LED_ON,!LED_ON,LED_ON);
  delay(100);
  // Serial.println("Preto");
  rgb(!LED_ON,!LED_ON,!LED_ON);
  delay(100);
#endif // USE_RGB

#if BEEPING
  beep();
#endif // BEEPING
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

#if USE_BUTTONS
  /* Botão Vermelho */
  if (digitalRead(B_R) == B_ON) {
    Serial.println("B_R");
    #if USE_RGB
    rgb(LED_ON,LED_ON,LED_ON);
    #endif //USE_RGB
    while (digitalRead(B_R) == B_ON);

    if(passwordIndex == passwordLen) {
      LoRa.write('S');
      Serial.print("Enviou: S");
      passwordIndex = 0;
    }

    if(password[passwordIndex] == red) passwordIndex++;
    else passwordIndex = 0;
  }
  
  /* Botão Azul */
  if (digitalRead(B_B) == B_ON) {
    Serial.println("B_B");
    #if USE_RGB
    rgb(LED_ON,LED_ON,LED_ON);
    #endif // USE_RGB
    while (digitalRead(B_B) == B_ON);

    if(password[passwordIndex] == blue) passwordIndex++;
    else passwordIndex = 0;
  }
  
  /* Botão Verde */
  if (digitalRead(B_G) == B_ON) {
    Serial.println("B_G");
    #if USE_RGB
    rgb(LED_ON,LED_ON,LED_ON);
    #endif // USE_RGB
    while (digitalRead(B_G) == B_ON);

    if(password[passwordIndex] == green) passwordIndex++;
    else passwordIndex = 0;
  }
  
  /* Botão Amarelo */
  if (digitalRead(B_Y) == B_ON) {
    Serial.println("B_Y");
    #if USE_RGB
    rgb(LED_ON,LED_ON,LED_ON);
    #endif // USE_RGB
    while (digitalRead(B_Y) == B_ON);

    if(password[passwordIndex] == yellow) passwordIndex++;
    else passwordIndex = 0;

    LoRa.write('*');
    Serial.println("Enviou: *");
  }
#endif // USE_BUTTONS

  if(passwordIndex == passwordLen) pingCheck = 2;


  #if USE_RGB
  if(passwordIndex == passwordLen) // Estado armado
    rgb(LED_ON,!LED_ON,!LED_ON);
  else if(passwordIndex>0) // Senha em percurso
    rgb(LED_ON,LED_ON,!LED_ON);
  else if(pingCheck > 0) // Ping com resposta positiva
    rgb(!LED_ON,LED_ON,!LED_ON);
  else if(ajuda.forT()) // Estado de acionamento
    rgb(LED_ON,!LED_ON,LED_ON);
  else // Estado normal
    rgb(!LED_ON,!LED_ON,LED_ON);
  #endif // USE_RGB
  
  if (!ajuda.forT()) holder = ' ';
  #if PROPER_BOARD
  digitalWrite(IGN, holder == 'S' ? ON_SIG : !ON_SIG);
  #else
  digitalWrite(IGN_1, holder == 'S' ? ON_SIG : !ON_SIG);
  digitalWrite(IGN_2, holder == 'S' ? ON_SIG : !ON_SIG);
  digitalWrite(IGN_3, holder == 'S' ? ON_SIG : !ON_SIG);
  digitalWrite(IGN_4, holder == 'S' ? ON_SIG : !ON_SIG);
  #endif // PROPER_BOARD
  // digitalWrite(buzzPin, holder == 'S' ? ON_SIG : !ON_SIG);
  // if(holder == 'S') Serial.println("ON_SIG");
  
  #if BEEPING
  if(pingCheck != 0){
    if(beep(pingCheck > 0?3:1)) pingCheck=pingCheck+((pingCheck > 0) ? (-1) : (1));
  }
  else beep();
  #endif // BEEPING

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