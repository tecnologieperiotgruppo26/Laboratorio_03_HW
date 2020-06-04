import paho.mqtt.client as PahoMQTT
import time

class MyPublisher:
    def __init__(self, clientID, broker):
        self.clientID = clientID

        #creiamo un'istanza di paho client
        self._paho_mqtt = PahoMQTT.Client(self.clientID)
        #registriamo la callback
        self._paho_mqtt.on_connect = self.myOnConnect
        self.messageBroker = broker

    def start(self):
        #gestiamo la connessione col broker
        self._paho_mqtt.connect(self.messageBroker, 1883)
        self._paho_mqtt.loop_start()

    def stop(self):
        self._paho_mqtt.loop_stop()
        self._paho_mqtt.disconnect()

    def myPublish(self, topic, message):
        # publichiamo un messagio relativo ad un certo topic. l'int inviato come parametro
        # indica il grado di QOS relativo a quella comunicazione
        self._paho_mqtt.publish(topic, message, 2)

    def myOnConnect(self, paho_mqtt, userdata, flags, rc):
        #conferma della connessione
        print("Connected to %s with result code: %d" % (self.messageBroker, rc))

class MySubscriber:
    def __init__(self, clientID,topic,broker):
        self.clientID = clientID
        #creo l'istanza del client
        self._paho_mqtt = PahoMQTT.Client(clientID, False)
        # registro la calback
        self._paho_mqtt.on_connect = self.myOnConnect
        self._paho_mqtt.on_message = self.myOnMessageReceived
        self.topic = topic
        self.messageBroker = broker

    def start (self):
        #gestione connessione col broker
        self._paho_mqtt.connect(self.messageBroker, 1883)
        self._paho_mqtt.loop_start()
        # sottoscrizione ad un topic
        self._paho_mqtt.subscribe(self.topic, 2)

    def stop (self):
        self._paho_mqtt.unsubscribe(self.topic)
        self._paho_mqtt.loop_stop()
        self._paho_mqtt.disconnect()

    def myOnConnect (self, paho_mqtt, userdata, flags, rc):
        print ("Connected to %s with result code: %d" % (self.messageBroker, rc))

    def myOnMessageReceived (self, paho_mqtt , userdata, msg):
        # A new message is received
        print ("Topic:'" + msg.topic+"', QoS: '"+str(msg.qos)+"' Message: '"+str(msg.payload) + "'")

    """
    separazione fra i due progetti, quelli sopra è roba data a prescindere, 
    questa sotto invece per gli esercizi 1 e 2 a quanto pare
    
    """

class MyMQTT:
    #praicamente da quel che ho capito sta classe è un qualcosa di più specifico rispetto ad un publisher
    #creato un po meglio
    def __init__(self, clientID, broker, port, notifier):
        self.broker = broker
        self.port = port
        self.notifier = notifier
        self.clientID = clientID
        self._topic = ""
        self._isSubscriber = False

        # create an instance of paho.mqtt.client
        self._paho_mqtt = PahoMQTT.Client(clientID, False)
        # register the callback
        self._paho_mqtt.on_connect = self.myOnConnect
        self._paho_mqtt.on_message = self.myOnMessageReceived

    def myOnConnect(self, paho_mqtt, userdata, flags, rc):
        print("Connected to %s with result code: %d" % (self.broker, rc))

    def myOnMessageReceived(self, paho_mqtt, userdata, msg):
        # A new message is received
        self.notifier.notify(msg.topic, str(msg.payload.decode("utf-8")))

    def mySubscribe(self, topic):
        # if needed, you can do some computation or error-check before subscribing
        print("subscribing to %s" % (topic))
        # subscribe for a topic
        self._paho_mqtt.subscribe(topic, 2)
        # just to remember that it works also as a subscriber
        self._isSubscriber = True
        self._topic = topic

    def start(self):
        # manage connection to broker
        self._paho_mqtt.connect(self.broker, self.port)
        self._paho_mqtt.loop_start()

    def stop(self):
        if (self._isSubscriber):
            # remember to unsuscribe if it is working also as subscriber
            self._paho_mqtt.unsubscribe(self._topic)
        self._paho_mqtt.loop_stop()
        self._paho_mqtt.disconnect()

    def myPublish(self, topic, message):
        # publichiamo un messagio relativo ad un certo topic. l'int inviato come parametro
        # indica il grado di QOS relativo a quella comunicazione
        self._paho_mqtt.publish(topic, message, 2)

class DoSomething():
    def __init__(self, clientID):
        # create an instance of MyMQTT class
        self.clientID = clientID
        self.myMqttClient = MyMQTT(self.clientID, "test.mosquitto.org", 1883, self)

    def run(self):

        # if needed, perform some other actions befor starting the mqtt communication
        print("running %s" % (self.clientID))
        self.myMqttClient.start()

    def end(self):

        # if needed, perform some other actions befor ending the software
        print("ending %s" % (self.clientID))
        self.myMqttClient.stop()

    def notify(self, topic, msg):

        # manage here your received message. You can perform some error-check here
        print("received '%s' under topic '%s'" % (msg, topic))

    def mySubscribe(self, topic : str):
        self.myMqttClient.mySubscribe(topic)
        print("Mi sono sottoscritto al topic : {}".format(topic))

    def mySecondPublish(self, topic, msg):
        self.myMqttClient.myPublish(topic, msg)

if __name__ == '__main__':
    client = DoSomething("C1")
    client.run()
    topic = "/tiot/26/tmp"
    #input("Inserire un topic a cui sottoscriversi per poi mandare i messaggi")
    client.mySubscribe(topic)
    while(True):
        command = input("Digita q per uscire, se vuoi")
        if command != 'q':
            statusLed = input("Inseire lo stato della lampadina e vedi se te lo fa leggere come subscriber")
            client.mySecondPublish("/tiot/26/led", statusLed)
        else:
            break

    client.end()