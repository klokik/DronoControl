#include "dronostateinfo.h"

DronoStateInfo::DronoStateInfo(std::shared_ptr<DronoDataBridge> _reader)
{
    setReader(_reader);
}

DronoStateInfo::DronoStateInfo()
{
}

bool DronoStateInfo::armed()
{
    uint32_t sum =
            reader->readUInt16(REG_MOTOR_VAL0+0) +
            reader->readUInt16(REG_MOTOR_VAL0+2) +
            reader->readUInt16(REG_MOTOR_VAL0+4) +
            reader->readUInt16(REG_MOTOR_VAL0+6);

    return (sum > 0);
}

void DronoStateInfo::setReader(std::shared_ptr<DronoDataBridge> _reader)
{
    reader = _reader;

    gps.reader = reader;
    altitude.reader = reader;
    battery.reader = reader;
}

QJsonObject DronoStateInfo::toJson()
{
    QJsonObject json;

    json["arm"] = armed();
    json["sonar"] = altitude.sonar_enabled()?altitude.sonar_alt():-1;
    json["altitude"] = altitude.estimated_alt();
    json["speed"] = gps.speed();
    json["baro"] = altitude.baro_enabled();
    json["baroAlt"] = altitude.baro_alt();
    json["baroTemp"] = altitude.baro_temp();

    json["gps"] = gps.toJson();
    json["battery"] = battery.toJson();

    QJsonObject conn;
    conn["nrf"] = false;
    conn["rpi"] = true;
    conn["ping"] = reader->getLastPing();

    json["connection"] = conn;

    json["greeting"] = QString("Ou, yeah!");

    qDebug()<<QString(QJsonDocument(json).toJson());

    return json;
}

bool DronoGPSInfo::enabled()
{
    return reader->readFloat(REG_GPS_AVAIL);
}

bool DronoGPSInfo::fixed()
{
    return reader->readBool(REG_GPS_FIX);
}

bool DronoGPSInfo::home_set()
{
    return reader->readBool(REG_GPS_HOME_SET);
}

float DronoGPSInfo::speed()
{
    return reader->readFloat(REG_GPS_SPEED_VAL0);
}

float DronoGPSInfo::latitude()
{
    return reader->readFloat(REG_GPS_LAT_VAL0);
}

float DronoGPSInfo::longitude()
{
    return reader->readFloat(REG_GPS_LONG_VAL0);
}

float DronoGPSInfo::distance_home()
{
    return reader->readFloat(REG_GPS_DIST_HOME_VAL0);
}

QJsonObject DronoGPSInfo::toJson()
{
    QJsonObject json;

    json["fix"] = fixed();
    json["lat"] = latitude();
    json["long"] = longitude();
    json["sats"] = 0;
    json["homeSet"] = home_set();
    json["distanceHome"] = distance_home();

    return json;
}

bool DronoAltitudeInfo::ground_set()
{
    return false;
}

bool DronoAltitudeInfo::baro_enabled()
{
    return reader->readBool(REG_BARO_AVAIL);
}

bool DronoAltitudeInfo::sonar_enabled()
{
    return reader->readBool(REG_SONAR_AVAIL);
}

float DronoAltitudeInfo::sonar_alt()
{
    return reader->readUInt16(REG_SONAR_VAL0);
}

float DronoAltitudeInfo::baro_alt()
{
    return reader->readFloat(REG_BARO_VAL0);
}

float DronoAltitudeInfo::estimated_alt()
{
    return reader->readFloat(REG_EST_ALT_VAL0);
}

float DronoAltitudeInfo::baro_temp()
{
    return reader->readByte(REG_BARO_TEMP)/5;
}

float DronoBatInfo::voltage()
{
    float sum = 0;
    int num = cell_num();

    for(int q=0;q<num;q++)
        sum += cell_val(q);

    return sum;
}

int DronoBatInfo::cell_num()
{
    return reader->readByte(REG_BATT_CELL_NUM);
}

float DronoBatInfo::cell_val(int i)
{
    float voltage = reader->readByte(REG_BATT_VAL0+i);
    voltage /= 10;

    return voltage;
}

bool DronoBatInfo::is_low()
{
    return reader->readBool(REG_BATT_LOW);
}

bool DronoBatInfo::is_critical()
{
    //FIXME
    return is_low();
}

QJsonObject DronoBatInfo::toJson()
{
    QJsonObject json;

    json["present"] = (cell_num()>0);
    json["cells"] = cell_num();
    json["voltage"] = voltage();
    json["low"] = is_low();
    json["critical"] = is_critical();


    return json;
}
