#include <ESP32Servo.h>
 
#define HEL_CENT 13
#define HEL_ESQ 12
#define HEL_DIR 14
#define POT_PIN1 15
#define POT_PIN2 35
#define POT_PIN3 34

Servo centro; // Pin, Min, Max
Servo esquerda;
Servo direita;

void setup() {
  Serial.begin(9600);
  pinMode(POT_PIN1, INPUT);
  pinMode(POT_PIN2, INPUT);
  pinMode(POT_PIN3, INPUT);
  centro.attach(HEL_CENT, 1000, 2000);
  esquerda.attach(HEL_ESQ, 1000, 2000);
  direita.attach(HEL_DIR, 1000, 2000);

  
}

void loop() {
  int val1 = analogRead(POT_PIN1);
  int val2 = analogRead(POT_PIN2);
  int val3 = analogRead(POT_PIN3);
  val1 = map(val1, 0, 4095, 1000, 2000);
  val2 = map(val2, 0, 4095, 1000, 2000);
  val3 = map(val3, 0, 4095, 1000, 2000); // Scale 12-bit ADC to PWM
  centro.writeMicroseconds(val1);
  esquerda.writeMicroseconds(val2);
  direita.writeMicroseconds(val3);

  /*Serial.print(val1);
  Serial.print(" ");
  Serial.print(val2);
  Serial.print(" ");
  Serial.println(val3);*/
  delay(10);   
}