#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Web server and WebSocket server instances
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set up the web server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // Set up the WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started");
}

void loop() {
  server.handleClient();
  webSocket.loop();
}

void handleRoot() {
  String html = "<!DOCTYPE html>\
<html lang='en'>\
<head>\
  <meta charset='UTF-8'>\
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
  <title>ESP32 Web Terminal</title>\
  <style>\
    body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; text-align: center; }\
    #container { max-width: 600px; margin: 50px auto; padding: 20px; background-color: #fff; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }\
    h1 { color: #333; }\
    input[type='text'] { width: 80%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }\
    button { padding: 10px 20px; background-color: #28a745; color: #fff; border: none; border-radius: 5px; cursor: pointer; }\
    button:hover { background-color: #218838; }\
    #output { text-align: left; margin-top: 20px; background-color: #f8f9fa; padding: 10px; border-radius: 5px; white-space: pre-wrap; }\
  </style>\
</head>\
<body>\
  <div id='container'>\
    <h1>ESP32 Web Terminal</h1>\
    <input type='text' id='command' placeholder='Enter command'>\
    <button onclick='sendCommand()'>Send</button>\
    <div id='output'></div>\
  </div>\
  <script>\
    var ws = new WebSocket('ws://' + location.hostname + ':81/');\
    ws.onmessage = function(event) {\
      document.getElementById('output').innerText += event.data + '\\n';\
    };\
    function sendCommand() {\
      var command = document.getElementById('command').value;\
      if (command != '') {\
        ws.send(command);\
        document.getElementById('command').value = '';\
      }\
    }\
  </script>\
</body>\
</html>";
  server.send(200, "text/html", html);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_TEXT) {
    Serial.printf("[%u] Received: %s\n", num, payload);
    
    String message = "Received: " + String((char*)payload);
    webSocket.sendTXT(num, message);

    // Example: Echoing the command back (you can replace this with actual command handling)
    webSocket.sendTXT(num, "Command processed: " + String((char*)payload));
  }
}