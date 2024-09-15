#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "ESP32_Access_Point";
const char* password = "your_PASSWORD";

// Create an instance of the server
WebServer server(80);

// Define the onboard LED pin
const int ledPin = 2; // GPIO2 is typically the onboard LED for ESP32

// HTML page content
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Web Terminal</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    h1 { color: #333; }
    input[type="text"] { padding: 10px; font-size: 16px; width: 80%; }
    button { padding: 10px 20px; font-size: 16px; }
    #output { margin-top: 20px; white-space: pre-wrap; text-align: left; }
  </style>
</head>
<body>
  <h1>ESP32 Web Terminal</h1>
  <input type="text" id="command" placeholder="Enter command">
  <button onclick="sendCommand()">Send</button>
  <div id="output"></div>
  <script>
    function sendCommand() {
      const command = document.getElementById('command').value;
      fetch(`/command?cmd=${encodeURIComponent(command)}`)
        .then(response => response.text())
        .then(data => {
          document.getElementById('output').innerText += data + '\\n';
        });
    }
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleCommand() {
  if (server.hasArg("cmd")) {
    String command = server.arg("cmd");
    String response;

    // Process the command here
    if (command == "blink") {
      response = "Blinking LED";
      digitalWrite(ledPin, !digitalRead(ledPin)); // Toggle the LED state
    } else {
      response = "Unknown command: " + command;
    }

    server.send(200, "text/plain", response);
  } else {
    server.send(400, "text/plain", "Command not found");
  }
}

void setup() {
  Serial.begin(115200);

  // Set up the ESP32 as an access point
  WiFi.softAP(ssid, password);

  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Print the IP address
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/command", handleCommand);
  server.begin();
}

void loop() {
  server.handleClient();

  // Blink the onboard LED every second
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {
    lastTime = currentTime;
    digitalWrite(ledPin, !digitalRead(ledPin)); // Toggle the LED state
  }
}