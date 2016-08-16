/******************************************S*L*A*V*E*14**************************/
#include<Wire.h>
void setup() {
  Wire.begin(3);                //join i2c bus
  Wire.onRequest(requestEvent); //register an event

}

void loop() {
  delay(100);
}

void requestEvent()
{
  //check here
  Wire.write("Slave 2 responding - COM 14\n");
}

