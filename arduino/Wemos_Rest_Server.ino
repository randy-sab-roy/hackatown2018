#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <aREST.h>

aREST rest = aREST();

const char *ssid = "Team GLaDOS";
const char *password = "1234pouet";

// The port to listen for incoming TCP connections
#define LISTEN_PORT 80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

//Definition of routes
int setAlertLevel(String);        //Sets general color
int level = 0;

void setup()
{

    // Start Serial
    Serial.begin(115200);
    
    // Declare routes
    rest.function("alertLevel",      setAlertLevel);

    // Give name & ID to the device (ID should be 6 characters long)
    rest.set_id("1");
    rest.set_name("Wemos_Rest_Server");

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.println(WiFi.localIP());

    // Set up mDNS responder:
    // - first argument is the domain name, in this example
    //   the fully-qualified domain name is "esp8266.local"
    // - second argument is the IP address to advertise
    //   we send our IP address on the WiFi network
    if (!MDNS.begin("wemos.local", WiFi.localIP()))
    {
        Serial.println("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");

    // Start TCP (HTTP) server
    server.begin();
    Serial.println("TCP server started");

    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", LISTEN_PORT);

    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(14, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(16, OUTPUT);
}

void loop()
{

    // Handle REST calls
    WiFiClient client = server.available();

    if (!client)
    {
        return;
    }

    while (!client.available())
    {
        writeLevel();
        delay(50);
    }
    rest.handle(client);
}

//Implementation of routes
int setAlertLevel(String request){
  level = request.toInt();

  level = constrain(level, 0, 16);
  return level;
}

void writeLevel(){
  digitalWrite(16, ( bitRead(level, 0) == 1 ? HIGH : LOW ) );
  digitalWrite(14, ( bitRead(level, 1) == 1 ? HIGH : LOW ) );
  digitalWrite(12, ( bitRead(level, 2) == 1 ? HIGH : LOW ) );
  digitalWrite(13, ( bitRead(level, 3) == 1 ? HIGH : LOW ) );
  digitalWrite(15, ( bitRead(level, 4) == 1 ? HIGH : LOW ) );
}

