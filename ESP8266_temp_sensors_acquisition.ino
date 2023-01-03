// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"
#include <Wire.h>
#include <SparkFun_TMP117.h>
#include <SparkFunTMP102.h>
#include <ArduinoJson.h>

#define DHTPIN_1 D3
#define DHTPIN_2 D4
#define DHTPIN_3 D5 // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE1 DHT11   // DHT 11
#define DHTTYPE2 DHT11
#define DHTTYPE3 DHT11  
 
// DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
DHT dht1(DHTPIN_1, DHTTYPE1);
DHT dht2(DHTPIN_2, DHTTYPE2);
DHT dht3(DHTPIN_3, DHTTYPE3);

TMP117 tmp117;
TMP102 tmp102;

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  Wire.begin();

  dht1.begin();
   dht2.begin();
    dht3.begin();

    bool status7 = tmp102.begin(0x4B); 
  if (!status7) {
  Serial.println("Could not find TMP 0 sensor, check wiring!");
 while (1);
 Serial.println("TMP117 test!");
 }

bool statusB = tmp117.begin(0x48);  
 if (!statusB) {
 Serial.println("Could not find TMP117 , check wiring!");
while (1);
}
 }


void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  
  float temp1 = dht1.readTemperature();
  // Read temperature as Celsius (the default)
  float temp2 = dht2.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float temp3 = dht3.readTemperature();


 //Serial.println("DHT PIN 5");
 // Serial.println(temp1);

  // Serial.println("DHT PIN 18");
  //Serial.println(temp2);
 
  // Serial.println("DHT PIN 19");
 // Serial.println(temp3);

  //TMP102
float TMP_1_tmp;
TMP_1_tmp = tmp102.readTempC();
//Serial.print(F("TMP102 1 Temperature:\t"));
//Serial.print(TMP_1_tmp);
//Serial.print(F("°C\n"));

//TMP117
float tmp117_temperature = tmp117.readTempC();
//Serial.print(F("TMP117 Temperature:\t"));
//Serial.print(tmp117_temperature);
//Serial.print(F("°C\n"));



StaticJsonDocument<1000> doc;

//JsonArray Temperature = doc.createNestedObject("Temperature_json");

//JsonObject Temperature_1 = doc.createNestedObject();
//Temperature_1["TMP102_esp8266"];
JsonArray Temperature_1_Data = doc.createNestedArray("TMP102_esp8266");
Temperature_1_Data.add(TMP_1_tmp);

//JsonObject Temperature_2 = doc.createNestedObject();
//Temperature_2["TMP117_esp8266"];
JsonArray Temperature_2_Data = doc.createNestedArray("TMP117_esp8266");
Temperature_2_Data.add(tmp117_temperature);

//JsonObject Temperature_3 = doc.createNestedObject();
//Temperature_3["DHT11"];
JsonArray Temperature_3_Data = doc.createNestedArray("DHT_esp8266");
//Temperature_3_Data.add(temp1);
Temperature_3_Data.add(temp2);
Temperature_3_Data.add(temp3);

String output;
serializeJson(doc,Serial);
//const char* json = output.c_str();
//SerialBT.write(json)

delay(2000);

}
