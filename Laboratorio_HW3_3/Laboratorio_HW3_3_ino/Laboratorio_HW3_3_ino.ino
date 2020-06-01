/**
 *    IOT laboratorio HW-3
 *    esercizio 3
 *    todo: realizzare sketch in grado di comunicare
 *          tramite MQTT sia come publisher che come
 *          subscriber
 */

#include <MQTTclient.h>

#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>
#include <Process.h>

const int capacity = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(4) + 40;
DynamicJsonDocument doc_snd(capacity);
DynamicJsonDocument doc_rec(capacity);

const int tempPin = A1;
float temp = 0.0;
const long int R0 = 100000;
const int beta = 4275;

String myBaseTopic = "tiot/26";

void setup() {
  // put your setup code here, to run once:
  pinMode(tempPin, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  mqtt.begin("test.mosquitto.org", 1883);
  mqtt.subscribe(myBaseTopic + String("/led"), setLedValue);
  Serial.begin(9600);
  Serial.print("Lab 3.3 Starting:");
}

void loop() {
  // put your main code here, to run repeatedly:

}

void setLedValue(const String&
