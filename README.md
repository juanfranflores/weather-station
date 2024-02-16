# Weather Station
## Platform
Homeassistant's MQTT broker.
## Feeds:
- Temperatura:
  - DHT11 = "homeassistant/number/weather-station/temperature/DHT11"  
  - DS18B20 = "homeassistant/number/weather-station/temperature/DS18B20"
  - BMP180 = "homeassistant/number/weather-station/temperature/BMP180"
- Humedad:
  - DHT11 = "homeassistant/number/weather-station/humidity"
- Presión:
  - BMP180 = "homeassistant/number/weather-station/pressure"