/**
 *    IOT laboratorio HW-3
 *    esercizio 1
 *    todo: realizzare sketch in grado di rispondere
 *          richieste http get ad un server gestendo 
 *          il led e riportando i dati rilevati dal
 *          sensore di temperatura
 */


#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>

const int capacity = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(4) + 40;
DynamicJsonDocument doc_snd(capacity);

BridgeServer server; 

const int ledPin = 11;
const int tempPin = A1;

float temp = 0.0;

const long int R0 = 100000;
const int beta = 4275;


void setup() {
  // put your setup code here, to run once:

  pinMode(tempPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  server.listenOnLocalhost();
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  temp = checkTemp();
  
  BridgeClient client = server.accept();
  
  if (client){
    process(client);
    client.stop();
  }

  delay(50);
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
 * funzione processa la comunicazione
 * in teoria la porta è la 80
 */
void process(BridgeClient client){

  String command = client.readStringUntil('/');
  command.trim();

  if (command == "led"){
    int val = client.parseInt();
    if (val == 0 || val == 1){
      digitalWrite(ledPin, val);
      printResponse(client, 200, senMlEncode(F("led"), val, F("")));
    } else {
      printResponse(client, 400, "");
    /**  
     *   print errori
     */
    }
  }
  else if (command == "tmp"){
    printResponse(client, 200, senMlEncode(F("tmp"), temp, F("C")));
    /**
     * gestione tmp
     */
  }
  else {
    printResponse(client, 404, "PAGE NOT FOUND");
    /**
     * gestione errori
     */
  }
}

/**
 * invia risposta al bridge
 */
void printResponse(BridgeClient client, int code, String body){
  client.println("Status: " + String(code));
  if (code = 200) {
    client.println(F("Content-type : application/json; charset=utf-8"));
    client.println();
    client.println(body);
  }
}

/**
 * crea il json
 */
String senMlEncode(String res, float v, String unit){
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
