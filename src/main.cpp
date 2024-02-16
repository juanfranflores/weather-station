#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// ---------------------------- DEFINICIÓN DE PINES ----------------------------

#define DHTPIN 4
#define DHTTYPE DHT22
#define ONE_WIRE_BUS 13

// ---------------------------- CREDENCIALES DE CONEXIÓN ----------------------------

const char *ssid = "Flores 2.4GHz";
const char *password = "Lilas549";
const char *mqtt_server = "docker.local";
const char *clientId = "weather-station";

// ---------------------------- VARIABLES DE SENSORES ----------------------------

float tempDHT22 = 0;
float tempDS18B20 = 0;
float tempBMP280 = 0;
float humDHT22 = 0;
float pressBMP280 = 0;

//  ---------------------------- FEEDS  ----------------------------

const char *tempDHT22Feed = "homeassistant/sensor/weather-station/temperatureDHT22";
const char *tempDS18B20Feed = "homeassistant/sensor/weather-station/temperatureDS18B20";
const char *tempBMP280Feed = "homeassistant/sensor/weather-station/temperatureBMP280";
const char *humDHT22Feed = "homeassistant/sensor/weather-station/humidityDHT22";
const char *pressBMP280Feed = "homeassistant/sensor/weather-station/pressureBMP280";

// ---------------------------- INSTANCIAS DE SENSORES  ----------------------------

OneWire oneWire(ONE_WIRE_BUS);       ///////////// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); ///// Pass our oneWire reference to Dallas Temperature sensor
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;
//  ---------------------------- DEFINICIÓN DE FUNCIONES  ----------------------------
void printSensorData();
void readSensorData();
void initBMP();
void publishData(float value, const char *feed);

//  ---------------------------- INICIALIZACIÓN DE LIBRERÍAS ----------------------------
WiFiClient espClient;
PubSubClient client(espClient);

//  ---------------------------- DECLARACIÓN DE FUNCIONES ----------------------------
void setup_wifi();
void reconnect();
void callback(char *topic, byte *message, unsigned int length);

void setup()
{
  Serial.begin(115200);
  dht.begin();
  sensors.begin();
  sensors.setResolution(12);
  initBMP();
  delay(2000);
  // while (!Serial);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  reconnect();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  readSensorData();
  // printSensorData();
  publishData(tempBMP280, tempBMP280Feed);
  publishData(tempDHT22, tempDHT22Feed);
  publishData(tempDS18B20, tempDS18B20Feed);
  publishData(humDHT22, humDHT22Feed);
  publishData(pressBMP280, pressBMP280Feed);

  delay(10000);
}
void initBMP()
{
  if (!bmp.begin(0x76))
  {
    Serial.println("Could not find a valid BMP085/BMP280 sensor, check wiring!");
    while (1)
    {
    }
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

//  ---------------------------- DEFINICIÓN DE FUNCIONES ----------------------------
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (!client.connect(clientId))
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    else
    {
      Serial.println("connected");
    }
  }
}

void callback(char *topic, byte *message, unsigned int length)
{
  // Llegó un nuevo mensaje
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feeds de setup min y max
  // if ((String(topic) == topic))
  // {

  // }
}

void readSensorData()
{
  tempDHT22 = dht.readTemperature();
  humDHT22 = dht.readHumidity();
  sensors.requestTemperatures();
  delay(200);
  tempDS18B20 = sensors.getTempCByIndex(0);
  tempBMP280 = bmp.readTemperature();
  pressBMP280 = bmp.readPressure() / 100;
}
void printSensorData()
{
  // Print BMP280 sensor data
  Serial.println("------------------------");
  Serial.print("Temperatura BMP280 = ");
  Serial.print(tempBMP280);
  Serial.println(" ºC");
  Serial.print("Presión BMP280 = ");
  Serial.print(pressBMP280);
  Serial.println(" hPa");
  Serial.println("------------------------");
  // // Read DS18B20 sensor data
  Serial.print("temperatura DS18B20:");
  Serial.print(tempDS18B20);
  Serial.println("ºC");
  // // Print DHT22 sensor data
  Serial.println("------------------------");
  Serial.print(F("Temperatura DHT22: "));
  Serial.print(tempDHT22);
  Serial.println(F("ºC "));
  Serial.print(F("Humedad DHT22: "));
  Serial.print(humDHT22);
  Serial.println(F("%"));
  Serial.println("------------------------");
}
void publishData(float value, const char *feed)
{
  client.publish(feed, String(value).c_str(), true);
}