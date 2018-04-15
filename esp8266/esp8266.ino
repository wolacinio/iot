#include <SoftwareSerial.h>

SoftwareSerial softSerial(D0, D1); // RX, TX

void setup() {
  softSerial.begin(115200);
}

void loop() {
  long val = millis();
  softSerial.print(val);
  softSerial.print("\n");
  delay(2000);
}
