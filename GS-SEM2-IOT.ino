/****************************************
 * Integrantes do grupo - GS-IOT | 5ECR - Fiap |
 * 
 * Henrique Mendes Coelho Rodrigues RM 81242
 * Lucas Fernando Cardoso Gonçalves RM 81333
 * Luiz Felipe Cardoso Gonçalves RM 80721
 * 
 * 
 ****************************************/

//Biblioteca do sensor DHT11
#include <DHT.h>

//Biblioteca MQTT ubidots
#include "UbidotsEsp32Mqtt.h"

#define DHTPIN 18 //D18
#define DHTTYPE DHT11

//Definindo velocidade do som para o cáculo da distancia do sensor ultrasonico
#define VELOCIDADE_SOM 0.034

//Constantes da Ubidots
#define TOKEN "BBFF-nAeQHPD1jyXzpEuPehcGzIIANU5Mxw"     // Your Ubidots TOKEN
#define WIFINAME "Disco_Voador"  // Your SSID
#define WIFIPASS "10211214"  // Your Wifi Pass

Ubidots client(TOKEN);


//Instanciando variavel do sensor de humidade e temperatura
DHT dht(DHTPIN, DHTTYPE);

//Pinagem sensor ultrasonico
const int trigPin = 19; //D19
const int echoPin = 21; //D21

//Variaveis do sensor ultrasonico
long duracao;
float distanciaCm;

//Função auxiliar Ubidots
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.println("Iniciando");

  //Sinais do sensor ultrasonico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
   
  Serial.begin(115200);
  dht.begin();
  client.setDebug(true);  // Pass a true or false bool value to activate debug messages
  client.connectToWifi(WIFINAME, WIFIPASS);
  client.setCallback(callback);
  client.setup();
  //client.begin(callback);
}

void loop() {

  //Verifica conexão
  if (!client.connected()) {
    Serial.print("Não conectado com a ubidots");
    client.reconnect();
  }

  //Lógica do sensor ultrasonico
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  

  duracao = pulseIn(echoPin, HIGH);
  distanciaCm = duracao * VELOCIDADE_SOM/2;
  
  Serial.print("Distância (cm): ");
  Serial.println(distanciaCm);
  //Fim da logica sensor ultrasonico

  //Lógica de humidade e temperatura
  float humidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  Serial.print("Humidade: ");
  Serial.println(humidade);

  Serial.print("Temperatura: ");
  Serial.println(temperatura);
  Serial.println();
  Serial.println();
  //Fim da logica sensor de humidade e temperatura

  client.add("sensor-humidade", humidade);
  client.add("sensor-temperatura", temperatura);
  client.add("sensor-distancia", distanciaCm);
  
  client.publish("esp32-iot-gs-sem2");
  client.loop();
  
  delay(1000);
}
