#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

BridgeServer server; 

void setup() {
  // put your setup code here, to run once:
  pinMode(D13, OUTPUT);
  digitalWrite(D13, LOW);
  Bridge.begin();
  digitalWrite(D13, HIGH);
  server.listenOnLocalHost();
  server.begin()
}

void loop() {
  // put your main code here, to run repeatedly:
  BridgeClient client = server.accept();

  if (client){
    process(client);
    client.stop();
  }

  delay(50);
}

void process
