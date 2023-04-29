#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"



const char* ssid = "Pablo";
const char* password = "andres11";

const char *mqttServer = "driver.cloudmqtt.com";
const int mqttPort = 18582;
const char *mqttUser = "IoT";
const char *mqttPassword = "LifeIsIoT";

const char *mqtt_topic_sub = "feed";
const char *mqtt_topic_pub = "serial";
const char *weight_topic = "weight";
String cadena;


HX711 balanza;
WiFiClient espClient; 
PubSubClient clientMQTT(espClient);


const int ledPin = D1;
const int DOUT = 12;
const int CLK = 13;

void onoff(String cadena){
  if (cadena == "on"){
    digitalWrite(ledPin, HIGH);
  }
    else if(cadena == "off"){
    digitalWrite(ledPin, LOW);
    }
}

void publicar_peso(float UltimoPeso){
  clientMQTT.publish(weight_topic, String(UltimoPeso).c_str());
  Serial.println(UltimoPeso);
  
}

void callback(char *topic, byte *payload, unsigned int length)
{
  // Notify about message arrived
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  //

  // Print the message received
  Serial.print("Message:");
  String response = "";
  for (unsigned int i = 0; i < length; i++)
  {
    response += (char)payload[i];
  }

  // Prints with visualization purposes
  Serial.println();
  Serial.println("-----------------------");
  Serial.println(response);
  cadena = response;
  onoff(cadena);
}

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup() {
   Serial.begin(9600);
   setup_wifi();
   
  pinMode(ledPin, OUTPUT);
  clientMQTT.setServer(mqttServer, mqttPort);
  clientMQTT.setCallback(callback);

  balanza.begin(DOUT, CLK);
  balanza.set_scale(217014.322581);
  balanza.tare(20);

  while (!clientMQTT.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (clientMQTT.connect("test1", mqttUser, mqttPassword))
    {

      Serial.println("connected");
    }
    else
    {

      Serial.println("failed with state ");
      Serial.print(clientMQTT.state());
      delay(2000);
    }
  }
  clientMQTT.subscribe(mqtt_topic_sub);
  clientMQTT.publish(mqtt_topic_pub, "conectado jeje");

  
}

void loop() {
  clientMQTT.loop();
  float salida = balanza.get_units();
  publicar_peso(salida);
  //clientMQTT.publish(weight_topic, String(salida).c_str());
  //float salida = digitalRead(DOUT);
  
  delay(5000);
}