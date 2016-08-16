/* Display the data from the master on the LCD
 * Slave - OLED - S15 - Reading the information on the SD card
 */

//Initialize the OLED
#define sclk 13
#define mosi 11
#define dc   8
#define cs   5
#define rst  6

#define  BLACK           0x0000
#define WHITE           0xFFFF

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <Wire.h>

Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);  

float p = 3.1415926;
//char dat[100];    //buffer for OLED
int ss = 0;

void fillpixelbypixel(uint16_t color) {
  for (uint8_t x=0; x < tft.width(); x++) {
    for (uint8_t y=0; y < tft.height(); y++) {
      tft.drawPixel(x, y, color);
    }
  }
  delay(100);
}

 
void setup() {
  Serial.begin(9600);
  Serial.print("Hello");
  tft.begin();
  Serial.println("init");

  tft.fillScreen(BLACK);
  //testdrawtext("The data goes here", WHITE);
  Wire.begin(3);
  Wire.onReceive(receiveEvent);
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0,0);
  tft.setTextColor(color);
  tft.print(text);
}

void loop() {
  //receiveEvent(ss);
}

void receiveEvent(int data)
{ 
  ss = Wire.read();
  char *dat = malloc(ss);     //added this
  Serial.print(ss);
  while(1<Wire.available())
  {
    for(int a=0;a<ss;a++)
    {
      dat[a] = Wire.read();    //try this with &dat for single stream...
    }
    Serial.print(dat);
    testdrawtext(dat,WHITE);
    free(dat);                  //free data.
  }

    //testdrawtext("What is this?\n",WHITE);*/
  
}


