/* MODULES
 *  1. Fingerprint - Fingerprint on slave 14. The command from the OLED over i2c 
 *                   wakes up the fingerprint.
 *  2. Bluetooth - Bluetooth on slave 14. For additional security layer. Needs a pair code. 
 *  
 */

/* Dheeraj Dake - 8/30/15
 * All about functions
 * The UNO SL 14 has the fingerprintsensor on it. The fingerprint turns on when the command
 * is displayed on the OLED. A flag f is sent over the i2c to the fingerprint sensor for it
 * to wake up. It waits for the user to register his fingerprint. Once it's done, the 
 * fingerprint goes inactive. 
 * 
 * NEED TO DECIDE
 * When does the device locks itself? After a certain time period of inactivity or upon 
 * the user request?? 
 * 
 * The i2c is being ended and restarted to send the char from fingerprint to the OLED. When
 * the fingerprint registers a scan, the information is displayed on the OLED. 
 * 
 * FUNCTIONS
 * Wire.beginTransmission() should be followed by a Wire.write() and Wire.endTransmission() 
 * for the data to pass. If the wire is not ended the data doesn't pass. I didn't use any
 * additional pull up resistors. 
 * 
 * PROBLEMS
 * 1. While loop not working. Have to check for mismatch. Will do it later. Add are you sure, with
 *    a button functionality. 
 * 2. 
 */

#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
uint8_t getFingerprintEnroll(int id);

SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);


SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}


void fingerprint_mycheck()
{
  //check fingerprint
  Wire.begin();
  Serial.println("I2C");

  //getting loop code in here
  Serial.print("Enrolling ID #");
  Wire.requestFrom(3,1);
  while(Wire.available())
  {
    char flag = Wire.read();
    //Serial.println(flag);
      if (flag == 'f')       //request from the OLED to access fingerprint
        {
            getFingerprintEnroll();   //removed the while loop
              fingerprintcheck();
        }
      else
        {
            Serial.println("No access") ;
        }  
    }
}

void bluetooth_mycheck()
{
  //check bluetooth
  
Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  /* Perform a factory reset to make sure everything is in a known state */
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

/****************************************************************************/

  //Bluetooth setup
  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }
  Serial.println(F("*****************"));

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("*****************"));
}

void setup() {
  Serial.begin(9600);
  Serial.println("fingertest");
  
  //Setting up the GPIO's
  pinMode(4,OUTPUT);    //Triggers wifi
  pinMode(A0,INPUT);   //Triggers fingerprint
  pinMode(13,OUTPUT);
  
  // set the data rate for the sensor serial port
    finger.begin(57600);  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  Serial.println(A0);
  static int fix = 1;

  while(fix)
  {
    volatile int check = digitalRead(A0);
    if(check == fix)
    {
      // fingerprint and bluetooth
      fingerprint_mycheck();
      bluetooth_mycheck();
      //wait to enter wifi name and password........
      fix = 0;
    }
  }

  //Get the signal from OLED activating bluetooth
  
  Serial.println("#####----> Broke the loop!");
    //access fingerprint

/****************************************************************************/

}


void loop() {
  //Bluetooth info
  // Check for user input
  char n, inputs[BUFSIZE+1];

  if (Serial.available())
  {
    n = Serial.readBytes(inputs, BUFSIZE);
    inputs[n] = 0;
    // Send characters to Bluefruit
    Serial.print("Sending: ");
    Serial.print(inputs);

    // Send input data to host via Bluefruit
    ble.print(inputs);
  }

  // Echo received data*/
  // char wifi_name[33];
//   int s=0;
  while ( ble.available() )
  {
    char c = ble.read();
    //Serial.print(c);    //$ is the passkey
    if(c == '$')
    {
      Serial.println("Bluetooth unlocked");
      //Send a request over I2C for the device to get unlocked
      Wire.endTransmission();
      Wire.beginTransmission(3);
      Wire.write("db");
      delay(100);
      Wire.endTransmission();
      digitalWrite(4,HIGH);   //triggers wifi
      Serial.println("Wire high - Wifi should trigger");
      //delay(200);
     // digitalWrite(4,LOW);
    }
    else if(c == '1' || c == '2' || c == '3' || c == '4' || c =='5' || c == '6' || c == '7' || c == '8' || c =='9')
    {
       digitalWrite(13,HIGH);
       int len = 1;
       Serial.println("Accepted the network");
       Wire.endTransmission();
       Wire.beginTransmission(8);
       Wire.write(len);
       delay(100);
       Wire.write(c);
       delay(100);
       Wire.endTransmission();
       digitalWrite(13,LOW);
    }
    else
    {
      digitalWrite(13,HIGH);
      char password[33];
      password[0] = c;
      int a=1;
      while(ble.available())
      {
        int c = ble.read();
        password[a] = char(c);
        a++;
      }
      password[a] = '\0';
      digitalWrite(13,LOW);
      Serial.println(password);
      Wire.endTransmission();
      Wire.beginTransmission(8);
      int len = strlen(password);
      Wire.write(len);
      delay(100);
      Serial.println(len);
     // delay(100);
      for(int q=0;q<len;q++)
      {
        char pass = password[q];
        Wire.write(pass);
        delay(100);
      }
      Wire.endTransmission();
     Serial.println("Invalid passkey");
    }
  }
  /*static int check = 1;
  while(check)
  {
    Serial.println("Sending wifi name");
   Wire.endTransmission();
   Wire.beginTransmission(8);
   int len;
   len = strlen(wifi_name);
   Wire.write(len);
   delay(100);
   Wire.write(wifi_name);
   check = 0;
  }*/
   
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.println("Waiting for valid finger to enroll");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }
//image taken till here

p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  //go to OLED to display the info on the screen about your finger
  Wire.endTransmission();
  Wire.beginTransmission(3);
  Wire.write("fd");
  delay(100);
  Wire.endTransmission();
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
}
//image converted -  remove finger
  //Device secured. Now read the image back and unlock the device.

  //This should be in a loop till the right finger is placed
  // New function
  int fingerprintcheck()
  {
   static int stat = 1;
   while(stat == 1)   //this while loop is not working....why ??
    {
     int p = -1;
     Serial.println(stat);
     //Serial.println(
     Serial.println("Place same finger again");
     while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        switch (p) {
          case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
          case FINGERPRINT_NOFINGER:
            Serial.print(".");
            break;
          case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
          case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
          default:
            Serial.println("Unknown error");
            break;
            }
          }

        p = finger.image2Tz(2);
        switch (p) {
          case FINGERPRINT_OK:
            Serial.println("Image converted");
            break;
          case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
          case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
          case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
          case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
          default:
            Serial.println("Unknown error");
            return p;
            }
        // OK converted!
        p = finger.createModel();
        if (p == FINGERPRINT_OK) {
          Serial.println("Prints matched!");
          stat = 0;
          Serial.println(stat);
          //send this data over the wire
          Wire.endTransmission();
          Wire.beginTransmission(3);
          Wire.write("pm");
          delay(100);
          Wire.endTransmission();
   
          } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
            Serial.println("Communication error");
            return p;
          } else if (p == FINGERPRINT_ENROLLMISMATCH) {
            Serial.println("Fingerprints did not match");
            stat = 1;
            Serial.println(stat);
          //send this data over the wire
            //return p;
          } else {
            Serial.println("Unknown error");
            return p;
            }
      }
  
}
  
  

