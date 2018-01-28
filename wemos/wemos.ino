#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

//relays
byte rel1ON[] = {0xA0, 0x01, 0x01, 0xA2};  //Hex command to send to serial for open relay
byte rel1OFF[] = {0xA0, 0x01, 0x00, 0xA1}; //Hex command to send to serial for close relay
byte rel2ON[] = {0xA0, 0x02, 0x01, 0xA3};  //Hex command to send to serial for open relay
byte rel2OFF[] = {0xA0, 0x02, 0x00, 0xA2}; //Hex command to send to serial for close relay

// prototypes
boolean connectWifi();

//on/off callbacks 
bool Relay1On();
bool Relay2Off();
bool kitchenLightsOn();
bool kitchenLightsOff();

// Change this before you flash
const char* ssid = "MyWiFiSSID";
const char* password = "MyWiFiPassword";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *relay1 = NULL;
Switch *relay2 = NULL;

bool isRelay1On = false;
bool isRelay2On = false;

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your switches here. Max 10
    // Format: Alexa invocation name, local port no, on callback, off callback
    relay1 = new Switch("Kitchen Fan", 80, Relay1On, Relay1Off);
    relay2 = new Switch("kitchen lights", 81, Relay2On, Relay2Off);

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*relay1);
    upnpBroadcastResponder.addDevice(*relay2);
  }
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      relay1->serverLoop();
      relay2->serverLoop();
	 }
}

bool Relay1On() {
    Serial.println("Switch 1 turn on ...");
    Serial.write(rel1ON, sizeof(rel1ON));     // turns the relay ON
    isRelay1On = true;    
    return isRelay1On;
}

bool Relay1Off() {
    Serial.println("Switch 1 turn off ...");
    Serial.write(rel1OFF, sizeof(rel1OFF));   // turns the relay OFF
    isRelay1On = false;
    return isRelay1On;
}

bool Relay2On() {
    Serial.println("Switch 2 turn on ...");
    Serial.write(rel2ON, sizeof(rel2ON));
    isRelay2On = true;
    return isRelay2On;
}

bool Relay2Off() {
  Serial.println("Switch 2 turn off ...");
  Serial.write(rel2OFF, sizeof(rel2OFF));
  isRelay2On = false;
  return isRelay2On;
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}
