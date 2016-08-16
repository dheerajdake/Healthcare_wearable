/******************************************S*L*A*V*E*8**************************/
#include<Wire.h>
void setup() {
  Wire.begin(2);                //join i2c bus
  Wire.onRequest(requestEvent); //register an event
  Serial.begin(9600);
  Serial.println("Done requesting");
}

void loop() {
  delay(100);
}

void requestEvent()
{
  //Serial.print("writing to device");
  Wire.write("Slave 1 responding - COM 8\n");
}

