#include<SoftwareSerial.h>
#include<OneWire.h>
#include<DallasTemperature.h>

#define ONE_WIRE_BUS 12
#define TEMPERATURE_PRECISION 9

SoftwareSerial softSerial(10, 11); // RX, TX
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int numberOfDevices;
DeviceAddress tempDeviceAddress;
int publishInterval = 30000;  //30 seconds
int minPublishInterval = 5000;  //5 seconds
long lastPublishMillis;

void setup() {
  softSerial.begin(9600);
  Serial.begin(115200);
  initTempSensor();
}

void initTempSensor() {
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Locating temp. devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  for(int i=0;i<numberOfDevices; i++) {
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    
      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION,DEC);
    
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    
      Serial.print("Resolution actually set to: ");
      Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
      Serial.println();
      
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}

void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
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
  sensors.requestTemperatures();
  for(int i=0;i<numberOfDevices; i++) {
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Temperature for device: ");
      Serial.println(i,DEC);
      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print("Temp C: ");
      Serial.println(tempC);
      sendSensorData(tempC);
    }
  }
}

void sendSensorData(float temp) {
  softSerial.print(temp);
  softSerial.print("\n");
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
