#include "sjoystickhandler.h"

#include <QStringList>

#if !defined(__ANDROID__)

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

//    qDebug()<< "No acceptable serial device found";
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

#endif // __ANDROID__

void BTJoystickHandler::run()
{
//    done = false;

//    float axes[] = {0.f,0.f,0.f,-1.0f};
//    float hmag = 500;

//    while(!done)
//    {
//        if(socket->isOpen())
//        {
//            qDebug() << "Waiting for RFCOMM message";
//            if(!socket->waitForReadyRead(1000))
//            {
//                qDebug() << "Timeout";
//                msleep(500);
//                qDebug() << "BytesAvailable: " << socket->bytesAvailable();
//                continue;
//            }

//            if(!socket->canReadLine())
//            {
//                qDebug() << "Message is not ready!";
////                msleep(15);
//                continue;
//            }

//            auto line = QString(socket->readLine(16));
//            auto tokens = line.split(' ');

//            if(tokens.size() != 2)
//                continue;

//            auto id = tokens.at(0).toInt();
//            auto value = tokens.at(1).toInt();

//            if(id >= 4)
//                emit ButtonChange(id-4,value>500?"high":"low",false);
//            else
//            {
//                axes[id] = value/hmag - 1.f;

//                if(id < 2)
//                    emit AxisChange(0,axes[0],axes[1]);
//                else
//                    emit AxisChange(1,axes[2],axes[3]);
//            }
//        }
//        else
//            sleep(1);
//    }
}

void BTJoystickHandler::tryConnect()
{
    if(!connecting.tryLock())
        return; // connection routine has been already started

    qDebug()<<"Trying to connect single client \"Angel\"";

    //! [Connecting the socket]
    // memory leak!?
    QBluetoothUuid uuid = QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB"));
    qDebug() << "Used uuid " << QBluetoothUuid(QBluetoothUuid::Rfcomm)
             << "\nvs " << QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB"));
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    socket->connectToService(QBluetoothAddress(baddress),uuid);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(connected()), this, SLOT(serverConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));


//    QWaitCondition waiter;
//    waiter.wait(&connecting);

    qDebug()<<"Connection routine finished";

//    qDebug()<< "No acceptable serial device found";
//    QTimer::singleShot(1000,this,SLOT(tryConnect()));
}

void BTJoystickHandler::serverConnected()
{
    qDebug()<<"Server connected\twriting some data..";

//    QByteArray data;

//    data.append("Hello, world!\n");
//    data.append("Hello, world!\n");
//    socket->write(data.constData());

    qDebug()<<"\tdone";

    qDebug() << "Open: " << socket->isOpen();
    qDebug() << "Readable: " << socket->isReadable();
    qDebug() << "Writable: " << socket->isWritable();
    qDebug() << "TextMode: " << socket->isTextModeEnabled();
    qDebug() << "pName: " << socket->peerName();
    qDebug() << "pAddress: " << socket->peerAddress();
    qDebug() << "pPort: " << socket->peerPort();
    qDebug() << "OpenMode: " << socket->openMode();
    qDebug() << "BytesAvailable: " << socket->bytesAvailable();
    socket->dumpObjectInfo();

    connected = true;
    connecting.unlock();
}

void BTJoystickHandler::serverDisconnected()
{
    qDebug()<<"Server disconnected";
    connected = false;
}

void BTJoystickHandler::socketError(QBluetoothSocket::SocketError error)
{
    qDebug()<<"Socket error "<<error;
    connecting.unlock();
}

void BTJoystickHandler::readSocket()
{
    if (!socket)
        return;

    float hmag = 500;

    while (socket->canReadLine())
    {
        auto line = QString(socket->readLine(16));
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

}

BTJoystickHandler::BTJoystickHandler()
{
    tryConnect();

//    this->start();
}

BTJoystickHandler::~BTJoystickHandler()
{
    done = true;

    if(socket->isOpen())
    {
        socket->close();
        qDebug()<<"RFCOMM connection closed";
        delete socket;
    }
}
