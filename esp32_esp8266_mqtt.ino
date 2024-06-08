#include <DHT.h>
#include "EspMQTTClient.h"

// #include <ESP32HTTPUpdateServer.h>
// #include <EspMQTTClient.h>

#define DHTPIN 2      // DHT11 sensor pin
#define DHTTYPE DHT11 // DHT 11

#define LED1_PIN 5    // GPIO 5 (D1)
#define LED2_PIN 4    // GPIO 4 (D2)
#define LED3_PIN 14   // GPIO 14 (D5)
#define LED4_PIN 12   // GPIO 12 (D6)

DHT dht(DHTPIN, DHTTYPE);

EspMQTTClient client(
  "TFKioT",           // Wifi SSID
  "00000000",         // Wifi Password
  "test.mosquitto.org",     // MQTT Broker server IP
  "",                 // MQTT Username (can be omitted if not needed)
  "",                 // MQTT Password (can be omitted if not needed)
  "TestClient",       // Client name that uniquely identifies your device
  1883                // The MQTT port (default is 1883)
);

void setup()
{
  Serial.begin(115200);
  dht.begin();

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  digitalWrite(LED4_PIN, LOW);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater();    // Enable the web updater
  client.enableOTA();               // Enable OTA (Over The Air) updates
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // Set last will message
}

void onConnectionEstablished()
{
  // Subscribe to LED topics
  client.subscribe("myTopic/led1", [](const String & payload) {
    digitalWrite(LED1_PIN, payload == "1" ? LOW : HIGH);
  });

  client.subscribe("myTopic/led2", [](const String & payload) {
    digitalWrite(LED2_PIN, payload == "1" ? LOW : HIGH);
  });

  client.subscribe("myTopic/led3", [](const String & payload) {
    digitalWrite(LED3_PIN, payload == "1" ? LOW : HIGH);
  });

  client.subscribe("myTopic/led4", [](const String & payload) {
    digitalWrite(LED4_PIN, payload == "1" ? LOW : HIGH);
  });

  // Publish a message to confirm connection
 // client.publish("myTopic/status", "connected");
}

void loop()
{
  client.loop();

  static unsigned long lastMsg = 0;
  unsigned long now = millis();

  if (now - lastMsg > 2000) {
    lastMsg = now;
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    
    String tempMsg = "Temperature: " + String(t, 1) + " °C";
    String humidityMsg = "Humidity: " + String(h, 1) + " %";

    Serial.println("Publish message: " + tempMsg);
    client.publish("myTopic/temp", tempMsg);

    Serial.println("Publish message: " + humidityMsg);
    client.publish("myTopic/humidity", humidityMsg);
  }
}
