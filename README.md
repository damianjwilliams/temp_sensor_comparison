# temp_sensor_comparison
Arduino code, python code, and raw data for comparing a few different temperature sensors  

This video shows a rough comparison of a few different temperature sensors using an ESP32 and ESP8266. For a reference temperature, I used a Eutronics 4500 thermocouple sensor, which is used to calibrate scientifc equipment (PCR machines). I also took readings from two glass thermometers (shown [here](https://www.youtube.com/watch?v=_NWB49mpcyM)).

The sensors tested are:

DHT11 [Adafruit](https://www.mouser.com/ProductDetail/Adafruit/386?qs=GURawfaeGuDbeGFpZ2393w%3D%3D&countrycode=US&currencycode=USD)

DHT22 [Adafruit](https://www.mouser.com/ProductDetail/Adafruit/385?qs=GURawfaeGuCJCHEuv7lcfg%3D%3D&countrycode=US&currencycode=USD)

PT1000 RTD [Heraeus](https://www.mouser.com/ProductDetail/Heraeus-Nexensos/32208727?qs=MQgg6%252BVqoaNZPs9PuUxgRw%3D%3D&countrycode=US&currencycode=USD) and MAX31865 [Adafruit](https://www.mouser.com/ProductDetail/Adafruit/3648?qs=chTDxNqvsyl5Basop5AIZg%3D%3D&countrycode=US&currencycode=USD)

Type K thermocouple [Reed Instruments](https://www.amazon.com/gp/product/B008S0K9KU) and MAX31856 [Adafruit](https://www.amazon.com/gp/product/B01LZBBI7D)

LM35 [Texas Instruments](https://www.mouser.com/ProductDetail/Texas-Instruments/LM35DZ-LFT1?qs=QbsRYf82W3H30AtKgrS0IA%3D%3D&countrycode=US&currencycode=USD)

TMP36 [KOOKYE](https://www.amazon.com/gp/product/B01GH4VNNK)

BME280 [Sparkfun](https://www.sparkfun.com/products/13676)

TMP102 [Sparkfun](https://www.amazon.com/gp/product/B086KZ6RWN/)

TMP117 [Sparkfun](https://www.amazon.com/gp/product/B081QK731D)

MCP9808 [Adafruit](https://www.mouser.com/ProductDetail/Adafruit/1782?qs=GURawfaeGuDb33OvJbnOMQ%3D%3D&countrycode=US&currencycode=USD)

SHTC3 [Adafruit](http://www.adafruit.com/product/4636)

DS18B20 [Eiechip](https://www.amazon.com/dp/B07MR71WVS)

HTU310 [Adafruit](http://www.adafruit.com/product/4832)

ADT7410 [Adafruit](http://www.adafruit.com/product/4089)

AHT20 [Adafruit](http://www.adafruit.com/product/4566)

LM75A [HiLetgo](https://www.amazon.com/gp/product/B082KLD7XT)

Si7021[Adafruit](http://www.adafruit.com/product/3251)

The ESP32-CAM Arduino code was the CameraWebServer example from the AI Thinker ESP32 library. For the analog LM35 and TMP36, a ADS1115 ADCs were used to increase the resolution.

The measurements were taken in a room in my house over a 6 hour period, taking a measurement every 2 s. There is a period where Ithe window is open  and another point where the sun shines for a few minutes. 

Many of the readings were higher than the reference which either means the reference is off, or the sensors are heating up a little. It is probably worth moving them away from the microcontroller and other sensors to reduce this a fan may also help. Taking fewer readings and using a sleep period (where possible) between measurements would probably help. 

The python script creates a json file at each time point. The file name of each timepoint is a timestamp. All the plots were created in R.  To create the scrolling video, frames at each timepoint were create in R, and coverted to a video in ImageJ.


