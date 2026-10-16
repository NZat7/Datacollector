#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

ESP8266WebServer server(80);

void handleRoot() {
  String html = "<html><body>";
  html += "<h2>ESP-01 LED Sequence Controller</h2>";
  html += "<form action='/run' method='POST'>";
  html += "Sequence (e.g., 10101): <input type='text' name='seq'>";
  html += "<input type='submit' value='Send'>";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleRun() {
  if (server.hasArg("seq")) {
    String sequence = server.arg("seq");
    // Send the sequence out over Serial to the Arduino
    Serial.println(sequence);
    server.send(200, "text/html", "Sequence sent: " + sequence + "<br><a href='/'>Back</a>");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  server.on("/", handleRoot);
  server.on("/run", HTTP_POST, handleRun);
  server.begin();
}

void loop() {
  server.handleClient();
}