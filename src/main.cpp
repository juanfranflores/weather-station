#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// ---------------------------- DEFINICIÓN DE PINES ----------------------------

#define DHTPIN 4
#define DHTTYPE DHT11
#define ONE_WIRE_BUS 13

// ---------------------------- CREDENCIALES DE CONEXIÓN ----------------------------

const char *ssid = "Flores 2.4GHz";
const char *password = "Lilas549";
const char *mqtt_server = "192.168.1.11";
const char *clientId = "weatherStation";

// ---------------------------- VARIABLES DE SENSORES ----------------------------

float tempDHT11 = 0;
float tempDS18B20 = 0;
float tempBMP180 = 0;
float humDHT11 = 0;
float pressBMP180 = 0;

//  ---------------------------- FEEDS  ----------------------------

const char *tempDHT11Feed = "homeassistant/number/weatherStation/temperature/DHT11";
const char *tempDS18B20Feed = "homeassistant/number/weatherStation/temperature/DS18B20";
const char *tempBMP180Feed = "homeassistant/number/weatherStation/temperature/BMP180";
const char *humDHT11Feed = "homeassistant/number/weatherStation/humidity";
const char *pressBMP180Feed = "homeassistant/number/weatherStation/pressure";

// ---------------------------- INSTANCIAS DE SENSORES  ----------------------------

OneWire oneWire(ONE_WIRE_BUS);       ///////////// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); ///// Pass our oneWire reference to Dallas Temperature sensor
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);

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
  printSensorData();
  publishData(tempBMP180,tempBMP180Feed);

  delay(5000);
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
  tempDHT11 = dht.readTemperature();
  humDHT11 = dht.readHumidity();
  // sensors.requestTemperatures();
  // delay(1000);
  // tempDS18B20 = sensors.getTempCByIndex(0);
  tempBMP180 = bmp.readTemperature();
  pressBMP180 = bmp.readPressure();
}
void publishData(float value, const char *feed)
{
  client.publish(feed, String(value).c_str(), true);
}
void printSensorData()
{
  // Print BMP180 sensor data
  Serial.println("------------------------");
  Serial.print("Temperatura BMP180 = ");
  Serial.print(tempBMP180);
  Serial.println(" ºC");
  Serial.print("Presión BMP180 = ");
  Serial.print(pressBMP180);
  Serial.println(" Pa");
  Serial.println("------------------------");
  // // Read DS18B20 sensor data
  Serial.print("temperatura DS18B20:");
  Serial.print(tempDS18B20);
  Serial.println("ºC");
  // // Print DHT11 sensor data
  Serial.println("------------------------");
  Serial.print(F("Temperatura DHT11: "));
  Serial.print(tempDHT11);
  Serial.println(F("ºC "));
  Serial.print(F("Humedad DHT11: "));
  Serial.print(humDHT11);
  Serial.println(F("%"));
  Serial.println("------------------------");
}
void initBMP()
{
  if (!bmp.begin())
  {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1)
    {
    }
  }
}