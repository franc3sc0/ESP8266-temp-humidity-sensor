#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "PrivateEnvVariables.h"

#define DHTPIN 4      // DHT Sensor connected to digital pin 2.
#define DHTTYPE DHT11 // Type of DHT sensor.

WiFiClient client;        // Initialize the Wifi client library.
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

const unsigned long postingInterval = 60L * 1000L; // Post data interval (60 seconds)

int connectWifi();
void updateDHT();
unsigned long lastConnectionTime = 0;
float dhtTemp = 0;
float dhtHumidity = 0;

WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

// Attempts a connection to WiFi network and repeats until successful.
int connectWifi()
{
  digitalWrite(LED_BUILTIN, LOW);  // BUILTIN LED on to indicate WiFi connection in progress
  WiFi.begin(ssid, wifiPwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println( "Connecting to WiFi..." );
    delay(2500);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  // Serial.println( "Connected to WiFi" );
}

void updateDHT()
// Get the most recent readings for temperature and humidity.
{
  digitalWrite(2, LOW);   // ESP-12 LED on to indicate we're reading temp, humidity
  // To return Fahrenheit use dht.readTemperature(true)
  dhtTemp = dht.readTemperature();
  dhtHumidity = dht.readHumidity();
  Serial.println( dhtTemp );
  Serial.println( dhtHumidity );
  digitalWrite(2, HIGH);
}

void setup()
{
  Serial.begin( 115200 );
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(2, OUTPUT);               // Initialize GPIO2 pin as an output
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(2, HIGH);
  Serial.println( "Starting" );

  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP & event)
  {
    digitalWrite(LED_BUILTIN, HIGH); // Turn  BUILTIN LED off to indicate WiFi connection
    Serial.print( "Connected to Wifi with IP: " );
    Serial.println(WiFi.localIP());
  });

  disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected & event)
  {
    // digitalWrite(LED_BUILTIN, LOW);  // BUILTIN LED on to indicate no WiFi connection
    Serial.println( "Disconnected from WiFi" );
    connectWifi();
  });

  connectWifi();

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
    ThingSpeak.writeFields(tsChannelID, tsWriteAPIKey);

    lastConnectionTime = millis();
  }
}
