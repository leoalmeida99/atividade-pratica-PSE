#include <ESP8266WiFi.h> // >>> Para ESP8266
//#include <WiFi.h>      // >>> Para ESP32
#include "PubSubClient.h" 
#include "DHT.h" 
#include "credenciais.h"

// --- 1. CREDENCIAIS DO THINGSPEAK (MQTT) ---
const char* mqtt_server = MQTT_SERVER_CREDENCIAL; 
const char* mqtt_user = MQTT_USER_CREDENCIAL;
const char* mqtt_password = MQTT_PASSWORD_CREDENCIAL; 
const char* topic_publish = TOPIC_PUBLISH_CREDENCIAL;
const char* mqtt_client_id = MQTT_CLIENT_ID_CREDENCIAL; 

// --- 2. CONFIGURAÇÕES GERAIS ---
const char* ssid = WIFI_SSID_CREDENCIAL; 
const char* password = WIFI_PASSWORD_CREDENCIAL; 

#define DHTPIN 2 // DHT D4
#define DHTTYPE DHT11
#define LED_VERMELHO 5 // LED D1
#define LED_VERDE 4 // LED D2
DHT dht(DHTPIN, DHTTYPE); 
long lastMsg = 0;
const int INTERVALO_PUB = 15000; 

// Variáveis para armazenar a última temperatura enviada

float ultimaTempEnviada = -99.9; // Valor inicial impossível para forçar o primeiro envio
int ultimaUmidadeEnviada = -99;
const float LIMITE_MUDANCA_TEMP = 0.1; // Só envia se a temp. mudar mais que 0.1°C (evita ruído)
const int LIMITE_MUDANCA_UMI = 1;

// --- 3. OBJETOS MQTT ---
WiFiClient espClient;
PubSubClient client(espClient);

// ---------------------------------------------------------------- //

void reconnect() { // Caso o MQTT venha a desconectar em tempo de execução
  while (!client.connected()) {
    Serial.print("Tentando conexao MQTT..."); 
    
    // LED vermelho indica falha de conexão MQTT
    digitalWrite(LED_VERMELHO, HIGH);
    digitalWrite(LED_VERDE, LOW);

    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      Serial.println("Conectado! Cliente: " + String(mqtt_client_id)); 
      
      // Quando reconectar, volta ao LED verde
      digitalWrite(LED_VERMELHO, LOW);
      digitalWrite(LED_VERDE, HIGH);
      
    } else {
      Serial.print("Falhou, rc="); 
      Serial.print(client.state()); 
      Serial.println(". Tentando novamente em 5s"); 
      delay(5000);
    }
  }
}

// ---------------------------------------------------------------- //

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando..."); 

  dht.begin(); 
  Serial.println("Sensor DHT inicializado."); 

  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  
  // indica que esta ligando...
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(LED_VERDE, LOW);

  // --- FAZ A PRIMEIRA CONEXÃO COM O WIFI ---

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi"); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }

  Serial.println("\nWiFi Conectado!"); 
  
  // indica que foi conectado
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_VERDE, HIGH);

  client.setServer(mqtt_server, 1883); 
}

// ---------------------------------------------------------------- //

void loop() {

  // --- VERIFICA O WIFI CASO TENHA UMA QUEDA EM TEMPO DE EXECUÇÃO!!! ---

  if (WiFi.status() != WL_CONNECTED) {
    
    while (WiFi.status() != WL_CONNECTED) {

      //houve uma queda, ligar o led vermelho
      digitalWrite(LED_VERMELHO, HIGH);
      digitalWrite(LED_VERDE, LOW);
      delay(1000); 
    }

    reconnect(); // Força a reconexão imediata do MQTT, pois ele também caiu
  }

  if (!client.connected()) {
    reconnect(); 
  }
  client.loop(); // Mantém a conexão ativa do mqtt

  long now = millis();
  
  if (now - lastMsg > INTERVALO_PUB) { // verificar se o valor lido é maior que a margem de 0.1
    lastMsg = now; 

    float t = dht.readTemperature();
    int h = dht.readHumidity();
    
    if (!isnan(t) && !isnan(h)) { // verifica se é valido os valores de temperatura e umidade
      
      if ((abs(t - ultimaTempEnviada) > LIMITE_MUDANCA_TEMP) || (abs(h - ultimaUmidadeEnviada) > LIMITE_MUDANCA_UMI)) {     // <<< Verifica se a diferença (absoluta) é maior que o limite
        
        String payload = "field1=" + String(t, 1) + "&field2=" + String(h); 

        if (client.publish(topic_publish, payload.c_str())) { 
        
          ultimaTempEnviada = t; // <<< Atualiza o valor de controle APÓS o envio
          ultimaUmidadeEnviada = h;
        } 
      } 
    } 
  }

  delay(10); 
}