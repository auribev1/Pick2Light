//0013A200417FC543
const long rout = 0x00 + 0x13 + 0xA2 + 0x00 + 0x41 + 0x07 + 0x89 + 0x91;//Direccion MAC a cambiar
long rec;
byte numeros[10] = {0X30, 0X31, 0X32, 0X33, 0X34, 0X35, 0X36, 0X37, 0X38, 0X39};
char data;
byte display[11] = {63, 06, 91, 79, 102, 109, 125, 07, 127, 103, 64};
byte trash[24];
int i;
int rojo = 2;          // tdec en el pin 23
int verde = 6;

int recibido = 0;
int dir, dir_rec, dirdec, diruni, dircen, cant, uni, dec, cen, mil, envia = 0, dato = 0;
int diru, dird, dirc,bytefinal;
int mac1, mac2, mac3, mac4;


void setup() {

  pinMode(rojo, OUTPUT);      //define pin 49 como salida
  pinMode(verde, OUTPUT);      //define pin 49 como salida
  Serial.begin(115200);
  digitalWrite(rojo, HIGH);
  digitalWrite(verde, LOW);
   bytefinal=0;

}
//Descompone el numero en sus factores y retorna la longitud igual a 4

long msg(int valor, int dire) {
  long value = 8;
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
  Serial.write(byte(numeros[bytefinal]));
  return value;
}
//Calcula el checksum

//@param receiver is the High and Low Destinatinon Address
//@param text is the length of the message sent
//@return cs is the value of the checksum. calculated by adding the individual bytes of the API frame except for delimiter and frame length
long checksum(long receiver, long text) {
  long cs = 0x10 + 0x00 + receiver + 0xFF + 0xFE + numeros[mil] + numeros[cen] + numeros[dec] + numeros[uni] + numeros[dirc] + numeros[dird] + numeros[diru]+numeros[bytefinal];
  return cs;
}

//0013A20041078991
//Envia el dato de 4 bytes

//@param to represents the receiver of the message
//@param message[] is a string containing the message to be sent
void chat() {
  int frameLen = 8 + 14;//length of the API frame is calculated
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
  Serial.write(byte(0x41));//high destination address
  Serial.write(byte(0x07));
  Serial.write(byte(0x89));
  Serial.write(byte(0x91));
  rec = rout;//places destination address in variable 'rec' type long
  Serial.write(byte(0xFF));//const
  Serial.write(byte(0xFE));//const
  Serial.write(byte(0x00));//const (options)
  Serial.write(byte(0x00));//const (options)
  long mVal = msg(dato, dir_rec); //calls method to write message to serial and sets message length in variable 'mVal' type long
  long cs = checksum(rec, mVal); //sets checksum value to variable 'cs' type long
  Serial.write(byte(0xFF - (cs & 0xFF)));//checksum is fully calculated and written to serial
}

void loop()
{
}


void serialEvent()
{
  delay(11);//small delay to let Serial buffer(s) load
  String st;
  if (Serial.available() >= 22) {//require a minimum frame length of 17 (message with one character at least)
    data = Serial.read();//data read from Serial is stored in variable 'data' type char
    if (data == 0x7E) {//assures that incoming data is an API frame from another xbee
      data = Serial.read();
      data = Serial.read();
      for (int i = 0; i < data; ++i) {//cycles through the next 14 bytes
        trash[i] = Serial.read();//places bytes in variable 'trash' type byte
      }
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
     if ((mac1 == 0x41) && (mac2 == 0x07) && (mac3 == 0x89) && (mac4 == 0x91))
      {
        mil = trash[12];
        cen = trash[13];
        dec = trash[14];
        uni = trash[15];
        uni = uni - 48;
        dec = dec - 48;
        cen = cen - 48;
        mil = mil - 48;
        cant = mil * 1000 + cen * 100 + dec * 10 + uni;
        if (cant == 0)
        {
          digitalWrite(rojo, LOW);
          digitalWrite(verde, HIGH);
          dato = cant;
          chat();

        }
        if (cant == 1)
        {
          digitalWrite(rojo, HIGH);
          digitalWrite(verde, LOW);
          dato = cant;
          chat();
        }
      }
    }
     while (Serial.available() > 0)
      {
        i = Serial.read();//continues reading byte
      }
  }
}
