#include "BluefruitConfig1.h"
void setup() {
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  char len = strlen (data);
  Serial.println(len);
  delay(10000);
  Serial.println("Accessing buffer/n");
  for(int a=0; a<len;a++)
  {
    Serial.print(data[a]);
  }

}
