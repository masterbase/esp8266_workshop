/**
 * ESP_1OneWire.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

#define USE_SERIAL Serial
#define SSID "mrmcd2018"
#define PASSWORD "mrmcd2018"
#define MQTT_SERVER "mqtt.masterbase.at"
#define MQTT_USER "mrmcd"
#define MQTT_PASSWORD "VerySecurePassword"
#define MQTT_LED_TOPIC "mrmcd/led"
#define LEDPIN 2

ESP8266WiFiMulti WiFiMulti;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i=0;i<length;i++) {
        char receivedChar = (char)payload[i];
        Serial.print(receivedChar);
        if (strcmp(topic, MQTT_LED_TOPIC) == 0 ) {
            if (receivedChar == '0') {
                digitalWrite(LEDPIN, HIGH);
            } else if (receivedChar == '1') {
                digitalWrite(LEDPIN, LOW);
            }
        }
        Serial.println();
    }
}

bool mqttReconnect() {
    // Loop until we're reconnected
    if (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect

        if (mqttClient.connect(WiFi.macAddress().c_str(), MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("connected");
            // ... and subscribe to topic
            mqttClient.subscribe(MQTT_LED_TOPIC);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            //Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            //delay(5000);
            Serial.println();
        }
    }
    return mqttClient.connected();
}

void setup() {
    //setup LED GPIO
    digitalWrite(2, HIGH);
    pinMode(2, OUTPUT);
    
    //setup button GPIOs
    pinMode(D1, INPUT_PULLUP);
    pinMode(D2, INPUT_PULLUP);

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    /*for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }*/
    USE_SERIAL.print("[SETUP]\n");
    delay(10);
    USE_SERIAL.print("[WIFI] setup/connecting]\n");
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    
    mqttClient.setServer(MQTT_SERVER, 1883);
    mqttClient.setCallback(mqttCallback);

}

bool tempValid = false;
unsigned int tempTimestamp = micros();
String tempPayload;
unsigned int nextTemp = micros();

// waist memory but is faster
uint8_t button1_state = HIGH;
uint8_t button2_state = HIGH;

void loop() {
    if (mqttReconnect()) {
        if (button1_state == HIGH and digitalRead(D1) == LOW) {
            mqttClient.publish("mrmcd/button_demo/button", "1");
            USE_SERIAL.println(F("button 1 pushed"));
        }
        if (button2_state == HIGH and digitalRead(D2) == LOW) {
            mqttClient.publish("mrmcd/button_demo/button", "1");
            USE_SERIAL.println(F("button 2 pushed"));
        }
        button1_state = digitalRead(D1);
        button2_state = digitalRead(D2);
    }
       
    mqttClient.loop();
    delay(100);
}

