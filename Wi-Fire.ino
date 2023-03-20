//#include <SoftwareSerial.h>
//SoftwareSerial Serial3(7, 6);
//Excuir este bloco para o Mega

#define LoRa Serial3
// #define LoRa Serial

#define IGN_1 36  /*act1*/
#define IGN_2 61  /*act2*/
#define IGN_3 46  /*act3*/
#define IGN_4 55  /*act4*/



// #define aciona 2
#define ON 1
#define holdIGN 10

char data = ' ';
char holder = ' ';

class Helpful
{
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

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  Serial.begin(9600);
  Serial3.begin(9600);
  // pinMode(aciona, OUTPUT);
  pinMode(IGN_1, OUTPUT);
  pinMode(IGN_2, OUTPUT);
  pinMode(IGN_3, OUTPUT);
  pinMode(IGN_4, OUTPUT);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
  ajuda.begin();
}

void loop()
{
  if (LoRa.available())
  {
    while (LoRa.available())
    {
      data = LoRa.read();
      Serial.write(data);
      if(data == 'S') // Aciona a ignição
      {
        ajuda.forT(holdIGN);
        holder = data;
      } else if(data == '*') // Retorna estado do sinal
      {
        Lora.print("Sistema operante, tempo T = ");
        LoRa.println(ajuda.sinceBegin());
      }
    }
  }
  if (!ajuda.forT()) holder = ' ';
  // digitalWrite(aciona, holder == 'S' ? ON : !ON);
  digitalWrite(IGN_1, holder == 'S' ? ON : !ON);
  digitalWrite(IGN_2, holder == 'S' ? ON : !ON);
  digitalWrite(IGN_3, holder == 'S' ? ON : !ON);
  digitalWrite(IGN_4, holder == 'S' ? ON : !ON);
  digitalWrite(A0, holder == 'S' ? !ON : ON);
  // if(holder == 'S') Serial.println("ON");

  if (Serial.available())
  {
    LoRa.write(Serial.read());
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else digitalWrite(LED_BUILTIN, LOW);
}

///Coleaao de utilitarios
void Helpful::begin()
{
  T0 = micros();
}
unsigned long Helpful::counter()
{
  return count++;
}
unsigned long Helpful::getCount()
{
  return count;
}
unsigned long Helpful::counterReset()
{
  unsigned long C = count;
  count = 0;
  return  C;
}
bool Helpful::eachN(unsigned int N)
{
  conEach++;
  return (N == 0) ? false : conEach % N == 0;
}
bool Helpful::eachT(float T)
{
  if (micros() - timerEach > (unsigned long)(T * 1000000.0))
  {
    timerEach = micros();
    return true;
  }
  return false;
}
void Helpful::forT(float T)
{
  endT = micros() + (long)(T * 1000000.0);
  forTstate = 1;
}
bool Helpful::forT()
{
  if (micros() > endT && forTstate)
  {
    endT = 0;
    return forTstate = 0;
  }
  return forTstate;
}
float Helpful::lapse()
{
  unsigned long tnow = micros(), t = tnow - lapseT;
  lapseT = tnow;
  return (float)t / 1000000.0;
}
float Helpful::sinceBegin()
{
  return float(micros() - T0) / 1000000.0;
}
bool Helpful::oneTime()
{
  if (!one)
  {
    one = 1;
    return true;
  }
  return false;
}
void Helpful::oneTimeReset()
{
  if (one) one = 0;
}
void Helpful::comparer(float n)
{
  maxi = (n > maxi) ? n : maxi;
  mini = (n < mini) ? n : mini;
}
float Helpful::getMax()
{
  return maxi;
}
float Helpful::getMin()
{
  return mini;
}

