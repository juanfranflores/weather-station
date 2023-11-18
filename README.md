# Weather Station
## Platform
Homeassistant's MQTT broker.
## Feeds:
- Temperatura:
  - DHT11 = "homeassistant/number/weatherStation/temperature/DHT11"  
  - DS18B20 = "homeassistant/number/weatherStation/temperature/DS18B20"
  - BMP180 = "homeassistant/number/weatherStation/temperature/BMP180"
- Humedad:
  - DHT11 = "homeassistant/number/weatherStation/humidity"
- Presión:
  - BMP180 = "homeassistant/number/weatherStation/pressure"