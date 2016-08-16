//Arduino I2C tutorial
/**********************************M*A*S*T*E*R*****************************/
#include<Wire.h>

void setup() {
  Wire.begin();         //join the i2c bus
  Serial.begin(115200);   //start serial transfer - output
}

void loop() {
  //Serial.println("In the while loop");
  Wire.requestFrom(2,27);  //request 6 bytes from slave device 2
  //Serial.println("asldjflskjfls");
  while(Wire.available())
  {
    //Serial.println("7878787");
    char c = Wire.read(); //receive the byte
    Serial.print(c);      //print the received information
  }
    delay(500);
 /*  Wire.requestFrom(3,28);
   while(Wire.available())
  {
    char c = Wire.read(); //receive the byte
    Serial.print(c);      //print the received information
  }*/
  delay(1000);
}
