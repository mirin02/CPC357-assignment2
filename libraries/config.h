// config.h
#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials
#define WIFI_SSID "zainab-2.4GHz"
#define WIFI_PASSWORD "zai0133941228"

// MQTT broker settings
#define MQTT_SERVER "35.225.251.143" // Your VM instance public IP address
#define MQTT_PORT 1883               // MQTT broker port (non-TLS)
#define MQTT_TOPIC "iot"             // MQTT topic for publishing data

// DHT sensor configuration
#define DHTTYPE DHT11   // Define the type of DHT sensor
#define DHTPIN 22       // DHT sensor pin

// Soil sensor and relay pins
#define SOIL_SENSOR_PIN 34  // Soil moisture sensor pin
#define RELAY_PIN 32        // Relay control pin

// Soil moisture thresholds
#define MOISTURE_THRESHOLD_LOW 30  // Turn pump ON
#define MOISTURE_THRESHOLD_HIGH 60 // Turn pump OFF

#endif
