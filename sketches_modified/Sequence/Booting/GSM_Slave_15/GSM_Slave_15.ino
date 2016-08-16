
//GSM pins
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <avr/interrupt.h>
#include "Adafruit_FONA.h"
#include <Wire.h>

/*********** G S M code******************/
#ifdef __AVR__
  #include <SoftwareSerial.h>
  SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
  SoftwareSerial *fonaSerial = &fonaSS;
#else
  HardwareSerial *fonaSerial = &Serial1;
#endif

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup() {
  //GSM setup
  Serial.begin(9600);
  Serial.println("FONA initializing, takes 3 seconds");
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while(1);
  }
  Serial.println(F("FONA is OK"));  
 //Signalling
  pinMode(8,INPUT);   //Signal from the second GSM 
 
  Serial.println("End of setup");

  //I2C - get coordinates
  Wire.begin();
  

}

void loop() 
{
  // put your main code here, to run repeatedly:
 // Serial.print(F("FONA> "));
  volatile int stats = digitalRead(8);
  if(stats)
  {
  char message[17];
  Wire.requestFrom(5,17);
  Serial.println("Request made");
 
  int a;
  while(Wire.available())
  {
    Serial.println("Entered while");
    for(a=0; a<17;a++)
    {
      message[a] = Wire.read();
    }
  }
  message[a] = '\0';
  Serial.println(message);

  char website[160] = "http://ctrlq.org/maps/address/#";
  strcat(website,message);;
  Serial.println(website);
  
  Serial.println("Button triggered\n");
  char sendto[21] = "7143190447";
    if (!fona.sendSMS(sendto, website)) 
    {
        Serial.println(F("Failed"));
    } 
    else 
    {
        Serial.println(F("Sent!"));
    }
  }
  delay(100);
  
}
