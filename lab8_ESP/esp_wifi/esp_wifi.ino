#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//Wifi name
// #define WLAN_SSID       "..."
#define WLAN_SSID       "..."
//Wifi password
// #define WLAN_PASS       "..."
#define WLAN_PASS       "..."

//setup Adafruit
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
//fill your username                   
// #define AIO_USERNAME    "..."
#define AIO_USERNAME    "dgminh"
//fill your key
// #define AIO_KEY         "..."
#define AIO_KEY         "aio_RpUj009ZxQ39H9aaKJUkFlODtjAh"

//setup MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

//setup publish
Adafruit_MQTT_Publish temperature_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");

//setup subcribe
Adafruit_MQTT_Subscribe temperature_sub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/temperature", MQTT_QOS_1);

//pin Definitions
int led_pin = 2;  // LED indicator
int busy_pin = 5; // Busy pin for state indication

//variables
String temperature_data = "";
unsigned long last_publish_time = 0;
const unsigned long publish_interval = 30000; // 30 seconds

void setup() {
  // put your setup code here, to run once:
  //set pin 2,5 as OUTPUT
  pinMode(led_pin, OUTPUT);
  pinMode(busy_pin, OUTPUT);
  //set busy pin HIGH
  digitalWrite(busy_pin, HIGH);

  Serial.begin(115200);

  //connect Wifi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //toggle LED during connection
    digitalWrite(led_pin, !digitalRead(led_pin));
  }

  //connect MQTT
  while (mqtt.connect() != 0) { 
    mqtt.disconnect();
    delay(500);
  }

  //finish setup, set busy pin LOW
  digitalWrite(busy_pin, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  //receive packet
  mqtt.processPackets(10);
  
  //read temperature data from Serial
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '#') { // End of temperature string
      if (temperature_data.startsWith("!TEMP:")) {
        float temperature = temperature_data.substring(6).toFloat();
        Serial.println("Temperature: " + String(temperature));

        //publish temperature to Adafruit IO
        if (millis() - last_publish_time > publish_interval) {
          if (temperature_pub.publish(temperature)) {
            Serial.println("Published temperature: " + String(temperature));
            last_publish_time = millis();
          } else {
            Serial.println("Failed to publish temperature");
          }
        }
      }
      temperature_data = ""; //reset buffer
    } else {
      temperature_data += c; //build temperature string
    }
  }

  //lED indicator for normal operation
  static unsigned long led_last_toggle = 0;
  if (millis() - led_last_toggle > 500) { //toggle every 500 ms
    digitalWrite(led_pin, !digitalRead(led_pin));
    led_last_toggle = millis();
  }
}
