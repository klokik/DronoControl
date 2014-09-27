#ifndef VJOYSTICKIOANDLER_H
#define VJOYSTICKIOHANDLER_H

#include <QtCore>
#include <memory>
#include <QTimer>

#include "dronostateinfo.h"
#include "dronodatabridge.h"

class VJoystickIOHandler: public QObject
{
    Q_OBJECT

public slots:
    void triggerChanged(int tid,qreal x,qreal y)
    {
        QStringList slist;
        slist<<"Left"<<"Right";
        qDebug()<<slist[tid]<<" trigger changed ( "<<x<<" : "<<y<<" )";

//        return;
        dbridge->startWrite();

        dbridge->writeUInt16(REG_RC_VAL0+2*2*tid  ,(x+1)*0.5f*1000);
        dbridge->writeUInt16(REG_RC_VAL0+2*2*tid+2,(y+1)*0.5f*1000);

        dbridge->endWrite();
        dbridge->waitReady();
    }

    void auxChanged(int id,QString state)
    {
        qDebug()<<"AUX"<<id<<" changed to "<<state;

        uint16_t istate;
        if(state == "low")
            istate = 0;
        else
            istate = 1000;

        dbridge->startWrite();
        dbridge->writeUInt16(REG_RC_VAL0+2*4+(id-1)*2,istate);
        dbridge->endWrite();
    }

    void fetchInfo()
    {
//        qmlUpdateInfo(QString(QJsonDocument(dsinfo.toJson()).toJson()));
    }

signals:
    void qmlUpdateInfo(QVariant jsonData);

public:
    std::shared_ptr<DronoDataBridge> dbridge;
    DronoStateInfo dsinfo;

    QTimer update_info_timer;

    VJoystickIOHandler(void)
    {
        #if defined(DRONOSERIAL)
            dbridge = std::dynamic_pointer_cast<DronoDataBridge>(std::make_shared<DronoSerialDataBridge>());
        #else
            dbridge = std::dynamic_pointer_cast<DronoDataBridge>(std::make_shared<DronoHttpDataBridge>());
        #endif

        dsinfo.setReader(dbridge);
    }
};

#endif // VJOYSTICKIOHANDLER_H
