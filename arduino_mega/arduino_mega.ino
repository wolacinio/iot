#include <SoftwareSerial.h>

SoftwareSerial softSerial(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  softSerial.begin(115200);
}

void loop() {
  while(softSerial.available() > 0) {
    long value = softSerial.parseInt();
    if (softSerial.read() == '\n') {
      Serial.println(value);
    } 
  }
  delay(2000);
}
