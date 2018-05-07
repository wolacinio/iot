#include<BME280I2C.h>
#include<SoftwareSerial.h>
#include<Wire.h>
#include<SPI.h>
#include<ArduinoJson.h>

SoftwareSerial softSerial(10, 11); // RX, TX
BME280I2C bme;

int publishInterval = 30000;  //30 seconds
int minPublishInterval = 5000;  //5 seconds
long lastPublishMillis;

void setup() {
  softSerial.begin(9600);
  Serial.begin(115200);
  Wire.begin();
  initSensor();
}

void initSensor() {
  while(!bme.begin()) {
    Serial.println("Could not find BME280 sensor!");
    delay(2000);
  }

  switch(bme.chipModel()) {
    case BME280::ChipModel_BME280:
      Serial.println("Found BME280 sensor!");
      break;
    case BME280::ChipModel_BMP280:
      Serial.println("Found BMP280 sensor!");
      break;
    default:
      Serial.println("Found UNKNOWN sensor! Error!");
  }
}

void loop() {
  if (hasIntervalElapsed()) readSensorData();
  checkIntervalValueUpdate();
}

boolean hasIntervalElapsed() {
  return millis() - lastPublishMillis > publishInterval;
}

void readSensorData() {
  float temp(NAN), hum(NAN), pres(NAN);
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);
  bme.read(pres, temp, hum, tempUnit, presUnit);
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" \u00B0C");
  Serial.print("\t\tPressure: ");
  Serial.print(pres);
  Serial.println(" hPa");
  sendSensorData(temp, pres);
}

void sendSensorData(float temp, float pres) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temp"] = temp;
  root["pres"] = pres;
  root.printTo(softSerial);
  lastPublishMillis = millis();
}

void checkIntervalValueUpdate() {
  if(softSerial.available() > 0) {
    int interval = softSerial.parseInt();
    if (softSerial.read() == '\n') {
      if (interval >= minPublishInterval) {
        publishInterval = interval;
        Serial.print("Interval set to: ");
        Serial.println(publishInterval);
      } else {
        Serial.print("Interval value to low: ");
        Serial.println(interval);
        Serial.print("Minimum interval value: ");
        Serial.println(minPublishInterval);
        Serial.print("Current interval value: ");
        Serial.println(publishInterval);
      }
    }
  }
}
