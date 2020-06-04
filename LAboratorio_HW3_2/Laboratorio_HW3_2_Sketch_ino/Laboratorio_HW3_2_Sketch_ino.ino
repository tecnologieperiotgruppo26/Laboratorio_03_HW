/**
 *    IOT laboratorio HW-3
 *    esercizio 2
 *    todo: realizzare sketch in grado di effettuare
 *          richieste http post ad un server facendo
 *          un logging periodico dei dati del sensore 
 *          di temperatura.
 */


#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <ArduinoJson.h>
#include <Process.h>

const int capacity = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(4) + 40;
DynamicJsonDocument doc_snd(capacity);

BridgeServer server; 

const int tempPin = A1;

float temp = 0.0;

const long int R0 = 100000;
const int beta = 4275;

String jsonTemp = "";
String url = "192.168.1.69:9090";
/**
 * il mio indirizzo ip locale
 * 192.168.1.69
 */

void setup() {
  // put your setup code here, to run once:
  pinMode(tempPin, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  Serial.begin(9600);
  Serial.println("Lab 3.2 Starting:");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  temp = checkTemp();
  /**
   * ora devo lanciare la richiesta post al server cherrypy
   * quindi creo la stringa dati per poi passarla tramite richiesta
   * col metodo curl della shell
   */
  jsonTemp = senMlEncode("tmp", temp, "C");
  int postCallback = postRequest(jsonTemp);
  if (postCallback == 0){
    Serial.print("La POST Request è andata a buon fine con valore: ");
    Serial.println(postCallback);
  }
  else {
    Serial.print("POST Request ERROR VALUE: ");
    Serial.println(postCallback);
  }
  delay(5000);
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
  doc_snd["e"][0]["v"] = int(v);
  
  String output;
  serializeJson(doc_snd, output);
  Serial.println(output);
  return output;
}

/**
 * mando la richiesta curl al processore 
 * che la eseguirà per fare richieste al server 
 */
int postRequest(String data){
  Process p;
  p.begin("curl");
  p.addParameter("-H");
  p.addParameter("Content-Type: application/json");
  p.addParameter("-X");
  p.addParameter("POST");
  p.addParameter("-d");
  p.addParameter(data);
  p.addParameter(url);
  p.run();

  return p.exitValue();
}
