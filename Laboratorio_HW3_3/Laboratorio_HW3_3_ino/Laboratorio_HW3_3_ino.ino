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
  /**
   * setLedValue, ovvero il secondo argomento della funzione
   * subscribe serve ad associare una funzione al segnale di 
   * callback data dala notify di mqtt quando i dati sono
   * disponibili
   */
  Serial.begin(9600);
  Serial.print("Lab 3.3 Starting:");
}

void loop() {
  // put your main code here, to run repeatedly:

}
/**
 * penso sia difficile da interpretare per via della funzione di callback
 * non so come è inizializzata e creata 
 */
void setLedValue(const String& topic, const String& subtopic, const String& message){
  DeserializationError err = deserializeJson(doc_rec, message);
  if (err){
    Serial.print(F("DeserializeJson() failed with code "));
    Serial.println(err.c_str());
  }
  if (doc_rec["e"][0]["n"] == "led"){
    digitalWrite(pinLed, (int)doc_rec["e"][0]["v"]);
  }
}
