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
  if (hasIntervalElapsed()) sendSensorData();
  checkIntervalValueUpdate();
}

boolean hasIntervalElapsed() {
  return millis() - lastPublishMillis > publishInterval;
}

void sendSensorData() {
  Serial.print("Temp: ");
  Serial.println(temp);
  softSerial.print(temp);
  softSerial.print("\n");
  temp += 1.5;
  lastPublishMillis = millis();
}

void checkIntervalValueUpdate() {
  if(softSerial.available() > 0) {
    int interval = softSerial.parseInt();
    if (softSerial.read() == '\n') {
      if (interval >= 1000) {
        publishInterval = interval;
        Serial.print("Interval set to: ");
        Serial.println(publishInterval);
      } else {
        Serial.print("Interval value to low: ");
        Serial.println(interval);
        Serial.print("Current interval value: ");
        Serial.println(publishInterval);
      }
    }
  }
}

