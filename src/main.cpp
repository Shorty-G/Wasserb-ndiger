#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>
#include <TimeLib.h>

const char* ssid = "";
const char* password = "";

AsyncWebServer server(80);
WebSocketsServer webSocket(81);

const int aktualisierungsIntervall = 5000;  // Intervall in Millisekunden (hier 5 Sekunden)
unsigned long letzteAktualisierung = 0;    // Variable für die Zeit der letzten Aktualisierung

int humidity = 0;
int waterLvl = 0;

void aktualisiereDaten() {
  // Hier die Logik für die Aktualisierung der Daten einfügen
  // Zum Beispiel: humidity = analogRead(A0); oder andere Sensordaten aktualisieren
  
  if (humidity >= 3 /*ka welcher Wert hier sein soll, kommt auf den Output aus dem Sensor an*/) {
    String dataToSend = "{\"humidity\":" + String(humidity) + ",\"waterLevel\":" + String(waterLvl) + "}";
    webSocket.broadcastTXT(dataToSend);
  }

  letzteAktualisierung = millis();  // Setze die Zeit der letzten Aktualisierung auf die aktuelle Zeit
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) {
    Serial.println("WebSockets-Nachricht empfangen: " + String((char*)payload));
  }
}

void setup() {
  Serial.begin(9600);

  if (!SPIFFS.begin()) {
    Serial.println("Fehler beim Initialisieren von SPIFFS");
    return;
  }

  File ourHTML = SPIFFS.open("/index.html", "r");
  if (!ourHTML) {
    Serial.println("Fehler beim Öffnen der HTML-Datei");
    return;
  }

  String htmlContent = ourHTML.readString();
  ourHTML.close();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Verbindung zum WLAN wird hergestellt...");
  }

  Serial.println("Verbunden mit dem WLAN");
  Serial.println("IP-Adresse: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, [htmlContent](AsyncWebServerRequest *request) {
    request->send(200, "text/html", htmlContent);
  });

  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);  // Funktion zum Behandeln von WebSocket-Ereignissen hinzufügen
}

void loop() {
  webSocket.loop();  // Websocket im Loop aktualisieren

  unsigned long jetzt = millis();  // Aktuelle Zeit in Millisekunden

  if (jetzt - letzteAktualisierung >= aktualisierungsIntervall) {
    aktualisiereDaten();  // Aktualisiere die Daten im Intervall
  }

  // Weitere Logik im Loop, falls erforderlich
}
