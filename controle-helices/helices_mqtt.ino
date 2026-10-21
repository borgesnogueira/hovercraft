// para usar no vscode instale a extensao platformIO e mantenha o arquivo platformio.ini
// caso use o Arduino IDE apenas copie o codigo abaixo e instale as bibliotecas

#include <ESP32Servo.h>
#include <MQTT.h>
#include <WiFi.h>
const char *ssid = "..."; //colocar a rede que vai conectar
const char *password = "...";  //senha da rede
const char *mqtt_broker = "test.mosquitto.org";
const char *topic_prefix = "hovercraft/ect/";
WiFiClient client;
MQTTClient mqtt;
 
#define HEL_CENT 13
#define HEL_ESQ 12
#define HEL_DIR 14

Servo centro; // Pin, Min, Max
Servo esquerda;
Servo direita;

int converterAngulo(float val_joy, float speedMax = 1.0)
{
  int angulo = map(val_joy*100, -speedMax*100, speedMax*100, 0, 180);
  return constrain(angulo, 0, 180);
}

void connect()
{
  // Conecta ao Wi-Fi se ainda não estiver conectado
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi Conectado!");
  }
  // Conecta ao broker MQTT com um ClientID único baseado no MAC da placa
  Serial.print("Conectando ao broker MQTT...");
  String clientId = "ESP32_Hovercraft_" + String((uint32_t)ESP.getEfuseMac(), HEX);
  while (!mqtt.connect(clientId.c_str())) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nConectado ao broker MQTT com sucesso!");
  // Inscrição nos tópicos com o prefixo
  String prefix = String(topic_prefix);
  mqtt.subscribe(prefix + "helice_direita_x");
  mqtt.subscribe(prefix + "helice_direita_y");
  mqtt.subscribe(prefix + "helice_esquerda_x");
  mqtt.subscribe(prefix + "helice_esquerda_y");
  mqtt.subscribe(prefix + "helice_centro_y");

}

void setup() 
{
  Serial.begin(115200);

  centro.attach(HEL_CENT);
  esquerda.attach(HEL_ESQ);
  direita.attach(HEL_DIR);

  // Inicializa o broker MQTT
  mqtt.begin(mqtt_broker, client);
  mqtt.setTimeout(2000);

  mqtt.onMessage([](String topic, String payload){
    Serial.print("Chegou no topico: [");
    Serial.print(topic);
    Serial.print("] -> Payload: [");
    Serial.print(payload);
    Serial.println("]");

    float val = payload.toFloat();

    if (topic.endsWith("helice_direita_x"))
    {
      int angulo = converterAngulo(val);
      direita.write(angulo);
    }
    else if (topic.endsWith("helice_direita_y"))
    {
      int angulo = converterAngulo(val);
      direita.write(angulo);
    }
    else if (topic.endsWith("helice_esquerda_x"))
    {
      int angulo = converterAngulo(val);
      esquerda.write(angulo);
    }
    else if (topic.endsWith("helice_esquerda_y"))
    {
      int angulo = converterAngulo(val);
      esquerda.write(angulo);
    }
    else if (topic.endsWith("helice_centro_y"))
    {
      int angulo = converterAngulo(val);
      centro.write(angulo);
    }
  });

  connect();
}


void loop() 
{
  mqtt.loop();
  delay(10);

  // Reconecta se a conexão cair
  if (!mqtt.connected()) {
    connect();
  }  
}