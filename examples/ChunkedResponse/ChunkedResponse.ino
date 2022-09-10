#include <WiFi.h>
#include <WiFiClient.h>
#include <ChunkedWebServer.h>
#include <ESPmDNS.h>


WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp32!");
  digitalWrite(led, 0);
}
void handleChunked() {
  server.beginChunkedResponse("text/html");
  server.sendChunk("<h1>The server is up!</h1>");
  server.sendChunk("This server supports chunked responses<br>");
  for (int i =0; i < 10; i++) {
      server.sendChunk("Chunk ");
      server.sendChunk(String(i + 1) + "/10<br>");
      delay(100);
  }
  server.endChunkedResponse();
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("esp32ap", "");
  Serial.println("");


  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/chunked", handleChunked);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}
