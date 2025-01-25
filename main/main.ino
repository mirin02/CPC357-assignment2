#include <PubSubClient.h>
#include <WiFi.h>
#include "DHT.h"
#include "config.h" // Include the configuration file

// Global variables
DHT dht(DHTPIN, DHTTYPE);         // Initialize DHT sensor
WiFiClient espClient;             // WiFi client
PubSubClient client(espClient);   // MQTT client
char mqttBuffer[256];             // MQTT message buffer

// Function to connect to WiFi
void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Function to reconnect to MQTT broker
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Attempting to connect to MQTT broker...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Setup function
void setup() {
  Serial.begin(115200);                  // Initialize serial communication
  setupWiFi();                           // Connect to WiFi
  dht.begin();                           // Initialize DHT sensor
  client.setServer(MQTT_SERVER, MQTT_PORT);  // Configure MQTT server

  pinMode(RELAY_PIN, OUTPUT);      // Set relay pin as output
  digitalWrite(RELAY_PIN, LOW);  // Ensure the pump is off initially
}

// Main loop
void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Read temperature and soil moisture
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  int sensorValue = analogRead(SOIL_SENSOR_PIN);
  int soilMoisturePercent = map(sensorValue, 0, 4095, 100, 0);

  // Control water pump based on soil moisture
  if (soilMoisturePercent > MOISTURE_THRESHOLD_LOW) {
    digitalWrite(RELAY_PIN, LOW);  // Turn pump ON
    Serial.println("Pump OFF: Soil moisture is sufficient.");
  } else if (soilMoisturePercent < MOISTURE_THRESHOLD_HIGH) {
    digitalWrite(RELAY_PIN, HIGH);  // Turn pump OFF
    Serial.println("Pump ON: Soil moisture is low.");
  }

  // Format MQTT message
  snprintf(mqttBuffer, sizeof(mqttBuffer),
           "{\"temperature\": %.2f, \"humidity\": %.2f, \"soil_moisture\": %d}",
           temperature, humidity, soilMoisturePercent);

  // Publish MQTT message
  if (client.publish(MQTT_TOPIC, mqttBuffer)) {
    Serial.println("Data published successfully:");
    Serial.println(mqttBuffer);
  } else {
    Serial.println("Failed to publish data!");
  }

  delay(5000);  // Delay before next reading
}
