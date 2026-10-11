// para usar no vscode instale a extensao platformIO e mantenha o arquivo platformio.ini
// caso use o Arduino IDE apenas copie o codigo abaixo e instale as bibliotecas

#include <ESP32Servo.h>
#include <MQTT.h>
#include <WiFi.h>
const char *ssid = "Gilson Alves"; //colocar a rede que vai conectar
const char *password = "f28l11i25l5";  //senha da rede
const char *mqtt_broker = "test.mosquitto.org";
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
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  mqtt.begin(mqtt_broker, client);
  mqtt.setTimeout(2000);

  Serial.println("\nConectado ao broker MQTT");

  mqtt.subscribe("helice_direita_x");
  mqtt.subscribe("helice_direita_y");

  mqtt.subscribe("helice_esquerda_x");
  mqtt.subscribe("helice_esquerda_y");

  mqtt.subscribe("helice_centro_y");
}

void setup() 
{
  Serial.begin(115200);

  centro.attach(HEL_CENT);
  esquerda.attach(HEL_ESQ);
  direita.attach(HEL_DIR);

  mqtt.onMessage([](String topic, String payload){
    Serial.print("Chegou no topico: [");
    Serial.print(topic);
    Serial.print("] -> Payload: [");
    Serial.print(payload);
    Serial.println("]");

    float val = payload.toFloat();

    if (topic == "helice_direita_x")
    {
      int angulo = converterAngulo(val);
      direita.write(angulo);
    }
    else if (topic == "helice_direita_y")
    {
      int angulo = converterAngulo(val);
      direita.write(angulo);
    }
    else if (topic == "helice_esquerda_x")
    {
      int angulo = converterAngulo(val);
      esquerda.write(angulo);
    }
    else if (topic == "helice_esquerda_y")
    {
      int angulo = converterAngulo(val);
      esquerda.write(angulo);
    }
    else if (topic == "helice_centro_y")
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

  // Reconecta se a conexão cair
  if (!mqtt.connected()) {
    connect();
  }  
}