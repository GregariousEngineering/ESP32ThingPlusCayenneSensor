# ESP32 Thing Plus Cayenne Sensor Example
========================================

<img src="https://github.com/GregariousEngineering/UnoWifiRev2CayenneSensor/blob/master/images/AeroTempMon.jpeg" width="756" height="567">

A simple Ardruino class demonstrating Cayenne on ESP32 Thing Plus (since myDevices/Cayenne have no sample for this board)

This example logs SCD4x CO2/Temp/RH and DS18B20 temperature probe (or any compatible Dallas Probe; assumed to be on Digital IO 26) to Cayenne.

[![SparkFun Qwiic Kit for Raspberry Pi](https://cdn.sparkfun.com//assets/parts/1/4/2/4/1/15663-SparkFun_Thing_Plus_-_ESP32_WROOM-01.jpg)](https://www.sparkfun.com/products/15663)

[* "Real" Spinach & Arugula - Aero Garden *](https://www.gregariousengineering.com/2022/04/real-spinach-arugula-aero-garden.html)


Repository Contents
-------------------
* **/cayenne_co2_temps_and_rh.ino** - Example code 


Requirements
--------------
* **Qwiic sensors**
   * **[CO2 Humidity and Temperature Sensor - SCD41 (Qwiic)](https://www.sparkfun.com/products/18366)**
   * **[SparkFun Micro OLED Breakout (Qwiic)](https://www.sparkfun.com/products/14532)**
* **Temperature probe (Dallas compatible)
   * any
* **Ardruino Libraries**
   * **See includes**
* **Cayenne Account
   * **[Requires Cayenne setup a free account and add new a new device](https://developers.mydevices.com/cayenne/landing/jumpstart-arduino-projects-cayenne/)**

License Information
-------------------

This product is _**open source**_! 

Please review the LICENSE.md file for license information. 

Distributed as-is, with flaws; no warranty is given.

- GregariousEngineer