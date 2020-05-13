#include "DHT.h"
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"
#include "Adafruit_CCS811.h"

#define WIFI_AP "Nav6-A"
#define WIFI_PASSWORD "nav6ap123"

#define TOKEN "Arduino001"

// DHT
#define DHTPIN 4
#define DHTTYPE DHT22

char thingsboardServer[] = "demo.thingsboard.io";

// Initialize the Ethernet client object
WiFiEspClient espClient;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

PubSubClient client(espClient);

SoftwareSerial soft(2, 3); // RX, TX

int status = WL_IDLE_STATUS;
unsigned long lastSend;

Adafruit_CCS811 ccs;

void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  
  dht.begin();
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
  }
}

void loop() {
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 15000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  client.loop();
}


void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  //read CSS811 sensor
  float co2 = ccs.geteCO2();
  float voc = ccs.getTVOC();
  

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

if(ccs.available()){
    if(!ccs.readData()){
      Serial.print("CO2: ");
      Serial.print(co2);
      Serial.print("ppm, TVOC: ");
      Serial.print(voc);
      Serial.print("ppb");
    }
    else{
      Serial.println("ERROR!");
      while(1);
    }
  }
                                                                                                                                                                                                                                                    
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  
  String temperature = String(t);
  String humidity = String(h);
  String CO2 = String(co2);
  String VOC = String(voc);
  

  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
  Serial.print( temperature ); Serial.print( "," );
  Serial.print( humidity );Serial.print( "," );
  Serial.print( CO2 ); Serial.print( "," );
  Serial.print( VOC );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"humidity\":"; payload += humidity;payload += ",";
  payload += "\"CO2\":"; payload += CO2; payload += ",";
  payload += "\"VOC\":"; payload += VOC;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
}

void InitWiFi()
{
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Arduino Uno Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}


