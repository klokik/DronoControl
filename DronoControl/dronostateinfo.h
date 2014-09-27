#ifndef DRONOSTATEINFO_H
#define DRONOSTATEINFO_H

#include <memory>

#include "dronodatabridge.h"


class DronoGPSInfo
{
public:
    std::shared_ptr<DronoDataBridge> reader;

    bool enabled();
    bool fixed();
    bool home_set();
    float speed();
    float latitude();
    float longitude();
    float distance_home();

    QJsonObject toJson();
};

class DronoAltitudeInfo
{
public:
    std::shared_ptr<DronoDataBridge> reader;

    bool ground_set();
    bool baro_enabled();
    bool sonar_enabled();
    float sonar_alt();
    float baro_alt();
    float estimated_alt();
    float baro_temp();

};

class DronoBatInfo
{
public:
    std::shared_ptr<DronoDataBridge> reader;

    float voltage();
    int cell_num();
    float cell_val(int i);

    bool is_low();
    bool is_critical();

    QJsonObject toJson();
};

class DronoStateInfo
{
protected:
    std::shared_ptr<DronoDataBridge> reader;

public:
    DronoStateInfo();
    DronoStateInfo(std::shared_ptr<DronoDataBridge> _reader);

    bool armed();

    void setReader(std::shared_ptr<DronoDataBridge> _reader);

    DronoGPSInfo gps;
    DronoAltitudeInfo altitude;
    DronoBatInfo battery;

    QJsonObject toJson();
};

#endif // DRONOSTATEINFO_H
