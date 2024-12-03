#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// WiFi Configuration
#define WLAN_SSID       "..." // Replace with your WiFi SSID
#define WLAN_PASS       "..." // Replace with your WiFi password

// Adafruit IO Configuration
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883 // Use 8883 for SSL
#define AIO_USERNAME    "..." // Replace with your Adafruit IO username
#define AIO_KEY         "..." // Replace with your Adafruit IO key

// Command Configuration
#define TEMPERATURE_CMD_LENGTH 5
#define BEGIN_SYMBOL '!'
#define TERMINATE_SYMBOL '#'
#define MQTT_INTERVAL 30000  // 30 seconds
#define BLINK_INTERVAL 500   // 500ms

// Pin Definitions
const int LED_PIN = 2;  // LED indicator
const int BUSY_PIN = 5; // Busy pin for state indication

// MQTT Setup
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish temperature_pub = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");

// Variables for temperature processing
char temperature_cmd[] = "TEMP:";
int cmd_index = 0;
bool is_cmd_begin = false;
bool is_data_begin = false;
char temperature_buffer[20];
int data_index = 0;
unsigned long last_mqtt_time = 0;
unsigned long last_blink_time = 0;

// Function to connect and reconnect as necessary to the MQTT server
void MQTT_connect() {
    if (mqtt.connected()) {
        return;
    }

    Serial.print("Connecting to MQTT... ");
    while (mqtt.connect() != 0) {
        Serial.println("Failed, retrying in 5 seconds...");
        mqtt.disconnect();
        delay(5000);
    }
    Serial.println("MQTT Connected!");
}

void setup() {
    // Initialize Serial communication
    Serial.begin(115200);
    delay(10);
    
    // Configure pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUSY_PIN, OUTPUT);
    digitalWrite(BUSY_PIN, HIGH);  // Set busy during setup

    // Connect to WiFi
    Serial.println(); Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);

    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Blink LED while connecting
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.println("IP address: "); 
    Serial.println(WiFi.localIP());

    // Initial MQTT connection
    MQTT_connect();

    // Setup complete
    digitalWrite(BUSY_PIN, LOW);
}

void loop() {
    // Ensure the connection to MQTT is alive
    MQTT_connect();

    // Process MQTT packets
    mqtt.processPackets(10);

    // Read and process serial data 
    if (Serial.available()) {
        int incoming_byte = Serial.read();
        
        if (is_cmd_begin) {
            if (is_data_begin) {
                if (TERMINATE_SYMBOL == incoming_byte) {
                    if (data_index > 0) {
                        // Null terminate the string
                        temperature_buffer[data_index] = '\0';
                        float temp_value = atof(temperature_buffer);
                        
                        // Publish temperature if interval has passed
                        if (millis() - last_mqtt_time > MQTT_INTERVAL) {
                            digitalWrite(BUSY_PIN, HIGH);  // Set busy during publish
                            if (temperature_pub.publish(temp_value)) { // Corrected variable name
                                Serial.print("Published temperature: ");
                                Serial.println(temp_value);
                                last_mqtt_time = millis();
                            } else {
                                Serial.println("Failed to publish temperature");
                            }
                            digitalWrite(BUSY_PIN, LOW);  // Clear busy after publish
                        }
                    }
                    is_data_begin = false;
                    is_cmd_begin = false;
                } else {
                    // Only accept numeric values and decimal point
                    if ((incoming_byte >= '0' && incoming_byte <= '9') || incoming_byte == '.') {
                        temperature_buffer[data_index] = incoming_byte;
                        data_index++;
                        if (data_index >= sizeof(temperature_buffer) - 1) {
                            Serial.println("Temperature value too long");
                            is_data_begin = false;
                            is_cmd_begin = false;
                        }
                    } else {
                        Serial.println("Temperature must be a number");
                        is_data_begin = false;
                        is_cmd_begin = false;
                    }
                }
            } else {
                // Verify command format
                if (incoming_byte != temperature_cmd[cmd_index]) {
                    Serial.print("Wrong command format: ");
                    Serial.println((char)incoming_byte);
                    is_cmd_begin = false;
                } else {
                    cmd_index++;
                    if (TEMPERATURE_CMD_LENGTH == cmd_index) {
                        is_data_begin = true;
                        data_index = 0;
                    }
                }
            }
        } else if (incoming_byte == BEGIN_SYMBOL) {
            is_cmd_begin = true;
            cmd_index = 0;
        }
    }

    // LED indicator using millis() instead of delay
    if (millis() - last_blink_time > BLINK_INTERVAL) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        last_blink_time = millis();
    }
}