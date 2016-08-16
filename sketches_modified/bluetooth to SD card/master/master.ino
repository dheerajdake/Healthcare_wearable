/*The SD card is connected to the master for logging data from
 * the bluetooth through UART - SPI - I2C interface
 * 
 * This is the MASTER device - SD card mounted
 */

//required headers
#include <SoftwareSerial.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
//#include "BluefruitConfig1.h" - This has a static buffer

//Software Serial
SoftwareSerial bluefruitSS = SoftwareSerial(5, 4);

Adafruit_BluefruitLE_UART ble(bluefruitSS, 7,
                      6, BLUEFRUIT_UART_RTS_PIN);

#include <SPI.h>
#include <SD.h>
#include<Wire.h>
File myFile;

const int chipSelect = 10;

void setup() {
  Wire.begin();   
 // Open serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  myFile = SD.open("info_blu.txt", FILE_WRITE);
  Serial.println("Slave file created\n");

}

void files()
{
  if (myFile) {
    Serial.print("Writing from slave device to the master log\n");
  }
  
}

void loop() {
 // files();
  //Serial.println("*****Bluetooth info*******\n");;
  //I2C bus
  //delay(100);
  Wire.requestFrom(3,5);  //request data from the slave

  //poll for the data
  while(Wire.available())
  {
    
      Serial.println("Fetching data from the i2c bus\n");
         char c = Wire.read();
         Serial.println(c);   
         delay(100);
      //Serial.println("Done dana done\n");
    
  }
  
    /* Serial.println("Done/n");
      myFile.close();
      myFile = SD.open("blufile.txt");*/
}
