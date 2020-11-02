#include <Wire.h>
#include <Adafruit_BMP280.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <SPI.h>
#include <SD.h>

Adafruit_BMP280 bmp;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

const int chipSelect = 10;

void setup() {
  Serial.begin(115200);

  Serial.println(F("BMP280 test"));

  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1); // death loop
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  Serial.println("Orientation Sensor Test");
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(true);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

}
float sicaklik, basinc = 0;
void loop() {
  String dataString = "";
  Serial.print(F("Temperature = "));
  sicaklik = bmp.readTemperature();
  Serial.print(sicaklik);
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  basinc = bmp.readPressure();
  Serial.print(basinc);
  Serial.println(" Pa");

  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */ // hPa
  Serial.println(" m");

  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  
  
  /* Display the floating point data */
  Serial.print("X: ");
  Serial.print(event.orientation.x, 4);
  Serial.print("\tY: ");
  Serial.print(event.orientation.y, 4);
  Serial.print("\tZ: ");
  Serial.print(event.orientation.z, 4);
  Serial.println("");

  Serial.println();

  dataString = String(sicaklik) + "," + String(basinc);
  logSD(dataString);
  delay(1000);

}

void logSD(String dataS){
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataS);
    dataFile.close();
    // print to the serial port too:
    Serial.println("\nSaved: ");
    Serial.println(dataS);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
