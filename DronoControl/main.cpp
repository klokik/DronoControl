#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlProperty>

//#include <QtSerialPort/QSerialPort>
//#include <QtSerialPort/QSerialPortInfo>

#include "VJoystickIOHandler.h"
#include "joystickiohandler.h"
#include "sjoystickhandler.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    VJoystickIOHandler vjoy_handler;
#if !defined(__ANDROID__)
    JoystickIOHandler joystick;
    SJoystickHandler sjoystick;
#endif
    BTJoystickHandler btjoystick;

    QObject *root;
    if((root = engine.rootObjects().first()))
    {
        QObject *stick;
        if((stick = root->findChild<QObject*>("leftStick")))
            QObject::connect(stick,SIGNAL(qmlJShifted(int,qreal,qreal)),&vjoy_handler,SLOT(triggerChanged(int,qreal,qreal)));
        if((stick = root->findChild<QObject*>("rightStick")))
            QObject::connect(stick,SIGNAL(qmlJShifted(int,qreal,qreal)),&vjoy_handler,SLOT(triggerChanged(int,qreal,qreal)));

        QObject *aux;
        QStringList aux_id_list;
        aux_id_list<<"1"<<"2"<<"3"<<"4";
        foreach(const QString &aid,aux_id_list)
        {
            if((aux = root->findChild<QObject*>("AUX"+aid)))
                QObject::connect(aux,SIGNAL(qmlAUXChanged(int,QString)),&vjoy_handler,SLOT(auxChanged(int,QString)));
        }

        QObject::connect(&vjoy_handler,SIGNAL(qmlUpdateInfo(QVariant)),root,SLOT(qmlUpdateInfo(QVariant)));
        QObject::connect(&vjoy_handler.update_info_timer,SIGNAL(timeout()),&vjoy_handler,SLOT(fetchInfo()));

#if !defined(__ANDROID__)
        QObject::connect(&joystick,SIGNAL(AxisChange(QVariant,QVariant,QVariant)),root,SLOT(qmlExtTriggerChanged(QVariant,QVariant,QVariant)));
        QObject::connect(&joystick,SIGNAL(ButtonChange(QVariant,QVariant,QVariant)),root,SLOT(qmlExtAuxChanged(QVariant,QVariant,QVariant)));

        QObject::connect(&sjoystick,SIGNAL(AxisChange(QVariant,QVariant,QVariant)),root,SLOT(qmlExtTriggerChanged(QVariant,QVariant,QVariant)));
        QObject::connect(&sjoystick,SIGNAL(ButtonChange(QVariant,QVariant,QVariant)),root,SLOT(qmlExtAuxChanged(QVariant,QVariant,QVariant)));
#endif

        QObject::connect(&btjoystick,SIGNAL(AxisChange(QVariant,QVariant,QVariant)),root,SLOT(qmlExtTriggerChanged(QVariant,QVariant,QVariant)));
        QObject::connect(&btjoystick,SIGNAL(ButtonChange(QVariant,QVariant,QVariant)),root,SLOT(qmlExtAuxChanged(QVariant,QVariant,QVariant)));

        vjoy_handler.update_info_timer.start(1000);
    }

    return app.exec();
}
