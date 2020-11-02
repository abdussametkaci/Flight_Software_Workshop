#include <ESPmDNS.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"

// Set your access point network credentials
const char* ssid = "CompEng";
const char* password = "h98a2k3?12.";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();

  //Serial.println("Starting SD Card");
  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed");
    while(1);
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    while(1);
  }

  // Setting the ESP as an access point
  Serial.print("Setting …");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  //WiFi.softAP(ssid, password);

  //IPAddress IP = WiFi.softAPIP();
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // http://esp32.local adresi ile cihaza erisilebilir
  if (!MDNS.begin("esp32")) {
    Serial.println("Error starting mDNS");
  }

  Serial.print("\nhostname: http://");
  Serial.print("esp32");
  Serial.println(".local");
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {

    request->send_P(200, "text/plain", "WELCOME!"); // OK

  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest * request) {

    request->send_P(200, "text/plain", "12, 23, 54");

  });

  server.on("/file", HTTP_POST, [](AsyncWebServerRequest * request) {
    // request->send(200);
    request->send_P(200, "text/plain", "File was uploaded succesfully");

  }, handleUploadFile);


  server.on(
    "/data",
    HTTP_POST,
  [](AsyncWebServerRequest * request) {
    // request->send(200); // karsiya hemen istek atılmasli yoksa server iste hemen alamadigi icin resetleniyor
    request->send_P(200, "text/plain", "Data was uploaded succesfully");
  },
  NULL,
  [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {

    saveData("/data.txt", data, len);

  });
  // Ister yukaridaki gibi labmda fonksiyon ile isterseniz de asagidaki gibi bir yazim tercih edebilirsiniz
  /*
    server.on(
    "/data",
    HTTP_POST,
    [](AsyncWebServerRequest * request) {},
    NULL,
    handleBody);
  */


  // Start server
  server.begin();
}

void loop() {
  
}

void handleBody(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
  request->send(200); // karsiya hemen istek atılmali yoksa server istegi hemen alamadigi icin resetleniyor
  /*
    for (size_t i = 0; i < len; i++) {
    Serial.write(data[i]);
    }
  */

  saveData("/data.txt", data, len);
}

String saveData(String file_path, uint8_t *data, size_t len) {

  fs::FS &fs = SD_MMC;
  File file = fs.open(file_path.c_str(), FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file in appending mode");
  }
  else {
    file.write(data, len); // payload (data), payload length
  }
  file.close();
}

void handleUploadFile(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  File UploadFile;
  if (!index) {
    fs::FS &fs = SD_MMC;
    String f = "/" + filename;
    UploadFile = fs.open(f.c_str(), FILE_WRITE); // calismaz ise basina / koymayi dene !!!
    //Serial.printf("UploadStart: %s\n", filename.c_str());
  }
  /*
    for(size_t i=0; i<len; i++){
    Serial.write(data[i]);
    }
  */
  if (UploadFile) UploadFile.write(data, len);

  if (final) {
    if (UploadFile)         // If the file was successfully created
    {
      UploadFile.close();   // Close the file again
      //Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);

    }
  }
}
