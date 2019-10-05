/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "credentials.h"

/* commands */
enum command{
    RELAY_OFF,
    RELAY_ON
};

enum relay_state{
    OFF,
    ON
};

int state = OFF;

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVER_PORT, MQTT_USER, MQTT_PASS);

/****************************** Feeds ***************************************/

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe stove_cmd = Adafruit_MQTT_Subscribe(&mqtt, "/stove/cmd");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription to control topic 
  mqtt.subscribe(&stove_cmd);
}

void loop() {
  char cmd;
  char* cmd_str;
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &stove_cmd) {
      
      //Serial.print(F("Got: "));
      //Serial.println((char *)stove_cmd.lastread);
      cmd_str = (char *)stove_cmd.lastread;
      cmd_stove(atoi(&cmd_str[0]));
    }
  }

  // ping the server to keep the mqtt connection alive
  if(! mqtt.ping()) {
    Serial.println("Disconnecting");
    mqtt.disconnect();
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void turn_off_stove(){
  if(state == OFF){
    Serial.println("Stove is already off! What are you doing?");
  } else {
    Serial.println("Turning off the stove");
    state = OFF;
  }
}

void turn_on_stove(){
  if(state == ON){
    Serial.println("Stove is already on! What are you doing?");
  } else {
    Serial.println("Turning on the stove");
    state = ON;
  }
}

void cmd_stove(int cmd){
  /* Control the Relay here */
  switch(cmd){
    case RELAY_OFF:
      turn_off_stove();
      break;
    case RELAY_ON:
      turn_on_stove();
      break;
    default:
      Serial.println("Unknown command");
  }
}
