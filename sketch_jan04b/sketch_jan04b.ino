#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;

#define encoder0PinA  2
#define encoder0PinB  4

volatile unsigned int pos = 0;
volatile unsigned int cnt = 0;
int last_pos = 0;
int last_cnt = 0;

void setup()
{
     pinMode(encoder0PinA, INPUT); 
   pinMode(encoder0PinB, INPUT); 

    Serial.begin(9600);	  // Debugging only
    if (!driver.init())
         Serial.println("init failed");
  attachInterrupt(digitalPinToInterrupt(2), doEncoder, RISING);  // encoder pin on interrupt 0 - pin 2


}


uint8_t val = 0;


void loop()
{
    driver.send(&val, 1);
    driver.waitPacketSent();
    delay(1000);
    val = val + 8 ;
}
