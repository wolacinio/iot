/**
 IBM IoT Foundation managed Device

 Author: Ant Elder
 License: Apache License v2
*/
#include <ESP8266WiFi.h>
#include<SoftwareSerial.h>
#include <PubSubClient.h>  https:github.com/knolleary/pubsubclient/releases/tag/v2.3
#include <ArduinoJson.h>  https:github.com/bblanchon/ArduinoJson/releases/tag/v5.0.7

SoftwareSerial softSerial(D2, D3);  //RX, TX

const char* ssid = "";
const char* password = "";

#define ORG ""
#define DEVICE_TYPE ""
#define DEVICE_ID ""
#define TOKEN ""

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

const char publishTopic[] = "iot-2/evt/status/fmt/json";
const char manageTopic[] = "iotdevice-1/mgmt/manage";
const char updateTopic[] = "iotdm-1/device/update";

void callback(char* topic, byte* payload, unsigned int payloadLength);

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

int publishInterval = 30000;  //30 seconds
long lastPublishMillis;

void setup() {
 softSerial.begin(9600);
 Serial.begin(115200); Serial.println();

 wifiConnect();
 mqttConnect();
 initManagedDevice();
}

void loop() {
 if (millis() - lastPublishMillis > publishInterval) {
   if(softSerial.available() > 0) {
     Serial.println("SoftSerial available");
     float temp = softSerial.parseFloat();
     if (softSerial.read() == '\n') {
       Serial.print("Temp: ");Serial.println(temp);
       //publishData(temp);
       lastPublishMillis = millis();
     } 
   }
 }

 if (!client.loop()) {
   mqttConnect();
   initManagedDevice();
 }
}

void wifiConnect() {
 Serial.print("Connecting to "); Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }
 Serial.print("\nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
 if (!!!client.connected()) {
   Serial.print("Reconnecting MQTT client to "); Serial.println(server);
   while (!!!client.connect(clientId, authMethod, token)) {
     Serial.print(".");
     delay(500);
   }
   Serial.println();
 }
}

void initManagedDevice() {
 if (client.subscribe("iotdm-1/device/update")) {
   Serial.println("subscribe to update OK");
 } else {
   Serial.println("subscribe to update FAILED");
 }

 StaticJsonBuffer<300> jsonBuffer;
 JsonObject& root = jsonBuffer.createObject();
 JsonObject& d = root.createNestedObject("d");
 JsonObject& metadata = d.createNestedObject("metadata");
 metadata["publishInterval"] = publishInterval;
 JsonObject& supports = d.createNestedObject("supports");
 supports["deviceActions"] = true;

 char buff[300];
 root.printTo(buff, sizeof(buff));
 Serial.println("publishing device metadata:"); Serial.println(buff);
 if (client.publish(manageTopic, buff)) {
   Serial.println("device Publish ok");
 } else {
   Serial.print("device Publish failed:");
 }
}

void publishData(float temp) {
 String payload = "{\"d\":{\"temp\":";
 payload += temp;
 payload += "}}";

 Serial.print("Sending payload: "); Serial.println(payload);

 if (client.publish(publishTopic, (char*) payload.c_str())) {
   Serial.println("Publish OK");
 } else {
   Serial.println("Publish FAILED");
 }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
 Serial.print("callback invoked for topic: "); Serial.println(topic);

 if (strcmp (updateTopic, topic) == 0) {
   handleUpdate(payload);
 }
}

void handleUpdate(byte* payload) {
 StaticJsonBuffer<300> jsonBuffer;
 JsonObject& root = jsonBuffer.parseObject((char*)payload);
 if (!root.success()) {
   Serial.println("handleUpdate: payload parse FAILED");
   return;
 }
 Serial.println("handleUpdate payload:"); root.prettyPrintTo(Serial); Serial.println();

 JsonObject& d = root["d"];
 JsonArray& fields = d["fields"];
 for (JsonArray::iterator it = fields.begin(); it != fields.end(); ++it) {
   JsonObject& field = *it;
   const char* fieldName = field["field"];
   if (strcmp (fieldName, "metadata") == 0) {
     JsonObject& fieldValue = field["value"];
     if (fieldValue.containsKey("publishInterval")) {
       publishInterval = fieldValue["publishInterval"];
       softSerial.print(publishInterval);
       softSerial.print("\n");
       Serial.print("publishInterval:"); Serial.println(publishInterval);
     }
   }
 }
}
