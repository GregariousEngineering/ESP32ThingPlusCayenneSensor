/*
 * Reads SCD4x/Dallas Temp Probe and sends to Cayenne on SparkFun ESP32 Things Plus
 */

//#define CAYENNE_DEBUG       // Uncomment to show debug messages
//#define CAYENNE_PRINT Serial  // Comment this out to disable prints and save space

#include <WiFi.h>
#include <SFE_MicroOLED.h>
#include <CayenneMQTTWiFi.h>
#include <Wire.h> // Used to establish serial communication on the I2C bus 
#include <OneWire.h> // Single wire sensor driver
#include <DallasTemperature.h> // Temp sesnor driver, reqs OneWire
#include "SparkFun_SCD4x_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD4x

const int PIN_RESET = 9;  
const int DC_JUMPER = 1;
const int DS18B20_PIN = 26; // DS18B20 sensor's DQ pin
const int BUTTON_PIN = 0;
const int LED_PIN = 13;


// WiFi network name and password:
const char * networkName = "YOUR SSID";
const char * networkPass = "YOUR WIFI PASS";

// Cayenne authentication info
const char username[] = "YOUR DEVICES USERNAME";
const char password[] = "YOUR DEVICES PASSWORD";
const char clientID[] = "YOUR DEVICES CLIENTID";

// Cayenna setup
const long CAYENNE_UPDATE_INTERVAL = 30000L;
const int CO2_TOPIC = 0;
const int TEMP_TOPIC = 1;
const int RH_TOPIC = 2;
const int WATER_TEMP_TOPIC = 3;

// Global sensor update interval
const long UPDATE_INTERVAL = 30000L;
long updateTime = millis();

SCD4x scd4xSensor;

MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration
boolean isDisplay = true;

// Water temp sensor
OneWire oneWireDS18B20(DS18B20_PIN); // OneWire for temp prob
DallasTemperature waterTempSensor(&oneWireDS18B20); // DallasTemperature to interpret probe

void setup() {
  Serial.begin(115200);
  while (!Serial) ; // Wait for serial port to connect. Needed for USB for computer connection
  Serial.println(F("Qwiic SCD4x, DS18B20, and OLED Cayenne Example")); Serial.println();
  
  Serial.println(F("Beginning DS18B20..."));
  waterTempSensor.begin(); // Start DS18B20
  Serial.println(F("DS18B20 started.")); Serial.println();

  Wire.setClock(400000);   // Set clock speed to be the fastest for better communication (fast mode)
  Wire.begin();

  initDisplay();

  Serial.println("Beginning scd4x...");
  scd4xSensor.enableDebugging(); // Uncomment this line to get helpful debug messages on Serial
  if (scd4xSensor.begin() == false) { // begin() will start periodic measurements for us
    Serial.println(F("SCD4x Sensor failed to begin. Attempted re-init"));
    delay(10000l);
    scd4xSensor.reInit();
    if (scd4xSensor.begin() == false) { // begin() will start periodic measurements for us
      Serial.println(F("SCD4x Sensor still failed to begin...."));
    }
  } else {
    Serial.println("scd4x started.");
  }
    
  connectToWiFi(networkName, networkPass); Serial.println();

  Serial.println(F("Attempting to begin Cayenne client"));
  Cayenne.begin(username, password, clientID);
  Serial.println(F("Cayenne init complete.")); Serial.println();

  Serial.println(); Serial.println("setup() Complete!");
}

void loop()
{
  //Cayenne.loop();
  if(millis() > updateTime) { // Only send the update every UPDATE_INTERVAL
    sendUpdate();
    updateTime = millis() + UPDATE_INTERVAL;
  }
  delay(UPDATE_INTERVAL); // Not doing anything else so may as well just delay
}

void sendUpdate() {
  // LED on while sending update
  digitalWrite(LED_PIN, 1);

  Serial.println();
  Serial.print(F("Water Temp(DS18B20):"));
  waterTempSensor.requestTemperatures(); // Get temp
  float waterTemp = waterTempSensor.getTempCByIndex(0) * 9.0 / 5.0 + 32.0;
  Serial.print(waterTemp, 2);  
  Cayenne.virtualWrite(WATER_TEMP_TOPIC, waterTemp);

  float co2 = 0.0, temp = 0.0, humidity = 0.0;
  if (scd4xSensor.readMeasurement()) // readMeasurement will return true when fresh data is available
  {
    Serial.println();

    co2 = scd4xSensor.getCO2();
    Serial.print(F("CO2(ppm):"));
    Serial.print(co2);
    Cayenne.virtualWrite(CO2_TOPIC, co2);

    temp = scd4xSensor.getTemperature() * 9.0 / 5.0 + 32.0; // F
    Serial.print(F("\tTemperature(SCD41):"));
    Serial.print(temp, 1);
    Cayenne.virtualWrite(TEMP_TOPIC, temp);

    humidity = scd4xSensor.getHumidity();
    Serial.print(F("\tHumidity(%RH):"));
    Serial.print(humidity, 1);
    Cayenne.virtualWrite(RH_TOPIC, humidity);
  }
  else
    Serial.print(F("."));

  if(isDisplay) {
    oled.clear(PAGE);     // Clear the screen
    oled.setFontType(0);  // Set font to type 0
    oled.setCursor(0, 0); // Set cursor to top-left w=64 x l=64
    oled.print("CO2 "+String(co2,1));
    oled.setCursor(0, 12); 
    oled.print("Ta "+String(temp,1));
    oled.setCursor(0, 24);
    oled.print("Tw "+String(waterTemp,1));
    oled.setCursor(0, 36);
    oled.print("RH "+String(humidity,1)+"%");
    oled.display();
  }

  digitalWrite(LED_PIN, 0);
}

void connectToWiFi(const char * ssid, const char * pwd)
{
  int ledState = 0;

  Serial.println("Connecting to WiFi network: " + String(ssid));

  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) 
  {
    // Blink LED while we're connecting:
    digitalWrite(LED_PIN, ledState);
    ledState = (ledState + 1) % 2; // Flip ledState
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void initDisplay() {
  oled.begin();
  oled.clear(PAGE); // Clear the buffer.
  oled.setFontType(1);
  oled.print("Gregarious Engineer!!");
  oled.display();
  oled.setFontType(0);
}

// Default function for processing actuator commands from the Cayenne Dashboard.
// You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}
