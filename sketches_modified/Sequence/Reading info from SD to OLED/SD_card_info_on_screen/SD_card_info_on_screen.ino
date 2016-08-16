/* Fetch the data from the SD card and display it on the OLED
 * Master - SD CARD - M7 - Sending information to the slave
 */

//Initialize the SD CARD
#include <SPI.h>
#include <SD.h>
#include<Wire.h>
File myFile;
int i=0;
const int chipSelect = 10;
char data[100];

void setup() {
  Wire.begin();  
  Serial.begin(9600);
  pinMode(SS, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  myFile = SD.open("note1.txt", FILE_WRITE);

  //writing data into the file
  myFile.print("This is working");
  //myFile.print("Line2.....22");
  myFile.close();
  myFile = SD.open("note1.txt");

  if (myFile) {
    Serial.print("Writing from slave device to the master log\n");
    while(myFile.available() > 0)
      {
        data[i] = myFile.read();
        i++;
        data[i]=0;
      } 
  }
  Wire.beginTransmission(3);
  int ss = strlen(data);
  Serial.print(ss);
  Wire.write(ss);
  delay(100);
  Wire.write(data);  //number of bytes to read
  Wire.endTransmission();
}


void loop() {
 /*I2C*/
  
}
