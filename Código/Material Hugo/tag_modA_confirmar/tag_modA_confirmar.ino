//Definicion de pines
//0013A20040BA9759
#include <TimerOne.h>
const long rout = 0x00 + 0x13 + 0xA2 + 0x00 + 0x40 + 0xBA + 0x97 + 0x59;//Direccion MAC a cambiar
long rec;
byte numeros[10] = {0X30, 0X31, 0X32, 0X33, 0X34, 0X35, 0X36, 0X37, 0X38, 0X39};
char data;
byte trash[30];
int i;
byte display[11] = {63, 06, 91, 79, 102, 109, 125, 07, 127, 103, 64};
int tuni = 6;          // tdec en el pin 23
int tdec = 5;          // tdec en el pin 23
int tcen = 3;          // tdec en el pin 23
int tmil = 2;          // tdec en el pin 23
int led = 4;
int segA = 8;
int segB = 9;
int segC = 10;
int segD = 11;
int segE = 12;
int segF = 13;
int segG = 7;
int piezas, confirmar, incremento, decremento;

int recibido = 0;
int dir, dir_rec, dirdec, diruni, dircen, cant, estado = 0, uni, dec, cen, mil, envia = 0, dato = 0, veces;
unsigned int tini, tfin;
int diru, dird, dirc;
int mac1, mac2, mac3, mac4;


void setup() {

  pinMode(segA, OUTPUT);        //define pin 43 como salida
  pinMode(segB, OUTPUT);       //define pin 44 como salida
  pinMode(segC, OUTPUT);       //define pin 45 como salida
  pinMode(segD, OUTPUT);       //define pin 46 como salida
  pinMode(segE, OUTPUT);      //define pin 47 como salida
  pinMode(segF, OUTPUT);      //define pin 48 como salida
  pinMode(segG, OUTPUT);      //define pin 49 como salida
  pinMode(tuni, OUTPUT);      //define pin 49 como salida
  pinMode(led, OUTPUT);
  pinMode(tdec, OUTPUT);      //define pin 49 como salida
  pinMode(tcen, OUTPUT);      //define pin 49 como salida
  pinMode(tmil, OUTPUT);      //define pin 49 como salida
  Serial.begin(115200);
  Timer1.initialize(500000);
  Timer1.detachInterrupt();
  estado = 0;
  digitalWrite(led, LOW);
  recibido = 0;
  cant = 4;
  veces = 250;
  while (veces > 0)
  {
    mostrar(cant);
    veces--;
  }
  digitalWrite(tuni, LOW);   // apaga el display de decenas
  digitalWrite(tdec, LOW);   // enciende el display de unidades
  digitalWrite(tcen, LOW);   // apaga el display de decenas
  digitalWrite(tmil, LOW);   // enciende el display de unidades
  while(Serial.available() > 0) 
      {
          i = Serial.read();//continues reading byte
      }  
}

//Descompone el numero en sus factores y retorna la longitud igual a 4

long msg(int valor, int dire) {
  long value = 7;
  mil = 0;
  cen = 0;
  dec = 0;
  uni = 0;
  mil = valor / 1000;
  valor = valor % 1000;
  cen = valor / 100;
  valor = valor % 100;
  dec = valor / 10;
  uni = valor % 10;
  dirc = dire / 100;
  dire = dire % 100;
  dird = dire / 10;
  diru = dire % 10;
  Serial.write(byte(numeros[mil]));
  Serial.write(byte(numeros[cen]));
  Serial.write(byte(numeros[dec]));
  Serial.write(byte(numeros[uni]));
  Serial.write(byte(numeros[dirc]));
  Serial.write(byte(numeros[dird]));
  Serial.write(byte(numeros[diru]));
  return value;
}

//Calcula el checksum

//@param receiver is the High and Low Destinatinon Address
//@param text is the length of the message sent
//@return cs is the value of the checksum. calculated by adding the individual bytes of the API frame except for delimiter and frame length
long checksum(long receiver, long text) {
  long cs = 0x10 + 0x00 + receiver + 0xFF + 0xFE + numeros[mil] + numeros[cen] + numeros[dec] + numeros[uni] + numeros[dirc] + numeros[dird] + numeros[diru];
  return cs;
}

////0013A20040BA9759
//Envia el dato de 4 bytes

//@param to represents the receiver of the message
//@param message[] is a string containing the message to be sent
void chat() {
  int frameLen = 7 + 14;//length of the API frame is calculated
  long len = ("0x%p\n", frameLen);//frame length is converted to type long
  Serial.write(byte(0x7E));//delimiter
  Serial.write(byte(0x00));//length LSB
  Serial.write(byte(len));//length MSB
  Serial.write(byte(0x10));//frameType (transmit request)
  Serial.write(byte(0x00));//frameId
  Serial.write(byte(0x00));//high destination address
  Serial.write(byte(0x13));//high destination address
  Serial.write(byte(0xA2));//high destination address
  Serial.write(byte(0x00));//high destination address
  Serial.write(byte(0x40));//high destination address
  Serial.write(byte(0xBA));
  Serial.write(byte(0x97));
  Serial.write(byte(0x59));
  rec = rout;//places destination address in variable 'rec' type long
  Serial.write(byte(0xFF));//const
  Serial.write(byte(0xFE));//const
  Serial.write(byte(0x00));//const (options)
  Serial.write(byte(0x00));//const (options)
  long mVal = msg(dato, dir_rec); //calls method to write message to serial and sets message length in variable 'mVal' type long
  long cs = checksum(rec, mVal); //sets checksum value to variable 'cs' type long
  Serial.write(byte(0xFF - (cs & 0xFF)));//checksum is fully calculated and written to serial
}


void mostrar(int cont)
{
  int valor;
  valor = cont;
  if (valor > 0)
  {
    mil = cont / 1000;
    cont = cont % 1000;
    cen = cont / 100;
    cont = cont % 100;
    dec = cont / 10;
    uni = cont % 10;
    int2bcd(uni);
    digitalWrite(tuni, HIGH);   // apaga el display de decenas
    digitalWrite(tdec, LOW);   // enciende el display de unidades
    digitalWrite(tcen, LOW);   // apaga el display de decenas
    digitalWrite(tmil, LOW);   // enciende el display de unidades
    delay(1);                    //retardo de 1 milisegundo
    digitalWrite(tuni, LOW);   // apaga el display de decenas

    int2bcd(dec);
    digitalWrite(tuni, LOW);   // apaga el display de decenas
    if (valor > 9)
    {
      digitalWrite(tdec, HIGH);   // enciende el display de unidades
    }
    else
    {
      digitalWrite(tdec, LOW);   // enciende el display de unidades
    }
    digitalWrite(tcen, LOW);   // apaga el display de decenas
    digitalWrite(tmil, LOW);   // enciende el display de unidades
    delay(1);                    //retardo de 1 milisegundo
    digitalWrite(tdec, LOW);   // enciende el display de unidades

    int2bcd(cen);
    digitalWrite(tuni, LOW);   // apaga el display de decenas
    digitalWrite(tdec, LOW);   // enciende el display de unidades
    if (valor > 99)
    {
      digitalWrite(tcen, HIGH);   // enciende el display de unidades
    }
    else
    {
      digitalWrite(tcen, LOW);   // enciende el display de unidades
    }
    digitalWrite(tmil, LOW);
    delay(1);                    //retardo de 1 milisegundo
    digitalWrite(tcen, LOW);   // enciende el display de unidades

    int2bcd(mil);
    digitalWrite(tuni, LOW);   // apaga el display de decenas
    digitalWrite(tdec, LOW);   // enciende el display de unidades
    digitalWrite(tcen, LOW);   // apaga el display de decenas
    if (valor > 999)
    {
      digitalWrite(tmil, HIGH);   // enciende el display de unidades
    }
    else
    {
      digitalWrite(tmil, LOW);   // enciende el display de unidades
    }
    delay(1);                    //retardo de 1 milisegundo
    digitalWrite(tmil, LOW);   // enciende el display de unidades
  }
}


void int2bcd(int num)  //Funcion que toma un numero entero y lo convierte a BCD
{
  switch (num)
  {
    case 0:
      digitalWrite(segA, HIGH);
      digitalWrite(segB, HIGH);
      digitalWrite(segC, HIGH);
      digitalWrite(segD, HIGH);
      digitalWrite(segE, HIGH);
      digitalWrite(segF, HIGH);
      digitalWrite(segG, LOW);

      break;

    case 1:
      digitalWrite(segA, LOW);
      digitalWrite(segB, HIGH);
      digitalWrite(segC, HIGH);
      digitalWrite(segD, LOW);
      digitalWrite(segE, LOW);
      digitalWrite(segF, LOW);
      digitalWrite(segG, LOW);
      break;

    case 2:
      digitalWrite(segA, HIGH);
      digitalWrite(segB, HIGH);
      digitalWrite(segC, LOW);
      digitalWrite(segD, HIGH);
      digitalWrite(segE, HIGH);
      digitalWrite(segF, LOW);
      digitalWrite(segG, HIGH);
      break;

    case 3:
      digitalWrite(segA, HIGH);
      digitalWrite(segB, HIGH);
      digitalWrite(segC, HIGH);
      digitalWrite(segD, HIGH);
      digitalWrite(segE, LOW);
      digitalWrite(segF, LOW);
      digitalWrite(segG, HIGH);
      break;

    case 4:
      digitalWrite(segA, LOW);
      digitalWrite(segB, HIGH);
      digitalWrite(segC, HIGH);
      digitalWrite(segD, LOW);
      digitalWrite(segE, LOW);
      digitalWrite(segF, HIGH);
      digitalWrite(segG, HIGH);
      break;

    case 5:
      digitalWrite(segA, HIGH);
      digitalWrite(segB, LOW);
      digitalWrite(segC, HIGH);
      digitalWrite(segD, HIGH);
      digitalWrite(segE, LOW);
      digitalWrite(segF, HIGH);
      digitalWrite(segG, HIGH);
      break;

    case 6:
      digitalWrite(segA, LOW);
      digitalWrite(segB, LOW);
      digitalWrite(segC, HIGH);
      digitalWrite(segD, HIGH);
      digitalWrite(segE, HIGH);
      digitalWrite(segF, HIGH);
      digitalWrite(segG, HIGH);
      break;

    case 7:
      digitalWrite(segA, HIGH);
      digitalWrite(segB, HIGH);
      digitalWrite(segC, HIGH);
      digitalWrite(segD, LOW);
      digitalWrite(segE, LOW);
      digitalWrite(segF, LOW);
      digitalWrite(segG, LOW);
      break;

    case 8:
      digitalWrite(segA, HIGH);
      digitalWrite(segB, HIGH);
      digitalWrite(segC, HIGH);
      digitalWrite(segD, HIGH);
      digitalWrite(segE, HIGH);
      digitalWrite(segF, HIGH);
      digitalWrite(segG, HIGH);
      break;

    case 9:
      digitalWrite(segA, HIGH);
      digitalWrite(segB, HIGH);
      digitalWrite(segC, HIGH);
      digitalWrite(segD, LOW);
      digitalWrite(segE, LOW);
      digitalWrite(segF, HIGH);
      digitalWrite(segG, HIGH);
      break;
  }
}


void intermitencia()
{
  if (digitalRead(led) == LOW)
  {
    digitalWrite(led, HIGH);
  }
  else
  {
    digitalWrite(led, LOW);
  }
}
void loop()
{
  switch (estado) {
    case 0:
      digitalWrite(led, HIGH);
      if (recibido == 1)
      {
        mil = trash[12];
        cen = trash[13];
        dec = trash[14];
        uni = trash[15];
        dircen = trash[16];
        dirdec = trash[17];
        diruni = trash[18];
        uni = uni - 48;
        dec = dec - 48;
        cen = cen - 48;
        mil = mil - 48;
        dircen = dircen - 48;
        dirdec = dirdec - 48;
        diruni = diruni - 48;
        dir_rec = dircen * 100 + dirdec * 10 + diruni;
        cant = mil * 1000 + cen * 100 + dec * 10 + uni;
        recibido = 0;
        estado = 1;
        Timer1.attachInterrupt(intermitencia);
      }
      break;

    case 1:
      mostrar(cant);
      confirmar = analogRead(1);
      delay(1);
      incremento = analogRead(2);
      delay(1);
      decremento = analogRead(0);
      delay(1);
      if (incremento > 200)
      {
        cant++;
        estado = 2;
        tini = millis();
      }
      if ((decremento > 200) && (cant > 0))
      {
        cant--;
        estado = 2;
        tini = millis();
      }
      if (confirmar > 200)
      {
        estado = 3;
      }
      if(recibido==1)
      {
        recibido=0;
      }
     break;

    case 2:
      mostrar(cant);
      tfin = millis();
      if (tfin - tini > 300)
      {
        estado = 1;
      }
      break;

    case 3:
      digitalWrite(tuni, LOW);   // apaga el display de decenas
      digitalWrite(tdec, LOW);   // enciende el display de unidades
      digitalWrite(tcen, LOW);   // apaga el display de decenas
      digitalWrite(tmil, LOW);
      digitalWrite(led, LOW);
      dato = cant;
      chat();
      Timer1.detachInterrupt();
      tini = millis();
      estado = 4;
      break;

    case 4:
      tfin = millis();
      if (tfin - tini > 2000)
      {
        estado = 3;
      }
      if (recibido == 2)
      {
        recibido = 0;
        dircen = trash[16];
        dirdec = trash[17];
        diruni = trash[18];
        dircen = dircen - 48;
        dirdec = dirdec - 48;
        diruni = diruni - 48;
        dir = dircen * 100 + dirdec * 10 + diruni;
        if (dir == dir_rec)
        {
          estado = 0;
          Serial.print("Estado: ");
          Serial.println(estado);
        }
      }
      if (recibido == 1)
      {
        recibido=0;
      }
      break;
  }
}




void serialEvent()
{
  delay(11);//small delay to let Serial buffer(s) load
  String st;
  if (Serial.available() >= 22) { //require a minimum frame length of 17 (message with one character at least)
    data = Serial.read();//data read from Serial is stored in variable 'data' type char
    if (data == 0x7E) {
      //assures that incoming data is an API frame from another xbee

      data = Serial.read();
      data = Serial.read();
      for (int i = 0; i < data; ++i) {//cycles through the next 14 bytes
        trash[i] = Serial.read();//places bytes in variable 'trash' type byte
      }
      //i = -1;
      i = Serial.read();//continues reading bytes
    dircen = trash[16];
    dirdec = trash[17];
    diruni = trash[18];
    dircen = dircen - 48;
    dirdec = dirdec - 48;
    diruni = diruni - 48;
    dir_rec = dircen * 100 + dirdec * 10 + diruni;
    mac1 = trash[5];
    mac2 = trash[6];
    mac3 = trash[7];
    mac4 = trash[8];
   ////0013A20040BA9759
   if((mac1==0x40)&&(mac2==0xBA)&&(mac3==0x97)&&(mac4==0x59)&&(dir_rec < 36))
    {
      recibido = 1;
      mil = trash[12];
      cen = trash[13];
      dec = trash[14];
      uni = trash[15];
      uni = uni - 48;
      dec = dec - 48;
      cen = cen - 48;
      mil = mil - 48;
      cant = mil * 1000 + cen * 100 + dec * 10 + uni;
      confirmar = analogRead(1);
        delay(1);
        if (cant == 0)
        {
          recibido = 0;
          estado = 0;
          dato = cant;
          if(confirmar<200)
          {
          chat();
          }
          digitalWrite(tuni, LOW);   // apaga el display de decenas
          digitalWrite(tdec, LOW);   // enciende el display de unidades
          digitalWrite(tcen, LOW);   // apaga el display de decenas
          digitalWrite(tmil, LOW);   // enciende el display de unidades
        }
        if (cant > 0 && cant < 9999 && confirmar < 200)
        {
          dato = cant;
          chat();
        }
        if (cant > 0 && cant < 9999 && confirmar > 200)
        {
          recibido = 0;
        }
        if (cant == 9999)
        {
          dato = cant;
          recibido=2;
        }
         }
      else
      {
        recibido = 0;
      }
    }
  }
}
