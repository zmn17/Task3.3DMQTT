#include <WiFiNINA.h>
#include <PubSubClient.h>

// Define pins
const int trigPin = 12;
const int echoPin = 4;
const int ledPin = 13;

// Define WiFi credentials
const char* ssid = "Not-Connected...";
const char* password = "ZH9FagdbAdXMNpk3";

// Define MQTT broker details
const char* broker = "broker.emqx.io";
const int port = 1883;
const char* waveTopic = "SIT210/wave";
const char* patTopic = "SIT210/pat";
const char* yourName = "Zamin";

// Define variables
long duration;
int distance;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Connect to WiFi
  connectWiFi();

  // Set MQTT server
  client.setServer(broker, port);
  client.setCallback(callback);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Ultrasonic sensor measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Publish message on wave detection
  if (distance > 100) { 
    client.publish(waveTopic, yourName);
    Serial.println("Wave detected! Published message.");
    blinkLED(3);
    Serial.println("Distance wave detected: " + String(distance));
  }

  // Publish message on pat detection
  if (distance < 20) {
    client.publish(patTopic, yourName);
    Serial.println("Pat detected! Published message.");
    blinkLED(5);
    Serial.println("Distance pat detected: " + String(distance));
  }

  // Check for incoming messages
  if (client.connected()) { // Check connection before calling functions
    client.loop(); // Ensure the client keeps processing messages
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      client.subscribe(waveTopic);
      client.subscribe(patTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void connectWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void blinkLED(int numTimes) {
  for (int i = 0; i < numTimes; i++){
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
