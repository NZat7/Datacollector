
//code for the esp8266 01 to host a website and send data serially to the Arduino

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR_SSID"; //Replace with network name
const char* password = "YOUR_PASSWORD"; //Replace with network password

ESP8266WebServer server(80);

void handleRoot() {
  String html = "<html><body>";
  html += "<h2>DACO CONTROL</h2>";
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
