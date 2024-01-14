#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <AsyncJson.h>

const char *ssid = "";
const char *password = "";

AsyncWebServer server(80);

int pump = 23;
int soilSens = 33;
int waterSens = 32;
int soilMoistValue = 0;
int waterTankLevel = 0;

void setup()
{
  SPIFFS.begin();
  Serial.begin(9600);

  pinMode(pump, OUTPUT);
  pinMode(soilSens, INPUT);
  pinMode(waterSens, INPUT);
  digitalWrite(pump, LOW);

  if (!SPIFFS.begin())
  {
    Serial.println("Fehler beim Initialisieren von SPIFFS");
    return;
  }

  File ourHTML = SPIFFS.open("/index.html", "r");
  if (!ourHTML)
  {
    Serial.println("Fehler beim Öffnen der HTML-Datei");
    return;
  }

  String htmlContent = ourHTML.readString();
  ourHTML.close();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Verbindung zum WLAN wird hergestellt...");
  }

  Serial.println("Verbunden mit dem WLAN");
  Serial.println("IP-Adresse: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, [htmlContent](AsyncWebServerRequest *request)
            { request->send(200, "text/html", htmlContent); });
  server.on(
      "/pump", HTTP_POST,
      [](AsyncWebServerRequest *request)
      {
        digitalWrite(pump, HIGH);

        request->send(200, "text/plain", "<h1>Pump started</h1>");
      },
      NULL);
  server.on(
      "/data", HTTP_POST,
      [](AsyncWebServerRequest *request)
      {
        DynamicJsonDocument doc(1024);
        doc["sensor"][0] = "moist";
        doc["data"][0] = soilMoistValue;
        doc["sensor"][1] = "water";
        doc["data"][1] = waterTankLevel;

        String x;
        serializeJson(doc, x);

        request->send(200, "application/json", x);
      },
      NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         size_t index, size_t total)
      {
        if (len != total)
        {
          request->send(400, "text/html", "<h1>Paginagion kickt</h1>");
          return;
        }
        // processing of stuff
        request->send(200, "text/html", "<h1>Processed</h1>");
      });

  server.begin();
}

void loop()
{

  soilMoistValue = analogRead(soilSens);
  waterTankLevel = analogRead(waterSens);
  if (soilMoistValue > 2800 && waterTankLevel > 700)
  {
    digitalWrite(pump, HIGH);
  }
  else if (soilMoistValue < 2100 | waterTankLevel < 700)
  {
    digitalWrite(pump, LOW);
  }

  Serial.print("Soil Moist: ");
  Serial.print(soilMoistValue);
  Serial.print("Water Level: ");
  Serial.print(waterTankLevel);
  delay(5000);
}
