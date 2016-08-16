/*The SD card is connected to the master for logging data from
 * the bluetooth through UART - SPI - I2C interface
 * 
 * This is the SLAVE device - Bluetooth connected 
 * Bluetooth code - Done
 * I2C code - in progress
 */

//header files - we use software serial
 #include <Arduino.h>
 #include <Wire.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
//#include "BluefruitConfig1.h" - This has a static buffer called data in addition

//placing a buffer in BluefruitConfig.h and using it from there

//Software Serial
SoftwareSerial bluefruitSS = SoftwareSerial(5, 4);

Adafruit_BluefruitLE_UART ble(bluefruitSS, 7,
                      6, 9);

//Debugging stuff
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void setup() {
  //Setting up I2C bus
  Wire.begin(3);                //assiging the slave address
  Wire.onRequest(requestEvent); //register an event
  //Setting up the bluetooth
  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  //I skipped the factory reset since i did it already
  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

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

/*After this it polls for the data from the app
 * 
 */

//for the character
//char data[5]; - UNCOMMENT FOR BUFFER
//char len=0;   - UNCOMMENT FOR BUFFER
char c= 0;
void loop() {
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

  // Echo received data
 /* while ( ble.available() )
  {
   sprintf(data,"%c" ,ble.read());
   len = strlen(data);
   for(int a=0; a<len;a++)
   {
     Serial.print((char)data[a]);
   }*/
   /***********************************************WITHOUT BUFFER******************************/
    while(ble.available())
    {
     c = ble.read();
     Serial.print(c);
     
   }
   /***********************************************WITHOUT BUFFER******************************/
}

//  
    //c = ble.read();
   // Wire.write(c);
    //Serial.print((char)c);

    // Hex output too, helps w/debugging!
    //Serial.print(" [0x");
    //if (c <= 0xF) Serial.print(F("0"));
    //Serial.print(c, HEX);
    //Serial.print("] ");
//int req = 0;

void requestEvent()
{
  //c = ble.read();
  Wire.write("hello");
}

