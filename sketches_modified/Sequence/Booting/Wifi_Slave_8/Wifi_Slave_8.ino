/* MODULES
 * 1. WiFI - Wifi on slave 8. After the welcome screen the wifi networks are displayed on the screen.
 *           The request from the OLED goes to the Wifi module for listing the available wifi networks.
 *       
 */ 

/* Dheeraj Dake - 10/3/15
 * The user can choose  a wifi network. The wifi number and the password can be entered by the user to log into the network.
 * Currently applicable to 6 wifi networks. Have to write more test cases for incorrect combination. Have to update the same on the 
 * OLED. 
 */

#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include <Wire.h>
#include "utility/debug.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   3  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIVIDER); // you can change this clock speed but DI

//Make this over bluetooth
//#define WLAN_SSID       "myNetwork"        // cannot be longer than 32 characters!
#define WLAN_PASS       "c14d3233"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

/*
 * Setting up the wifi module
 */

void setup() {
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n")); 
  pinMode(8,INPUT);   //to signal the wifi
  
  pinMode(A0, OUTPUT); //signal the OLED that wifi is connected
  
  displayDriverMode();
  Serial.print("Free RAM: "); Serial.println(getFreeRam(), DEC);
  
  /* Initialise the module */
  Serial.println(F("\nInitialising the CC3000 ..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Unable to initialise the CC3000! Check your wiring?"));
    while(1);
  }
  
  uint16_t firmware = checkFirmwareVersion();
  if (firmware < 0x113) {
    Serial.println(F("Wrong firmware version!"));
    for(;;);
  } 
  Serial.println("i2c initializing......");
  Wire.begin(2);         //i2c bus 2 from OLED
  static int check = 1;
  while(check)
  {
  if(digitalRead(8))
  {
  Wire.requestFrom(2,1); //get the passkey from master - OLED
  Serial.println("i2c initialized, checking for the bit");
  //static int wifi_pass = 1, fix = 1;
  //while(wifi_pass == fix)
  //{
      while(Wire.available())       //waits for the OLED to send message to display the wifi networks
        {
          Serial.println("In while, ......");
          char flag = Wire.read();
        
          Serial.println(flag);
          if(flag = 'w')
            {
              displayMACAddress();
                #ifndef CC3000_TINY_DRIVER
                  listSSIDResults();     //displays the wifi names
                #endif
                  Serial.println("Wifi scanning done");
                  Wire.endTransmission();   //ends the current transmission
            }
          else
            {
                Serial.println("Flag not received");
              Serial.println(flag);
            }
        }
        check = 0;
  }
  }
  Serial.println("----------EXITED THE WIFI CHECK---------");
  //digitalWrite(A0,HIGH);   //Choose wifi network
  //delay(100);
  //digitalWrite(A0,LOW);   //Turn off signalling
/*From bluetooth - slave 14*/

Wire.begin(8);
//Wire.onRequest(requestEvent_bluetooth);
Wire.onReceive(receiveEvent_bluetooth);
/**************************************************************/
} //end of setup


char wifi_1[33], wifi_2[33], wifi_3[33], wifi_4[33], wifi_5[33], wifi_6[33], wifi_7[33], wifi_8[33], wifi_9[33];
static int change = 0;     //global
char msg[33];   //for password - this refreshes everytime

void choose_wifi()
{
    /**************WHICH NETWORK ?******************/
              /* Max 32 chars */
    char *ssid;
    Serial.println(change);
    switch(change)     //Currently supporting 6 wifi networks
    {
      case 1:
                ssid = wifi_1;
                Serial.print("Selected.............");    Serial.print(ssid);
                break;
      case 2:
                ssid = wifi_2;
                Serial.print("Selected.............");    Serial.print(ssid);
                break;
      case 3:
                ssid = wifi_3;
                Serial.print("Selected.............");    Serial.print(ssid);
                break;
      case 4:
                ssid = wifi_4;
                Serial.print("Selected.............");    Serial.print(ssid);
                break;
      case 5:
                ssid = wifi_5;
                Serial.print("Selected.............");    Serial.print(ssid);
                break;
      case 6:
                ssid = wifi_6;
                Serial.print("Selected.............");    Serial.print(ssid);
                break;
    }
     
  Serial.print(F("\nAttempting to connect to ")); Serial.println(ssid);
  Serial.print("Password is "); Serial.print(msg);
  if (!cc3000.connectToAP(ssid, msg, WLAN_SECURITY)) {        //changed this.....
    Serial.println(F("Failed!"));
    while(1);
  } 
  
  Serial.println(F("Connected!")); 
  digitalWrite(A0,HIGH);
  delay(200);
  digitalWrite(A0,LOW);
}

static int access = 0;      //check this always

//start of loop
void loop() {
     while(access == 2) {
      choose_wifi();
      access = 0;
     }      
}


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
//static int change = 0;     //global
//static int access = 0;      //global
//char msg[33];   //for password - this refreshes everytime
void receiveEvent_bluetooth(int data)
{
  Serial.println("Incoming data from bluetooth...");
  while(1<Wire.available())
  {
    Serial.println("Wire available.....");
    int len = Wire.read();
    int a = 0;
    for(a;a<len;a++)
    {
      msg[a] = Wire.read();
    }
    msg[a] = '\0';
    Serial.print("I selected ");
    static int flag = 0;
    if(flag == 0)
    {
      change = msg[0];
      change = change - 48;  
      flag++;
      //Refresh the screen and update the selected wifi network on the OLED
      
    }
    Serial.print(change);
   // Serial.print(msg);
    access++;                    //after entering password it becomes 2
  }
 
}


/*****************************MODULES*******************************/


/**************************************************************************/
/*!
    @brief  Displays the driver mode (tiny of normal), and the buffer
            size if tiny mode is not being used

    @note   The buffer size and driver mode are defined in cc3000_common.h
*/
/**************************************************************************/
void displayDriverMode(void)
{
  #ifdef CC3000_TINY_DRIVER
    Serial.println(F("CC3000 is configure in 'Tiny' mode"));
  #else
    Serial.print(F("RX Buffer : "));
    Serial.print(CC3000_RX_BUFFER_SIZE);
    Serial.println(F(" bytes"));
    Serial.print(F("TX Buffer : "));
    Serial.print(CC3000_TX_BUFFER_SIZE);
    Serial.println(F(" bytes"));
  #endif
  
}

/**************************************************************************/
/*!
    @brief  Tries to read the CC3000's internal firmware patch ID
*/
/**************************************************************************/
uint16_t checkFirmwareVersion(void)
{
  uint8_t major, minor;
  uint16_t version;
  
#ifndef CC3000_TINY_DRIVER  
  if(!cc3000.getFirmwareVersion(&major, &minor))
  {
    Serial.println(F("Unable to retrieve the firmware version!\r\n"));
    version = 0;
  }
  else
  {
    Serial.print(F("Firmware V. : "));
    Serial.print(major); Serial.print(F(".")); Serial.println(minor);
    version = major; version <<= 8; version |= minor;
  }
#endif
  return version;
}

/**************************************************************************/
/*!
    @brief  Tries to read the 6-byte MAC address of the CC3000 module
*/
/**************************************************************************/
void displayMACAddress(void)
{
  uint8_t macAddress[6];
  
  if(!cc3000.getMacAddress(macAddress))
  {
    Serial.println(F("Unable to retrieve MAC Address!\r\n"));
  }
  else
  {
    Serial.print(F("MAC Address : "));
    cc3000.printHex((byte*)&macAddress, 6);
  }
}


/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}

/**************************************************************************/
/*!
    @brief  Begins an SSID scan and prints out all the visible networks
*/
/**************************************************************************/
char num_ssid[33]; //globals
//char wifi_1[33], wifi_2[33], wifi_3[33], wifi_4[33], wifi_5[33], wifi_6[33], wifi_7[33], wifi_8[33], wifi_9[33];
void listSSIDResults(void)
{
  
  uint32_t index;
  uint8_t valid, rssi, sec;
  char ssidname[33]; 

  if (!cc3000.startSSIDscan(&index)) {
    Serial.println(F("SSID scan failed!"));
    return;
  }

  Serial.print(F("Networks found: ")); Serial.println(index);
  Serial.println(F("================================================"));

  while (index) {
    index--;
    valid = cc3000.getNextSSID(&rssi, &sec, ssidname);
    static int count = 0;
    num_ssid[0] = ++count + 48;
    num_ssid[1] = '.';
    num_ssid[2] = ' ';
    int a;
    for(a=3; a<33;a++)
    {
      num_ssid[a] = ssidname[a-3];
    }
    num_ssid[a] = '\0';
    /****Stores all the wifi networks****/
    static int change = 1;    //for writing into different buffer
    switch(change)     //Currently supporting 6 wifi networks
    {
      case 1:
            for(int r=0; r<strlen(ssidname); r++)
                wifi_1[r] = ssidname[r];
                Serial.println(wifi_1);
                change++;
                break;
      case 2:
            for(int r=0; r<strlen(ssidname); r++)
                wifi_2[r] = ssidname[r];
                Serial.println(wifi_2);
                change++;
                break;
      case 3:
            for(int r=0; r<strlen(ssidname); r++)
                wifi_3[r] = ssidname[r];
                Serial.println(wifi_3);
                change++;
                break;
      case 4:
            for(int r=0; r<strlen(ssidname); r++)
                wifi_4[r] = ssidname[r];
                Serial.println(wifi_4);
                change++;
                break;
      case 5:
            for(int r=0; r<strlen(ssidname); r++)
                wifi_5[r] = ssidname[r];
                Serial.println(wifi_5);
                change++;
                break;
      case 6:
            for(int r=0; r<strlen(ssidname); r++)
                wifi_6[r] = ssidname[r];
                Serial.println(wifi_6);
                change++;
                break;
    }
    
    /*^^^^^^^^^^^^^^^^****Choosing networks****^^^^^^^^^^^^^^^^^^^^^*/
    Serial.print(F("SSID Name    : ")); Serial.print(num_ssid);
    Serial.println();
    Serial.print(F("RSSI         : "));
    Serial.println(rssi);
    Serial.print(F("Security Mode: "));
    Serial.println(sec);
    Serial.println();

  //writing the wifi names to OLED
    Wire.endTransmission();
    Wire.beginTransmission(2);
    
    int len = strlen(num_ssid);    //how many bytes to read
    //Serial.println(len);
    //if(ssid 
    if(len != NULL)
    {
      //if(*ssidname == '\0')
      //{
      Wire.write(len);
      delay(200);
      Wire.write(num_ssid);
      delay(200);  
      //}
    }   
  }
  Serial.println(F("================================================"));

  cc3000.stopSSIDscan();
  Serial.println(index);
}





