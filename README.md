# temp_sensor_comparison
Arduino code, python code, and raw data for comparing a few different temperature sensors  


DHT11

DHT22

PT100 RTD (and MAX31865)

Type K thermocouple (and MAX31856)

LM35
TMP35
BME280
TMP102
TMP117
MCP9808
SHTC3
DS18B20
HTU310
ADT7410
AHT20
LM75A

This video shows a rough comparison of a few different temperature sensors using an ESP32 and ESP8266. For a reference temperature, I used a Eutronics 4500 thermocouple sensor, which is used to calibrate scientifc equipment (PCR machines). I also took readings from two glass thermometers (described here).

The sensors tested are:
DHT11
DHT22
PT100 RTD (and MAX31865)
Type K thermocouple (and MAX31856)
LM35
TMP35
BME280
TMP102
TMP117
MCP9808
SHTC3
DS18B20
HTU310
ADT7410
AHT20
LM75A

The measurements were taken in a room in my house over a 6 hour period, taking a measurement every 2 s. There is a period where Ithe window is open  and another point where the sun shines for a few minutes. 

Many of the readings were higher than the reference which either means the reference is off, or the sensors are heating up a little. It is probably worth moving them away from the microcontroller and other sensors to reduce this a fan may also help. Taking fewer readings and using a sleep period (where possible) between measurements would probably help. 

Sensor details, Arduino code and raw data can be found on my Github site:
https://github.com/damianjwilliams/temp_sensor_comparison
