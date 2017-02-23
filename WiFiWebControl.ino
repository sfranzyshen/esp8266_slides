/*
 *  This sketch demonstrates how to set up a simple HTTP server.
 *  The server will set the GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the LED_BUILTIN low,
 *    http://server_ip/gpio/1 will set the LED_BUILTIN high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */
// Include the WiFi library
#include <ESP8266WiFi.h>

// Define what pin to use for the led
//#define LED_BUILTIN 2 // The blue LED on the ESP-12 module is connected to GPIO2


// Setup variables
const char* ssid = "SSID"; // case sensitive
const char* password = "PASSWORD"; // case sensitive
const char* MyHostname = "MYNAME"; // not case sensitive

int val = 1; // the blue LED is active low (connected to Vcc and sinks through the chip to ground) 
             // so setting a logical value of 0 will light it up.

// Create an instance of the server class
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  // Start serial port
  Serial.begin(115200);
  delay(500);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());
  WiFi.mode(WIFI_STA); // We set to station mode only (STA, STA_AP, AP)
  WiFi.hostname(MyHostname); // Set the dhcp hostname (if your router supports it)
  delay(500);
  WiFi.begin(ssid, password); // Start WiFi

  while (WiFi.status() != WL_CONNECTED) { // Wait for client connection
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  
  server.begin(); // Start the server
  Serial.println("Server started");

  // Print the IP address
  Serial.printf("Hostname: %s - ", WiFi.hostname().c_str());
  Serial.println(WiFi.localIP());

  // Stop serial port
  delay(500);
  Serial.end();

  // prepare GPIO LED_BUILTIN for output mode
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, val); 
  

}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  client.flush();
  
  // Match the request
  if (req.indexOf("/gpio/0") != -1) val = 1;
  if (req.indexOf("/gpio/1") != -1) val = 0;

  // Set LED_BUILTIN according to the request
  digitalWrite(LED_BUILTIN, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val)?"low":"high";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(10);
  client.stop();

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

