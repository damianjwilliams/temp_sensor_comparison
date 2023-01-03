#include <ArduinoJson.h>

#include <Wire.h> // library for I2C communication 
#include <Adafruit_ADS1X15.h> //Library of external adc
#include "DHT.h"
#include <SPI.h>
#include "Adafruit_MAX31856.h"
#include <Adafruit_MAX31865.h>
#include <SparkFun_TMP117.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SparkFunTMP102.h> // Used to send and recieve specific information from our sensor
#include <Adafruit_BME280.h>
#include "Adafruit_MCP9808.h"


#include "Adafruit_SHTC3.h"
#include "Adafruit_Si7021.h"
#include <Adafruit_AHTX0.h>
#include "Adafruit_ADT7410.h"
#include "BluetoothSerial.h"



BluetoothSerial blueSer;
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth is not enabled! Please run `make menuconfig` to and enable it"
#endif




char input;

bool enableHeater = false;


#define TCAADDR 0x74

#define ONE_WIRE_BUS 4

//TwoWireZero
#define SDA_1 16
#define SCL_1 17

//TwoWireOne
#define SDA_2 21
#define SCL_2 22

#define DHTPIN22 5     // Digital pin connected to the DHT sensor
#define DHTTYPE22 DHT22

#define RREF      4300.0
#define RNOMINAL  1000.0

TwoWire I2C_0 = TwoWire(0);
TwoWire I2C_1 = TwoWire(1);


DHT dht22(DHTPIN22, DHTTYPE22);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address




Adafruit_ADS1115 ads0;
Adafruit_ADS1115 ads1;// define the types of external adc
Adafruit_ADS1115 ads2;
Adafruit_ADS1115 ads3; 
TMP102 tmp102a;
TMP102 tmp102b;
TMP117 tmp117;
TMP102 tmp102c;
TMP117 tmp117b;
// Create I2C LM75A instance
Adafruit_BME280 bme; 
Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
Adafruit_MCP9808 MCP9808_sensor = Adafruit_MCP9808();
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
Adafruit_Si7021 si7021_sensor = Adafruit_Si7021(&I2C_0);




void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  I2C_1.beginTransmission(TCAADDR);
  I2C_1.write(1 << i);
  I2C_1.endTransmission();  
}



// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(25);
Adafruit_MAX31856 maxim = Adafruit_MAX31856(33); 

//Camera = 32

int numberOfDevices; // Number of temperature devices found
byte currentPortNumber;

String temps_together;

String BME_reading;
String MCP9808_reading;

int sensorPin = 10;

void setup(void) 
{
Serial.begin (115200);

//Wire.begin();
//blueSer.begin("ESP32_enviroment_sensor"); //Bluetooth device name
//Serial.println("The device started, now you can pair it with bluetooth!");

I2C_0.begin(SDA_1, SCL_1); 
I2C_1.begin(SDA_2, SCL_2);



ads0.setGain(GAIN_ONE); 
ads1.setGain(GAIN_ONE); 
ads2.setGain(GAIN_TWO); 
ads3.setGain(GAIN_TWO); 


//Thermocouple and resistance temperature detectors
thermo.begin(MAX31865_2WIRE);  // set to 2WIRE or 4WIRE as necessary
maxim.begin();
maxim.setThermocoupleType(MAX31856_TCTYPE_K);





bool status0 = ads0.begin(0x48,&I2C_0);  
 if (!status0) {
   Serial.println("Could not find ADS1115 sensor 0, check wiring!");
   while (1);
  }

bool status1 = ads1.begin(0x49,&I2C_0);  
  if (!status1) {
    Serial.println("Could not find ADS1115 sensor 1 , check wiring!");
    while (1);
  }


bool status7 = tmp102a.begin(0x4A,I2C_0); 
  if (!status7) {
  Serial.println("Could not find TMP 0 sensor, check wiring!");
 while (1);
 }

bool status8 = shtc3.begin(&I2C_0);
  if (!status8) {
  Serial.println("Couldn't find SHTC3");
 while (1);
 }



  Serial.println("Si7021 test!");
  
  if (!si7021_sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true);
      
  }
 
 ////

Serial.println("ADS1115 sensor 2 test!");

bool status2 = ads2.begin(0x48,&I2C_1);  
  if (!status2) {
    Serial.println("Could not find  ADS1115 sensor 2, check wiring!");
    while (1);
  }

Serial.println("ADS1115 sensor 3 test!");
bool status3 = ads3.begin(0x4B,&I2C_1);  
  if (!status3) {
    Serial.println("Could not find  ADS1115 sensor 3, check wiring!");
    while (1);
}

Serial.println("BME test!");
bool status4 = bme.begin(0x77,&I2C_1); 
  if (!status4) {
  Serial.println("Could not find a valid BME280_1 sensor, check wiring!");
 while (1);
 }

Serial.println("MCP9808 test!");
bool status5 = MCP9808_sensor.begin(0x18,&I2C_1); 
 if (!status5) {
  Serial.println("Could not find a valid MCP9808 sensor, check wiring!");
 while (1);
 }

 Serial.println("TMP102 b test!");
 bool status6 = tmp102b.begin(0x49,I2C_1); 
  if (!status6) {
  Serial.println("Could not find TMP 0 sensor, check wiring!");
 while (1);
 }

Serial.println("TMP117 test!");
bool statusa = tmp117.begin(0x4A,I2C_1);  
 if (!statusa) {
 Serial.println("Could not find TMP117 , check wiring!");
while (1);
}



dht22.begin();




//--------------------------DS18B20------------------------
sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
 // Serial.print("Locating devices...");
 // Serial.print("Found ");
 // Serial.print(numberOfDevices, DEC);
 // Serial.println(" devices.");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      sensors.setResolution(tempDeviceAddress,12);
    //  Serial.print("Found device ");
    //  Serial.print(i, DEC);
    //  Serial.print(" with address: ");
      //printAddress(tempDeviceAddress);
    //  Serial.println();
    } else {
    //  Serial.print("Found ghost device at ");
    //  Serial.print(i, DEC);
    //  Serial.print(" but could not detect address. Check power and cabling");
    }
  }
//------------------------------------------------------------


  
 


  delay(1000);


}
 




void loop(void) {




//
  
//Wire 1//
int16_t adc0_0 = ads0.readADC_SingleEnded(0); // read ANO values
float temp0_0 = (adc0_0 * 0.125)/1000; // convert ADC value into voltage
temp0_0 = temp0_0 * 100; // converts voltage into temperature 10mv=1C

int16_t adc0_1 = ads0.readADC_SingleEnded(1); // read ANO values
float temp0_1 = (adc0_1 * 0.125)/1000; // convert ADC value into voltage
temp0_1 = temp0_1 * 100; // converts voltage into temperature 10mv=1C

int16_t adc0_2 = ads0.readADC_SingleEnded(2); // read ANO values
float temp0_2 = (adc0_2 * 0.125)/1000; // convert ADC value into voltage
temp0_2 = temp0_2 * 100; // converts voltage into temperature 10mv=1C

int16_t adc0_3 = ads0.readADC_SingleEnded(3); // read ANO values
float temp0_3 = (adc0_3 * 0.125)/1000; // convert ADC value into voltage
temp0_3 = temp0_3 * 100; // converts voltage into temperature 10mv=1C

////second ADS1115 
int16_t adc1_0 = ads1.readADC_SingleEnded(0); // read ANO values
float temp1_0 = (adc1_0 * 0.125)/1000; // convert ADC value into voltage
temp1_0 = temp1_0 * 100; // converts voltage into temperature 10mv=1C

int16_t adc1_1 = ads1.readADC_SingleEnded(1); // read ANO values
float temp1_1 = (adc1_1 * 0.125)/1000; // convert ADC value into voltage
temp1_1 = temp1_1 * 100; // converts voltage into temperature 10mv=1C

int16_t adc1_2 = ads1.readADC_SingleEnded(2); // read ANO values
float voltage = adc1_2*2.048;
voltage /= 16383.0;
float temp1_2 = (voltage-0.5)*100 ;

int16_t adc1_3 = ads1.readADC_SingleEnded(3); // read ANO values
voltage = adc1_2*2.048;
voltage /= 16383.0;
float temp1_3 = (voltage-0.5)*100 ;


//Wire 2

//tmp36

//third ADS1115
int16_t adc2_0 = ads2.readADC_SingleEnded(0); 
float temp2_0 = int16ToC(adc2_0);
int16_t adc2_1 = ads2.readADC_SingleEnded(1); 
float temp2_1 = int16ToC(adc2_1);
int16_t adc2_2 = ads2.readADC_SingleEnded(2); 
float temp2_2 = int16ToC(adc2_2);
int16_t adc2_3 = ads2.readADC_SingleEnded(3); 
float temp2_3 = int16ToC(adc2_3);

//fourth ADS1115 
int16_t adc3_0 = ads3.readADC_SingleEnded(0); // read ANO values
float temp3_0 = int16ToC(adc3_0);
int16_t adc3_1 = ads3.readADC_SingleEnded(1); // read ANO values
float temp3_1 = int16ToC(adc3_1);
int16_t adc3_2 = ads3.readADC_SingleEnded(2); // read ANO values
float temp3_2 = int16ToC(adc3_2);
int16_t adc3_3 = ads3.readADC_SingleEnded(3); // read ANO values
float temp3_3 = int16ToC(adc3_3);

/*
Serial.println();
Serial.println("WIRE 1"); 
print_temp("LM35 1: ",String(temp0_0));
print_temp("LM35 2: ",String(temp0_1));
print_temp("LM35 3: ",String(temp0_2));
print_temp("LM35 4: ",String(temp0_3));
print_temp("LM35 5: ",String(temp1_0));
print_temp("LM35 6: ",String(temp1_1));
print_temp("TMP35 1: ",String(temp1_2));
print_temp("TMP35 2: ",String(temp1_3));

Serial.println();
Serial.println("WIRE 2");
print_temp("TMP35 3: ",String(temp2_0));
print_temp("TMP35 4: ",String(temp2_1));
print_temp("TMP35 5: ",String(temp2_2));
print_temp("TMP35 6: ",String(temp2_3));
print_temp("TMP35 7: ",String(temp3_0));
print_temp("TMP35 8: ",String(temp3_1));
print_temp("TMP35 9: ",String(temp3_2));
print_temp("TMP35 10: ",String(temp3_3));

*/


//DHT22
float t22 = dht22.readTemperature();
//Serial.print(F("DHT22  Temperature:\t"));
//Serial.print(t22);
//Serial.print(F("°C\n"));


//TMP102
float TMP_0_tmp;
TMP_0_tmp = tmp102a.readTempC();
//Serial.print(F("TMP102 1 Temperature:\t"));
//Serial.print(TMP_0_tmp);
//Serial.print(F("°C\n"));

//TMP102
float TMP_1_tmp;
TMP_1_tmp = tmp102b.readTempC();
///Serial.print(F("TMP102 1 Temperature:\t"));
//Serial.print(TMP_1_tmp);
//Serial.print(F("°C\n"));

//BME280
sensors_event_t temp_event;
bme_temp->getEvent(&temp_event);
float bme_temperature = temp_event.temperature;
//Serial.print(F("BME280 Temperature:\t"));
//Serial.print(temp_event.temperature);
//Serial.print(F("°C\n"));

//MCP9808
MCP9808_sensor.wake();   // wake up, ready to read!
float MCP9808_temp = MCP9808_sensor.readTempC();

//Serial.print(F("MCP9808 Temperature:\t"));
///Serial.print(MCP9808_temp);
///Serial.print(F("°C\n"));

//float MCP9808b_temp = MCP9808_sensorb.readTempC();
//float MCP9808_temp = 1234;
//Serial.print(F("MCP9808b Temperature:\t"));
//Serial.print(MCP9808b_temp,4);
//Serial.print(F("°C\n"));


//LM75A
 //float temperature_in_degrees = lm75a.getTemperatureInDegrees();
//Serial.print(F("LM75A Temperature:\t"));
//Serial.print(temperature_in_degrees);
//Serial.print(F("°C\n"));

//TMP117
float tmp117_temperature = tmp117.readTempC();
//Serial.print(F("TMP117 Temperature:\t"));
//Serial.print(tmp117_temperature);
//Serial.print(F("°C\n"));


//SHTC3
sensors_event_t humidity3, temp3;
shtc3.getEvent(&humidity3, &temp3);// populate temp and humidity objects with fresh data
//Serial.print(" SHTC3 Temperature: "); Serial.print(temp3.temperature); Serial.println(" degrees C");
//Serial.print("Humidity: "); Serial.print(humidity3.relative_humidity); Serial.println("% rH");

//Si7021
//Serial.print("Humidity:    ");
//Serial.print(si7021_sensor.readHumidity(), 2);
 // Serial.print("\t Si7021 Temperature: ");
 // Serial.println(si7021_sensor.readTemperature(), 2);
  //float si7021_temp = 
 // delay(1000);

/*

 tcaselect(2);
    float temperature_in_degrees_a = temperature_a.readTemperatureC();
    Serial.print("LM45A A Temperature: ");
    Serial.print(temperature_in_degrees_a);
    Serial.print("\n");

tcaselect(4);
     float temperature_in_degrees_b = temperature_b.readTemperatureC();
    Serial.print("LM45A B Temperature: ");
    Serial.print(temperature_in_degrees_b);
    Serial.print("\n");
 */

//tcaselect(5);
 // float adt7410_reading;
 // adt7410_reading = adt7410.readTempC();
 //  Serial.print("ADT7410 Temperature: ");
 // Serial.print(adt7410_reading);
 // Serial.print("\n");

//tcaselect(6);

 // sensors_event_t humidity, temp;
 // htu31d.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
 // Serial.print("HTU31D temperature");
 // Serial.print(temp.temperature);
 // Serial.print("\n");

 //tcaselect(7);
 
// sensors_event_t humidity2, temp2;
// aht20.getEvent(&humidity2, &temp2);
 //Serial.print(" AHT20 Temperature: "); 
 //Serial.print(temp2.temperature);
 //Serial.println("degrees C");

  


//------------------Thermocouple and RTD------------------

uint16_t rtd = thermo.readRTD();
//Serial.print("\nRTD value: "); Serial.println(rtd);
float ratio = rtd;
ratio /= 32768;

float RTD_31865_temp = thermo.temperature(RNOMINAL, RREF);
//Serial.print(F("MAX381865 RTD Temperature:\t"));
//Serial.print(RTD_31865_temp);
//Serial.print(F("°C\n"));

float TC_31856_temp = maxim.readThermocoupleTemperature();
//Serial.print(F("MAX31856 TC Temperature:\t"));
//Serial.print(TC_31856_temp);
//Serial.print(F("°C\n"));

//---------------------------------------------------------


StaticJsonDocument<1000> doc;

JsonArray Temperature_0_Data = doc.createNestedArray("DS18B20");
sensors.requestTemperatures();
for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      // Output the device ID
      //Serial.print("Temperature for device: ");
      //Serial.println(i,DEC);
      // Print the data
      float tempC = sensors.getTempC(tempDeviceAddress);
      //Serial.print("Temp C: ");
      Temperature_0_Data.add(tempC);
      //Serial.print(" Temp F: ");
      //Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
    }
  }

JsonArray Temperature_1_Data = doc.createNestedArray("MCP");
Temperature_1_Data.add(MCP9808_temp);



JsonArray Temperature_2_Data = doc.createNestedArray("TMP102");
Temperature_2_Data.add(TMP_0_tmp);
Temperature_2_Data.add(TMP_1_tmp);


JsonArray Temperature_3_Data = doc.createNestedArray("BME280");
Temperature_3_Data.add(temp_event.temperature);


JsonArray Temperature_4_Data = doc.createNestedArray("Thermocouple");
Temperature_4_Data.add(maxim.readThermocoupleTemperature());


JsonArray Temperature_5_Data = doc.createNestedArray("RTD");
Temperature_5_Data.add(thermo.temperature(RNOMINAL, RREF));


JsonArray Temperature_6_Data = doc.createNestedArray("DHT 22");
Temperature_6_Data.add(t22);

JsonArray Temperature_8_Data = doc.createNestedArray("LM35");
Temperature_8_Data.add(temp0_0);
Temperature_8_Data.add(temp0_1);
Temperature_8_Data.add(temp0_2);
Temperature_8_Data.add(temp0_3);
Temperature_8_Data.add(temp1_0);
Temperature_8_Data.add(temp1_1);


JsonArray Temperature_9_Data = doc.createNestedArray("TMP35");
Temperature_9_Data.add(temp1_2);
Temperature_9_Data.add(temp1_3);
Temperature_9_Data.add(temp2_0);
Temperature_9_Data.add(temp2_1);
Temperature_9_Data.add(temp2_2);
Temperature_9_Data.add(temp2_3);
Temperature_9_Data.add(temp3_0);
Temperature_9_Data.add(temp3_1);
Temperature_9_Data.add(temp3_2);
Temperature_9_Data.add(temp3_3);


//JsonArray Temperature_10_Data = doc.createNestedArray("L75A");
//Temperature_10_Data.add(temperature_in_degrees);


JsonArray Temperature_11_Data = doc.createNestedArray("TMP117");
Temperature_11_Data.add(tmp117.readTempC());

JsonArray Temperature_12_Data = doc.createNestedArray("SHTC3");
Temperature_12_Data.add(temp3.temperature);


JsonArray Temperature_13_Data = doc.createNestedArray("si7021");
Temperature_13_Data.add((si7021_sensor.readTemperature()));


input = Serial.read(); 

if (input == 'R') {
    serializeJson(doc, Serial);
    }
    




//if (blueSer.available()) 
 // {
    //input = blueSer.read(); 
    //Serial.print("Received:"); Serial.println(input);
    //serializeJson(doc, blueSer);
    
 //   blueSer.println(String("damian"));

  
    
 // }



delay(2000);



}




float int16ToC(int16_t value)
{
    float voltage = value*2.048;
    voltage /= 32767.0;
    return (voltage-0.5)*100 ;
}


void  print_temp(const String& SensorID,const String& temperature)
{ 
   Serial.println(SensorID+" "+temperature);
    return; 
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}
