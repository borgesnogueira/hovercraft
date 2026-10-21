"""
Para compilar no Arduino IDE basta selecionar a pasta main_gps/ e compilar
o arquivo de mesmo nome, os outros serão lidos automaticamente
"""

#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include "GPSModule.h"

// Hardware Serial 2 do ESP32 (Pinos RX=16, TX=17)
HardwareSerial gpsHwSerial(2);
GPSModule gpsHover;

// Display OLED I2C (Endereço 0x3C, SDA=21, SCL=22)
SSD1306Wire display(0x3c, 21, 22);
unsigned long ultimoUpdateDisplay = 0;


void setup() {
    Serial.begin(115200);
    // Inicialização do Display OLED
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.drawString(0, 0, "HOVERCRAFT GPS");
    display.drawString(0, 20, "Iniciando GPS...");
    display.display();
    // Inicializa o módulo GPS (9600 baud, RX=16, TX=17)
    gpsHover.begin(gpsHwSerial, 9600, 16, 17);
    Serial.println("GPS NEO-6M Inicializado. Aguardando fix de satélites...");


}
void loop() {

    bool novoDado = gpsHover.update();
    if (novoDado) 
    {
        // imprime a cada novo dado
        Serial.printf("[GPS] Sat: %d | X: %.2fm | Y: %.2fm | Dist: %.2fm | Vel: %.2f km/h | Rumo: %.1f°\n",
                      gpsHover.getSatellites(),
                      gpsHover.getX(),
                      gpsHover.getY(),
                      gpsHover.getDistanceFromOrigin(),
                      gpsHover.getSpeedKmh(),
                      gpsHover.getHeading());
    }

    // Atualiza o display OLED
    if (millis() - ultimoUpdateDisplay >= 200) 
    {
        ultimoUpdateDisplay = millis();
        display.clear();
        if (!gpsHover.hasFix()) 
        {
            display.drawString(0, 0, "Aguardando Satelites...");
            display.drawString(0, 20, "Detectados: " + String(gpsHover.getSatellites()));
;
        } else {
            display.drawString(0, 0,  "X: " + String(gpsHover.getX(), 1) + "m | Y: " + String(gpsHover.getY(), 1) + "m");
            display.drawString(0, 14, "Dist. Origem: " + String(gpsHover.getDistanceFromOrigin(), 1) + " m");
            display.drawString(0, 28, "Vel: " + String(gpsHover.getSpeedKmh(), 1) + " km/h");
            display.drawString(0, 42, "Rumo: " + String(gpsHover.getHeading(), 0) + " deg");
            display.drawString(0, 54, "Sat: " + String(gpsHover.getSatellites()));
        }
        display.display();
    }
}