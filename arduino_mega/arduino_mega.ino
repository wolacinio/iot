#include<SoftwareSerial.h>

SoftwareSerial softSerial(10, 11); // RX, TX
float temp = 5.0;

int publishInterval = 30000;  //30 seconds
long lastPublishMillis;

void setup() {
  softSerial.begin(9600);
  Serial.begin(115200);
}

void loop() {
  if (millis() - lastPublishMillis > publishInterval) {
    Serial.print("Temp: ");
    Serial.println(temp);
    softSerial.print(temp);
    softSerial.print("\n");
    temp += 1.5;
    lastPublishMillis = millis();
  }

  if(softSerial.available() > 0) {
    int interval = softSerial.parseInt();
    if (softSerial.read() == '\n') {
      Serial.print("New interval: ");
      Serial.println(interval);
      publishInterval = interval;
    }
  }
}
