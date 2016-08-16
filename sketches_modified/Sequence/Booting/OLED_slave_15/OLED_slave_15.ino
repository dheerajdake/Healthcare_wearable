/* This shows th sequence on the OLED when the system boots up
 * Display after the device is powered on
 */

 //Initialize the OLED - it's the Master for now. 

 /*   Dheeraj Dake - 8/30/15  
  *   All about functions
  *   The UNO SL 15 is acting as a master. It is fetching the information from all the slaves.
  *   The I2C bus acts as a medium between the different boards to sync information. Since the
  *   I2C is not a fully duplex bus(can't communicate bidirectionally at the same time), the
  *   bus needs to be closed and reopened every single time the data is sent. I have removed 
  *   the code from the void loop() since it keeps the bus active all the time. 
  *   
  *   FUNCTIONS---
  *   Wire.onRequest() - This is used by the OLED to send flags to all other devices which 
  *                      are necessary for initialization. The device sees its appropriate flag
  *                      and turns on.
  *   Wire.onReceive() - This is used by the OLEd to fetch the flags from the devices. The 
  *                      important information needed to be displayed on the screen are sent 
  *                      over the bus and stored in a char buff. The data from that buff is
  *                      displayed on the OLED. 
  */


//OLED pins
#define sclk 13
#define mosi 11
#define dc   8
#define cs   5
#define rst  6

//GSM pins
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

// Color definitions
#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <avr/interrupt.h>
#include "Adafruit_FONA.h"
#include <SPI.h>
#include <Wire.h>

Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);  

float p = 3.1415926;
char dat[50];    //buffer for OLED


void fillpixelbypixel(uint16_t color) {
  for (uint8_t x=0; x < tft.width(); x++) {
    for (uint8_t y=0; y < tft.height(); y++) {
      tft.drawPixel(x, y, color);
    }
  }
  delay(100);
}

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

int buzz = 8;
volatile int state = LOW;
/**************G S M END**************/


void setup() {
  Serial.begin(9600);
  Serial.println("Hello");
  tft.begin();
//  Serial.println("init");

  tft.fillScreen(BLACK);
  testdrawtext("Machine booting up..", YELLOW);
  tft.setCursor(0,10);
  tft.setTextColor(YELLOW);  
  tft.println("Initializing devices..");
  tft.setCursor(0,20);
  tft.setTextColor(BLUE); 
  tft.println("Register fingerprint");
 // Wire.begin(3);
  //Wire.onReceive(receiveEvent);

  //GPIO signalling
  
  pinMode(10,INPUT);  //triggers wifi
  pinMode(9, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(12,INPUT); //signal from wifi that it has been connected

  Serial.println("Fingerprint accessed");
  digitalWrite(9,HIGH);
  //Finger print code here
  Wire.begin(3);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

 

  pinMode(A0, OUTPUT);   //For the oh shit switch - red
  pinMode(A1, INPUT);    //For the signalling
  pinMode (A2, OUTPUT);  //buzzer -beeps, the black leg of buzzer goes to ground
  pinMode(3,INPUT);   //from bluetooth
  //Serial.println("End of setup");

}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0,0);
  tft.setTextColor(color);
  tft.print(text);
}
static int wifi = 1;  //for wifi
static int gps = 1;  //for gps
static int blu = 1;  //from bluetooth for updating the screen
static int conn = 1; //wifi connected from slave 8 

void loop() {

  volatile int wificheck = digitalRead(10);
  if(wificheck == wifi)
  {
    //trigger the wifi
    Serial.println("WIFI triggered");
    digitalWrite(7,HIGH);
    Serial.println(digitalRead(7));
    Wire.begin(2);
    Serial.println("w request wire begin");
    Wire.onRequest(requestEvent_wifi);
    Wire.write("w");
    Wire.onReceive(receiveEvent_wifi);
    digitalWrite(7,LOW);
    delay(5000);
    Wire.endTransmission();
   Serial.println();
    wifi = 0;
    return;
  }

  //update the screen
  volatile int upd = digitalRead(3);
  static int ssss=1;
  while(upd)
  {
    if(ssss==1)
    {
      tft.fillScreen(BLACK);
      testdrawtext("Enter password", YELLOW);
      ssss++;
      return;
    }
    else if(ssss == 2)
    {
      tft.setCursor(0,30);
      tft.println("waiting......");
      ssss++;
      return;
      //testdrawtext("Waiting//////", YELLOW);
    }
  }

  //wifi connected
  volatile int wcon = digitalRead(12);
  //Serial.print(wcon);
  static int bbbb=1;
  if(wcon == bbbb)
  {
    Serial.print("....");
    tft.setCursor(0,60);
    tft.println("Connected");
    bbbb++;
    return;
  }
}

void requestEvent()  //sending info from OLED
{
  Serial.println("sending f");
  Wire.write("f");  //access to fingerprint
  Serial.println("sent f");
}

void requestEvent_wifi()
{
  Serial.println("sending w");
  Wire.write("w");
  Serial.println("sent w");
}

void receiveEvent (int data) //sending info to OLED
{
  Serial.println("Data received");
   while(1<Wire.available())
   {
      for(int a=0; a<2;a++)
      {
        dat[a] = Wire.read();
      }
      //Serial.print(dat);
      if(dat[1] == 'd')
      {
        Serial.println("Finger check 1");
        tft.setCursor(0,30);
        tft.println("Remove your finger"); 
        tft.setCursor(0,40);
        tft.setTextColor(GREEN);  
        tft.println("Print Registered!"); 
        tft.setCursor(0,50);
        tft.setTextColor(BLUE);  
        tft.println("Unlock using finger");
      }
      else if(dat[1] == 'm')
      {
        Serial.println("Unlocked!");
        tft.setCursor(0,70);
        tft.setTextColor(GREEN); 
        tft.println("Fingerprint Success!"); 
        tft.setCursor(0,80);
        tft.setTextColor(BLUE);
        tft.println("Pair with blutooth..."); 
        delay(200);
        tft.setCursor(0,90);
        tft.println("Enter passkey"); 
      }
      else if(dat[1] == 'b')
      {
        Serial.println("bluetooth unlock");
        tft.setCursor(0,110);
        tft.setTextColor(GREEN);
        tft.println("Bluetooth unlocked"); 
        tft.setTextColor(YELLOW);
        tft.println("Loading info...");
        tft.fillScreen(WHITE);
        tft.setTextSize(2.5);
        tft.setTextColor(BLACK);
        tft.setCursor(25,50);
        tft.println("Welcome");
        delay(3000);
        tft.fillScreen(WHITE);
        tft.setCursor(0,0);
        tft.setTextSize(1);
        tft.println("Pick a Wifi network.Searching...");
        
        tft.setCursor(0,20);
        break;
        //list all the available wifi networks        
      }
      else
      {
        Serial.println("False");
      }
   }

   
}

void receiveEvent_wifi(int data)
{
  char buff[33];
  //char wifi[100];
 // delay(15000);
  Serial.println("->Data received from wifi");
  while(1<Wire.available())
  {
    Serial.println("Getting wifi's......");
    int len = Wire.read();
    int a = 0;        //changing this to 0 crashes it
    //int count = 0;       //for serials
    for(a; a<len; a++)
    {
      buff[a] = Wire.read();
    }
    buff[a] = '\0';
    for(int a =0;a<strlen(buff);a++)
      Serial.print(buff[a]);
    tft.println(buff);  

  }
}


