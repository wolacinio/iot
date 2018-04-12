#include <UIPEthernet.h>
#include <PubSubClient.h>
#include <dht11.h>
#define DHTPIN 9

byte mac[]    = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 15);
long lastMessage = 0; 
long lastReconnect = 0;
long now;

dht11 DHT11;
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
}
 

bool reconnect() {
  if(mqttClient.connect("*", "*", "*")) {
      //mqttClient.subscribe("iot-2/cmd/update/fmt/txt");
  }
  return mqttClient.connected();
}

void setup()
{
  Ethernet.begin(mac, ip);
  mqttClient.setServer("*", 1883);
  mqttClient.setCallback(callback);
}
 
void loop()
{
  DHT11.read(DHTPIN);
  if (!mqttClient.connected()) {
    now = millis();
    if (now - lastReconnect > 2000) {
      lastReconnect = now;
      if (reconnect()) {
        lastReconnect = 0;
      }
    }
  } else {
    now = millis();
    if (now - lastMessage > 1000) {
      lastMessage = now;
      char tab[10];
      sprintf(tab, "t=%d&w=%d", (int)DHT11.temperature, (int)DHT11.humidity);
      mqttClient.publish("iot-2/evt/status/fmt/txt", tab);
    }
    mqttClient.loop();
  }
}
