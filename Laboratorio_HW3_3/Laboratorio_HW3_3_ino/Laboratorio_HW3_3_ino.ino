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

const int ledPin = 11;
const int tempPin = A1;
float temp = 0.0;
const long int R0 = 100000;
const int beta = 4275;

String myBaseTopicLed = "/tiot/26/led";
String myBaseTopicTmp = "/tiot/26/tmp";

String jsonTemp = "";

void setup() {
  // put your setup code here, to run once:
  pinMode(tempPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  mqtt.begin("85.119.83.194", 1883);
  mqtt.subscribe(myBaseTopicLed, setLedValue);
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
  mqtt.monitor();
  /**
   * monitor attiva la comunicazione in arrivo dai topic
   * a cui si è iscritti, serve a chiamare la callback setLedValue
   */
   temp = checkTemp();
   jsonTemp = senMlEncode("tmp", temp, "C");
   mqtt.publish(myBaseTopicTmp, jsonTemp);
   Serial.println("published tmp on topic");
   delay(5000);
}
/**
 * penso sia difficile da interpretare per via della funzione di callback
 * 
 */
void setLedValue(const String& topic, const String& subtopic, const String& message){
  DeserializationError err = deserializeJson(doc_rec, message);
  if (err){
    Serial.print(F("DeserializeJson() failed with code "));
    Serial.println(err.c_str());
  }
  String topicCompleto = String(topic)+String(subtopic);
  if (topicCompleto == myBaseTopicLed){
    if (doc_rec["e"][0]["n"] == "led"){
      if(doc_rec["e"][0]["v"]==0 || doc_rec["e"][0]["v"]==1){
        digitalWrite(ledPin, (int)doc_rec["e"][0]["v"]);  
      }
      else{
        Serial.println("Valore errato del led");
      }
    } 
    else {
      Serial.println("Errore nel json, mi aspetto un messaggio con n = led!"); 
    }
  }
  else {
    Serial.println("ti sei iscritto al topic sbagliato. riprogrammare la scheda");
  }
  
}



/**
 * funzione rileva temperatura
 */
float checkTemp(){
  int vDigit = analogRead(tempPin);
  /**
   * Calcolo il valore di R, successivamente
   * uso il datasheet per ricavare le formule di conversione e 
   * calcolo T, per poi convertire in Celsius
  */
  float R = ((1023.0/vDigit)-1.0);
  R = R0*R;
  float loga = log(R/R0)/beta;
  float TK = 1.0/((loga)+(1.0/298.15));
  float TC = TK-273.15;
  return TC;
}

/**
 * crea il json
 */
String senMlEncode(String res, float v, String unit){
  /**
   * params:  res = risorsa in uso. in questo caso sarà 
   *                sempre la temperatura
   *          v = valore della risorsa. la prendo dalla 
   *              checktemp tramite la variabile temp
   *              (si potrebbe semplificare per un caso reale
   *              ma preferisco mantenere leggibilità)
   *          unit = unità di misura del valore della risorsa
   */
  doc_snd.clear();
  doc_snd["bn"] = "Yun";
  doc_snd["e"][0]["n"] = res;
  if (unit != ""){
    doc_snd["e"][0]["u"] = unit;
  } else {
    doc_snd["e"][0]["u"] = (char*)NULL;
  }
  doc_snd["e"][0]["t"] = millis()/1000;
  doc_snd["e"][0]["v"] = v;
  
  String output;
  serializeJson(doc_snd, output);
  return output;
}
