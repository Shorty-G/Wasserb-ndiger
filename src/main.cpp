#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>


const char* ssid = "lul";
const char* password = "";

// Erstelle eine Instanz des WebServers auf Port 80
AsyncWebServer server(80);

void setup() {
  // Start der seriellen Kommunikation
  Serial.begin(115200);

  // Verbindung mit dem WLAN
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Verbindung zum WLAN wird hergestellt...");
  }
  Serial.println("Verbunden mit dem WLAN");

  // Definiere Routen
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html><body><h1>Hallo, Welt!</h1></body></html>");
  });

  // Start für den Webserver
  server.begin();
}

void loop() {
  //weitere Aufgaben für deinen Mikrocontroller 
}
