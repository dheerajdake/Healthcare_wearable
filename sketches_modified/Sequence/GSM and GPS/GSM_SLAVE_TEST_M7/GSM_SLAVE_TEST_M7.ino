/*
 * GSM broadcasting location based alerts
 * 9/18/15 - Dheeraj Dake
 * 
 * The GSM device gets the latitute and longitude from GPS over I2C bus. These coordinates are then 
 * broadcasted to the users using Cellular network. The red button should be held down for 4 seconds
 * to send the 'OH SHIT' message. Digital pin 9 is configured as output to hook up the switch and digital
 * pin 10 is configured as input. 
 * 
 * The pin 10 will be floating. I added a pull down resistor to get the voltage to zero. The second GSM
 * needs to be added. This will come into play when then the first signal strength drops low. The data
 * should be moved from GSM1 to GSM2 via I2C bus and the GSM2 broadcasts it. If GSM2 fails then check
 * the RSSI of GSM1 and proceed. 
 * 
 * 9/20/15
 * Switch and buzzer connections.
 * I changed the connections since GPS is using 7 and 8. The wire which is input to the red button goes into pin 9. The second wire from the 
 * switch goes into the pin 10. The wire from 11 goes to the buzzer (red leg). The wire from buzzer(black leg) goes into ground. Check the excel sheet for GPS connections.
 */
#include "Adafruit_FONA.h"
#include <Wire.h>
#include <avr/interrupt.h>

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

// this is a large buffer for replies
char replybuffer[255];

// This is to handle the absence of software serial on platforms
// like the Arduino Due. Modify this code if you are using different
// hardware serial port, or if you are using a non-avr platform
// that supports software serial.
#ifdef __AVR__
  #include <SoftwareSerial.h>
  SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
  SoftwareSerial *fonaSerial = &fonaSS;
#else
  HardwareSerial *fonaSerial = &Serial1;
#endif

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

int buzz = 8;
volatile int state = LOW;

void setup() {
  while (!Serial);

  Serial.begin(115200);
  Wire.begin();
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while(1);
  }
  Serial.println(F("FONA is OK"));

  // Print SIM card IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("SIM card IMEI: "); Serial.println(imei);
  }

  // Optionally configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
  //fona.setGPRSNetworkSettings(F("your APN"), F("your username"), F("your password"));

  // Optionally configure HTTP gets to follow redirects over SSL.
  // Default is not to follow SSL redirects, however if you uncomment
  // the following line then redirects over SSL will be followed.
  //fona.setHTTPSRedirect(true);

  //led pins
  pinMode(9, OUTPUT);   //For the oh shit switch - red. Changed this to pin 9
  pinMode(10, INPUT);    //For the signalling. Change this to pin 10
  pinMode (11, OUTPUT);  //buzzer. Change this to pin 11
}

void loop() {
  float lat, lon;  
  //Serial.print(F("FONA> "));
  /*while (! Serial.available() ) {
    if (fona.available()) {
        Serial.write(fona.read());
    }
  }*/

  digitalWrite(9,HIGH);
  
  volatile int stat = digitalRead(10);     //always read from memory
  //Serial.println("Read.....................");
  //Serial.println(stat);
  ///////////////////FETCHING DATA FROM GPS MODULE//////////////
  #if 1    //get data from GPS slave
  while(Wire.available())
  {
    Wire.requestFrom(2,1);
    int a = Wire.read();
    float lat = a/10000;

    delay(200);
    Wire.requestFrom(2,1);
    int b = Wire.read();
    float lon = b/10000;

    //To access the GPS data from the module, connect it via I2C bus and use lat and lon instead of lat1 and lon1.
  }
  #endif

  ///////////////////////

  while(stat)
  {
    delay(4000);
    
    Serial.println("HIGH");
    
    #if 1
     float lat1 = 37.443225;
     float lon1 = -122.172173;
      char sendto[21] = "7143190447";  //hardcoded
      char message[160];
      Serial.println(lat);      //latitude
      Serial.println(lon);      //longitude
      //sprintf(message, "http://ctrlq.org/maps/address/#%d,-122.172173",lat1);  //coverting 
      sprintf(message, " OH SHIT! FIND ME HERE\nhttp://ctrlq.org/maps/address/#%6f,-122.172173",lat1);  //coverting 
      flushSerial();
      Serial.print(F("Sending to Danny"));
      //http://ctrlq.org/maps/address/#37.373685,-122.09835899999996
      Serial.println(message);
      if (!fona.sendSMS(sendto, message)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("Sent!"));
      }
 #endif
 digitalWrite(11,HIGH);
   
    return;
  }
   
  digitalWrite(11,LOW);
  delay(1000);
  Serial.println("LOW");
  
}

void flushSerial() {
    while (Serial.available()) 
    Serial.read();
}

char readBlocking() {
  while (!Serial.available());
  return Serial.read();
}
uint16_t readnumber() {
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //Serial.print(c);
  }
  Serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    Serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}
  
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;
  
  while (true) {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while(Serial.available()) {
      char c =  Serial.read();

      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;
        
        timeout = 0;         // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }
    
    if (timeoutvalid && timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}
