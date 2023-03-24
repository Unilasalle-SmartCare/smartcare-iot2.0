#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>

//ULTRASSONIC
Ultrasonic ultrassom(D8,D7); // define o nome do sensor(ultrassom) 
//e onde esta ligado o trig(8) e o echo(7) respectivamente
//float distancia; // Variável para receber a distância

#define sensorTrigPin    D7
#define sensorEchoPin    D6

// WiFi
char *ssid = "SmartCare"; // Enter your WiFi name
char *password = "smartcare";  // Enter WiFi password

// MQTT Broker
char *mqtt_broker = "192.168.0.102";
char *topic = "home/sensor/BTN-01";
char *mqtt_username = "sensor01";
char *mqtt_password = "smartcare";
int mqtt_port = 1883;

int pinMaster = 13;//D11
int x = 1;
int buttonState = 0;
int buttonPin = 15;//D10

float old_d = 0;
int t = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(sensorTrigPin, OUTPUT);
  pinMode(sensorEchoPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  // Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
 WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Conectando ao WiFi..");
  }
  Serial.println("Conectado à rede WiFi");
  
  //conectando ao broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("O client %s tenta connectar-se ao Broker... \n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Conectado ao Broker");
      } else {
          Serial.print("Falhou com estado: ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  
  // publish and subscribe
  // client.publish(topic, "hello emqx");
 // client.subscribe(topic);

  //Define both button and LED I/O
  pinMode(pinMaster , OUTPUT);//Define ledPin (pino 10) como saída
  //pinMode(buttonPin , INPUT);//Define buttonPin (pino 7) como entrada
  pinMode(buttonPin , INPUT); // ultrassom
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}
const char msg[] PROGMEM = "{\"msg\":";
char msg2[] = ",\"dispositivo\": \"BTN-01\"}";
   
void loop() {
   client.loop();

   //distancia = ultrassom.Ranging(CM);// ultrassom.Ranging(CM) retorna a distancia em
   int distancia;// = ultrassom.Ranging();
                                   // centímetros(CM)
     
   digitalWrite(sensorTrigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(sensorTrigPin, LOW);

   distancia = pulseIn(sensorEchoPin, HIGH);
   distancia = distancia/58.2;
    if(distancia - old_d >= 20 || distancia - old_d <= -20){
      t++;//se variacao detectada incrementa o n° de passagens
      old_d = distancia; 
      t++;
    }
   Serial.println(distancia);
   Serial.println(t);
   
   char bigBuf[300] = "";
   sprintf(bigBuf, "%s%d%s", msg,distancia, msg2);
   Serial.println(bigBuf);

   client.publish(topic, bigBuf);
   delay(3000);
  
}
