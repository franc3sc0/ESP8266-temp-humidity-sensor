#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "PrivateEnvVariables.h"

#define DHTPIN 4      // DHT Sensor connected to digital pin 2.
#define DHTTYPE DHT11 // Type of DHT sensor.

const unsigned long postingInterval = 60L * 1000L; // Post data every 60 seconds.

WiFiClient client;        // Initialize the Wifi client library.
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

int connectWifi();
void updateDHT();
unsigned long lastConnectionTime = 0;
float dhtTemp = 0;
float dhtHumidity = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Start");

  connectWifi();

  Serial.println("Connected to wifi");
  ThingSpeak.begin(client);
}

void loop()
{
  // If interval time has passed since the last connection, write data to ThingSpeak.
  if (millis() - lastConnectionTime > postingInterval)
  {
    updateDHT();

    ThingSpeak.setField(1, dhtHumidity);
    ThingSpeak.setField(2, dhtTemp);
    ThingSpeak.writeFields(channelID, writeAPIKey);

    lastConnectionTime = millis();
  }
}

// Attempts a connection to WiFi network and repeats until successful.
int connectWifi()
{
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to WiFi");
    delay(2500);
  }
  Serial.println("Connected");
}

// Get the most recent readings for temperature and humidity.
void updateDHT()
{
  // To return Fahrenheit use dht.readTemperature(true)
  dhtTemp = dht.readTemperature();
  dhtHumidity = dht.readHumidity();
  Serial.println(dhtTemp);
  Serial.println(dhtHumidity);
}
