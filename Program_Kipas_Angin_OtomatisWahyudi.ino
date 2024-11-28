#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Setting WiFi
const char* ssid = "Kucing Santo";
const char* password = "kucingsanto2022";

// Setting DHT11 sensor
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Setting Relay
#define RELAY 13
int relayState = LOW;

// Setting Web Server
WebServer server(80);

void setup() {
  // Starting Serial Monitor
  Serial.begin(115200);
  
  // Initializing DHT11 Sensor
  dht.begin();
  
  // Initializing Relay
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, relayState);
  
  // Connecting to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Mengambil dan menampilkan alamat IP WiFi
  IPAddress localIP = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(localIP);

  // Starting Web Server
  server.on("/", handleRoot);
  server.on("/relay/on", handleRelayOn);
  server.on("/relay/off", handleRelayOff);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Web Server started");
}

void loop() {
  // Handling Web Server Requests
  server.handleClient();

  // Reading Temperature and Humidity from DHT11 Sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Checking if Temperature is Higher than 25 Celcius
  if (temperature > 39) {
    relayState = LOW;
    digitalWrite(RELAY, relayState);
  }
  else {
    relayState = HIGH;
    digitalWrite(RELAY, relayState);
  }

  // Logging Temperature and Humidity
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  // Delaying for 2 Seconds
  delay(2000);
}

void handleRoot() {
  // Sending Web Page with Current Relay State and Temperature
  String html = "<html><body><h1>Relay Status: ";
  if (relayState == LOW) {
    html += "ON";
  }
  else {
    html += "OFF";
  }
  html += "</h1><h2>Temperature: ";
  html += dht.readTemperature();
  html += " *C</h2>";
  html += "<form method='POST' action='/relay/on'><button>Turn On Relay</button></form>";
  html += "<form method='POST' action='/relay/off'><button>Turn Off Relay</button></form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleRelayOn() {
  // Turning On Relay and Redirecting to Root Page
  relayState = LOW;
  digitalWrite(RELAY, relayState);
  server.sendHeader("Location", String("/"), true);
  server.send(302, "text/plain", "");
}

void handleRelayOff() {
  // Turning Off Relay and Redirecting to Root Page
  relayState = HIGH;
  digitalWrite(RELAY, relayState);
  server.sendHeader("Location", String("/"), true);
  server.send(302, "text/plain", "");
}

void handleNotFound() {
  // Sending Error Page
  String html = "<html><body><h1>404 Not Found</h1><p>The requested URL ";
html += server.uri();
html += " was not found on this server.</p></body></html>";
server.send(404, "text/html", html);
}
