#include "sjoystickhandler.h"

#include <QStringList>
#include <QtSerialPort/QSerialPortInfo>


void SJoystickHandler::run()
{
    done = false;

    float axes[] = {0.f,0.f,0.f,-1.0f};
    float hmag = 500;

    while(!done)
    {
        if(serial.isOpen())
        {
            if(!serial.canReadLine())
            {
                msleep(15);
                continue;
            }

            auto line = QString(serial.readLine(16));
            auto tokens = line.split(' ');

            if(tokens.size() != 2)
                continue;

            auto id = tokens.at(0).toInt();
            auto value = tokens.at(1).toInt();

            if(id >= 4)
                emit ButtonChange(id-4,value>500?"high":"low",false);
            else
            {
                axes[id] = value/hmag - 1.f;

                if(id < 2)
                    emit AxisChange(0,axes[0],axes[1]);
                else
                    emit AxisChange(1,axes[2],axes[3]);
            }
        }
        else
            sleep(1);
    }
}

void SJoystickHandler::tryConnect()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if(info.manufacturer() == "FTDI")
        {
            serial.setPort(info);
            serial.setBaudRate(serial.Baud115200);
            if(serial.open(QIODevice::ReadWrite))
            {
                qDebug()<<"Opened port "<<info.portName();
                return;
            }
            else
                qDebug()<<"Port found but failed to open";
        }
    }

    qDebug()<< "No acceptable serial device found";
    QTimer::singleShot(1000,this,SLOT(tryConnect()));
}

void SJoystickHandler::serial_error(QSerialPort::SerialPortError err)
{
    if(err != QSerialPort::NoError)
    {
        qDebug()<<"Serial port error occured: "<<err;

        if(err == QSerialPort::ResourceError)
        {
            // a hack to ignore extra error messages
            static bool waiting_for_error = false;
            if(waiting_for_error)
            {
                qDebug()<<"Ignoring";
                return;
            }

            if(serial.isOpen())
            {
                waiting_for_error = true;
                qDebug()<<"Waiting for error";
                qDebug()<<"Close";
                serial.close();
                waiting_for_error = false;
                qDebug()<<"endwaiting";
            }

            qDebug()<<"Start connecting";
            QTimer::singleShot(1000,this,SLOT(tryConnect()));
        }
    }
}

SJoystickHandler::SJoystickHandler()
{
    connect(&serial,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(serial_error(QSerialPort::SerialPortError)));
    tryConnect();

    this->start();
}

SJoystickHandler::~SJoystickHandler()
{
    done = true;


    if(serial.isOpen())
    {
        serial.close();
        qDebug()<<"serial port closed";
    }
}

