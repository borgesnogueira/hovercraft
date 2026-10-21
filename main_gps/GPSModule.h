#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include <TinyGPS++.h>

class GPSModule 
{
private:

    TinyGPSPlus gps;
    HardwareSerial* serialGPS;

    // Coordenadas de Origem (Ponto (0,0) em metros)
    double lat0;
    double lon0;
    bool originSet;

    // Posição Relativa Atual (em metros)
    double relX;       // Leste (+) / Oeste (-)
    double relY;       // Norte (+) / Sul (-)
    double relDist;    // Distância total da origem

    // Velocidade e Rumo
    double speedKmh;
    double speedMs;
    double headingDeg;

    // Constantes
    static constexpr double RAIO_TERRA = 6378137.0; // metros
    static constexpr double DEG_RAD = 0.017453292519943295;

    void updateRelativePosition(double lat, double lon);

public:

    GPSModule();

    // Inicializa a comunicação Serial com modulo
    void begin(HardwareSerial& serial, unsigned long baud = 9600, int rxPin = 16, int txPin = 17);

    // Retorna true quando novas coordenadas válidas forem processadas
    bool update();

    // Fixa manualmente ou reseta o ponto zero para a posição atual
    void resetOrigin();

    // Getters
    bool hasFix();
    bool isOriginSet() const { return originSet; }
    int getSatellites();
    double getHDOP();

    double getLatitude();
    double getLongitude();

    double getX() const { return relX; }
    double getY() const { return relY; }
    double getDistanceFromOrigin() const { return relDist; }

    double getSpeedKmh() const { return speedKmh; }
    double getSpeedMs() const { return speedMs; }
    double getHeading() const { return headingDeg; }
};

#endif