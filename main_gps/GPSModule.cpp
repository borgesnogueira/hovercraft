#include "GPSModule.h"
#include <cmath>

GPSModule::GPSModule()
    : serialGPS(nullptr)
    , lat0(0.0)
    , lon0(0.0)
    , originSet(false)
    , relX(0.0)
    , relY(0.0)
    , relDist(0.0)
    , speedKmh(0.0)
    , speedMs(0.0)
    , headingDeg(0.0)
{
}


// Inicializa a comunicação Serial com modulo
void GPSModule::begin(HardwareSerial& serial, unsigned long baud, int rxPin, int txPin) 
{
    serialGPS = &serial;
    serialGPS->begin(baud, SERIAL_8N1, rxPin, txPin);
}


// retorna true quando novas coordenadas válidas forem processadas
bool GPSModule::update() 
{
    if (!serialGPS) return false;
    while (serialGPS->available() > 0) 
    {
        gps.encode(serialGPS->read());
    }

    // verifica se houve atualização de localização válida
    if (gps.location.isUpdated() && gps.location.isValid()) 
    {
        double currentLat = gps.location.lat();
        double currentLon = gps.location.lng();
        
        // definição da origem
        if (!originSet && gps.satellites.value() >= 4) 
        {
            lat0 = currentLat;
            lon0 = currentLon;
            originSet = true;
        }

        // Calcula posição relativa se a origem já estiver fixada
        if (originSet) 
        {
            updateRelativePosition(currentLat, currentLon);
        }

        // Atualiza velocidade
        if (gps.speed.isValid()) 
        {
            speedKmh = gps.speed.kmph();
            speedMs  = gps.speed.mps();
            // se for menor que 0.3 m/s (~1 km/h), considera parado
            if (speedMs < 0.3) {
                speedKmh = 0.0;
                speedMs  = 0.0;
            }
        }

        // atualiza direção
        if (gps.course.isValid() && speedMs > 0.5) 
        {
            headingDeg = gps.course.deg();
        }

        return true;
    }

    return false;
}


void GPSModule::updateRelativePosition(double lat, double lon) 
{
    // Projeção Equiretangular Plana - precisão alta para alguns quilômetros (acho que é o bastante para o hover)
    double dLat = (lat - lat0) * DEG_TO_RAD;
    double dLon = (lon - lon0) * DEG_TO_RAD;
    double latMedia = ((lat + lat0) / 2.0) * DEG_TO_RAD;

    // Y = Norte (+)/Sul (-), X = Leste (+)/Oeste (-)
    relY = dLat * RAIO_TERRA;
    relX = dLon * RAIO_TERRA * cos(latMedia);
    relDist = sqrt(relX * relX + relY * relY);
}


void GPSModule::resetOrigin() 
{
    if (gps.location.isValid()) {
        lat0 = gps.location.lat();
        lon0 = gps.location.lng();
        originSet = true;
        relX = 0.0;
        relY = 0.0;
        relDist = 0.0;
    }
}

bool GPSModule::hasFix() 
{
    return gps.location.isValid() && gps.satellites.value() >= 4;
}

int GPSModule::getSatellites() 
{
    return gps.satellites.isValid() ? gps.satellites.value() : 0;
}

double GPSModule::getHDOP() 
{
    return gps.hdop.isValid() ? gps.hdop.hdop() : 99.9;
}
double GPSModule::getLatitude() 
{
    return gps.location.isValid() ? gps.location.lat() : 0.0;
}
double GPSModule::getLongitude() {
    return gps.location.isValid() ? gps.location.lng() : 0.0;
}