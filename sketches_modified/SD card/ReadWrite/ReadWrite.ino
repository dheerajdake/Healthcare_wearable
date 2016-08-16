/*
  SD card read/write
 
 This example shows how to read and write data to and from an SD card file 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** UNO:  MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 4 (CS pin can be changed)
  and pin #10 (SS) must be an output
 ** Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4 (CS pin can be changed)
  and pin #52 (SS) must be an output
 ** Leonardo: Connect to hardware SPI via the ICSP header

 
 created   Nov 2010  by David A. Mellis
 modified 9 Apr 2012  by Tom Igoe
 
 This example code is in the public domain.
 	 
 */
 
#include <SPI.h>
#include <SD.h>
#include<Wire.h>
File myFile;

// change this to match your SD shield or module;
//     Arduino Ethernet shield: pin 4
//     Adafruit SD shields and modules: pin 10
//     Sparkfun SD shield: pin 8
const int chipSelect = 10;

void setup()
{
  Wire.begin();   
 // Open serial communications and wait for port to open:
  Serial.begin(9600);

  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(SS, OUTPUT);
   
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("newfile.txt", FILE_WRITE);
  Serial.println("Slave file created\n");
  // if the file opened okay, write to it:
  
/****************************I2c code***************************************/
 /*
  
      
    myFile.println("testing 1, 2, 3.");
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    	Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    
  }*/
}


void files()
{
  if (myFile) {
    Serial.print("Writing from slave device to the master log");
  }
  
}

void loop()
{
  files();
	// nothing happens after setup

  myFile.println("Writing into the slave file");
  Wire.requestFrom(3,28);
  //check here
  while(Wire.available())
  {
      Serial.println("writing files to SD/n");
      for(int i=0;i<28;i++)
      {
         char c = Wire.read();
         myFile.println(c);   
      } 
      delay(1000);
      Serial.println("Done/n");
      myFile.close();
      myFile = SD.open("newfile.txt");
  }
  delay(100);

  //Serial.println("Done writing");
}


