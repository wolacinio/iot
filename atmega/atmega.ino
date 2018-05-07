#include <UIPEthernet.h>
#include <PubSubClient.h>
#include <dht11.h>
#define DHTPIN 9

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
long lastMessage = 0; 
long lastReconnect = 0;
long now;

dht11 DHT11;
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
IPAddress ip(192, 168, 1, 15);

void callback(char* topic, byte* payload, unsigned int length){}

bool reconnect() {
  while(!mqttClient.connect("*", "*", "*")) {
    delay(100);
  }
  return mqttClient.connected();
}

void setup() {
  Ethernet.begin(mac, ip);
  mqttClient.setServer("*", 1883);
  mqttClient.setCallback(callback);
}
 
void loop() {
  DHT11.read(DHTPIN);
  now = millis();
  if (!mqttClient.connected()) {
    if (now - lastReconnect > 5000) {
      lastReconnect = now;
      if (reconnect()) {
        lastReconnect = 0;
      }
    }
  } else {
    if (now - lastMessage > 5000) {
      lastMessage = now;
      char tab[40];
      sprintf(tab, "{\"temperatura\":%d,\"wilgotnosc\":%d}", (int)DHT11.temperature, (int)DHT11.humidity);
      mqttClient.publish("iot-2/evt/status/fmt/json", tab);
    }
    mqttClient.loop();
  }
}
