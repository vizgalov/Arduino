#include <OneWire.h>  //подключаем библиотеку
 
OneWire  ds(15);  // Номер ноги к которой подключили датчик
 
void setup(void) {
  Serial.begin(115200);   // настройки ком порта
}

static byte t_addr[8] = {0x28, 0xFF, 0x9D, 0x20, 0x4B, 0x04, 0x00, 0xA8};
byte t_data[12];

int8_t readLowTemp(double &temp) {
  byte i;

  ds.reset();
  ds.select(t_addr);
  ds.write(0x44, 1);      

  delay(1000);     

  if (ds.reset() == 0) return -1;
  ds.select(t_addr);    
  ds.write(0xBE);        

  for ( i = 0; i < 9; i++) {          
    t_data[i] = ds.read();
  }

  if (OneWire::crc8(t_data, 8) != t_data[8]) return -2;
 
//  Преобразование данных в фактическую температуру
 
  int16_t raw = (t_data[1] << 8) | t_data[0];

  temp = (double)raw / 16.0;
  return 0;
}

void loop(void) {

  double c = 0;

  Serial.println(readLowTemp(c));
  
    Serial.println(c);
  
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  double celsius;

  long start = millis();

  Serial.print("Start");
  Serial.println();
  
  ds.reset();
  ds.select(t_addr);
  ds.write(0x44, 1);      

  Serial.print("p1: ");
  Serial.println(millis() - start);

  delay(1000);     

  Serial.print("p2: ");
  Serial.println(millis() - start);

  present = ds.reset();
  ds.select(t_addr);    
  ds.write(0xBE);        

  Serial.print("p3: ");
  Serial.println(millis() - start);

  for ( i = 0; i < 9; i++) {          
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  if (OneWire::crc8(data, 8) != data[8]) {
  Serial.print(" CRC mismatch CRC =");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();
    
  }
 
//  Преобразование данных в фактическую температуру
 
    int16_t raw = (data[1] << 8) | data[0];
  Serial.println();
  Serial.println(raw, HEX);




  celsius = (double)raw / 16.0;
  Serial.print("  Temperature = ");
  Serial.println(celsius);

  Serial.print("p4: ");
  Serial.println(millis() - start);

  
  delay(3000);
}
