#include "dronodatabridge.h"

#include <QDebug>
#include <QTimer>

#include <utility>

void DronoDataBridge::flushTimerQueue()
{
    if(send_timer_queue.empty())
        return;

    startWrite();

    for(auto &item:send_timer_queue)
        writeByte(item.first,item.second);

    send_timer_queue.clear();

    endWrite();
}

bool DronoDataBridge::readBool(int reg)
{
    return (readByte(reg) != 0);
}

uint16_t DronoDataBridge::readUInt16(int reg)
{
    return readByte(reg)*256u+readByte(reg+1);
}

uint32_t DronoDataBridge::readUInt32(int reg)
{
    return
            readByte(reg+0)*16777216u+
            readByte(reg+1)*65536u+
            readByte(reg+2)*256u+
            readByte(reg+3);
}

float DronoDataBridge::readFloat(int reg)
{
    alignas(alignof(float)) uint32_t num = readUInt32(reg);

    return *reinterpret_cast<float*>(&num);
}

void DronoDataBridge::writeBool(int reg, bool val)
{
    writeByte(reg,val);
}

void DronoDataBridge::writeByte(int reg, uint8_t val)
{
    write_queue.push(std::pair<uint8_t,uint8_t>(reg,val));
}

void DronoDataBridge::writeUInt16(int reg, uint16_t val)
{
    writeByte(reg,val/256);
    writeByte(reg+1,val%256);
}

void DronoDataBridge::writeUInt32(int reg, uint32_t val)
{
    throw 0;
}

void DronoDataBridge::writeFloat(int reg, float val)
{
    throw 0;
}

void DronoDataBridge::startWrite()
{
    //
}

uint16_t DronoDataBridge::getLastPing()
{
    return 3;
}

void DronoDataBridge::enqueueTimerWriteUint16(int reg, uint16_t val)
{
    send_timer_queue[reg  ] = val/256;
    send_timer_queue[reg+1] = val%256;
}

void DronoDataBridge::enqueueTimerWriteByte(int reg, uint8_t val)
{
    send_timer_queue[reg] = val;
}

DronoDataBridge::DronoDataBridge()
{
    connect(&send_timer,SIGNAL(timeout()),this,SLOT(flushTimerQueue()));
}

#if defined(DRONOSERIAL)

#include <QtSerialPort/QSerialPortInfo>

DronoSerialDataBridge::DronoSerialDataBridge()
{
    connect(&serial,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(serial_error(QSerialPort::SerialPortError)));
    tryConnect();
}

uint8_t DronoSerialDataBridge::readByte(int reg)
{
    if(!serial.isOpen())
        return 0;

    uint8_t buf[] = {1,(uint8_t)reg,0,0};
    QString hexstr = "";
    for(uint q=0;q<sizeof(buf);q++)
        hexstr += QString::number(buf[q],16).rightJustified(2,'0');
    hexstr.append('\n');

    QByteArray ba_str = hexstr.toLocal8Bit();
    serial.write(ba_str.data(),hexstr.length());
    serial.flush();

    ba_str = serial.readLine(16);

    uint8_t val = ba_str[5]*16u+ba_str[6];

    return val;
}

void DronoSerialDataBridge::endWrite()
{
    if(!serial.isOpen())
        return;

    while(!write_queue.empty())
    {
        auto item=write_queue.front();
        write_queue.pop();

        uint8_t buf[] = {0,(uint8_t)item.first,item.second,0};
        QString hexstr = "";
        for(uint q=0;q<sizeof(buf);q++)
            hexstr += QString::number(buf[q],16).rightJustified(2,'0');
        hexstr.append('\n');

        QByteArray ba_str = hexstr.toLocal8Bit();
        serial.write(ba_str.data(),hexstr.length());
        serial.flush();

        ba_str = serial.readLine(16);
        qDebug()<<QString(ba_str);
    }
}

bool DronoSerialDataBridge::waitReady()
{
    return true;
}

void DronoSerialDataBridge::tryConnect()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
//        qDebug() << "Name        : " << info.portName();
//        qDebug() << "Description : " << info.description();
//        qDebug() << "Manufacturer: " << info.manufacturer();

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
    //return false;
}

void DronoSerialDataBridge::serial_error(QSerialPort::SerialPortError err)
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
            QTimer::singleShot(1000,this,SLOT(try_connect()));
        }
    }
}

DronoSerialDataBridge::~DronoSerialDataBridge()
{
    if(serial.isOpen())
    {
        serial.close();
        qDebug()<<"serial port closed";
    }
}
#endif /* DRONOSERIAL */



void DronoHttpDataBridge::ping()
{
    auto ver = readByte(REG_VER);

    qDebug()<<"#ping "<<ver;
}

uint8_t DronoHttpDataBridge::readByte(int reg)
{
    QNetworkAccessManager n_access_manager;
    QEventLoop n_event_loop;
    QObject::connect(&n_access_manager,SIGNAL(finished(QNetworkReply*)),&n_event_loop,SLOT(quit()));

    QNetworkRequest req(QUrl(QString("http://%1:%2/reg?%3").arg(server_ip).arg(server_port).arg(reg)));
    req.setHeader(QNetworkRequest::ContentTypeHeader,QStringLiteral("text/plain; charset=utf-8"));

    QNetworkReply *reply = n_access_manager.get(req);
    n_event_loop.exec();

    if(reply->error() == QNetworkReply::NoError)
    {
        QString fline = QString(reply->readLine(16));
        if(fline != QString("#ERROR\n"))
        {
            int val = fline.toInt();
            // OK is ok, don't interrupt user
            //qDebug()<<"Ok then, your requested reg value is "<<val<<"the rest is: "<<reply->readAll();
            return val;
        }
        else
        {
            qDebug()<<"Server hate's me, he refused to gimme reg value";
            //throw std::invalid_argument("reg num is incorrect");
            return 0;
        }
    }
    else
    {
        qDebug()<<"Oh, no! ntwokr rqwets fiald!";
//        throw std::runtime_error("failed to get request");
        return 0;
    }
}

void DronoHttpDataBridge::endWrite()
{
    QNetworkAccessManager n_access_manager;
    QEventLoop n_event_loop;
    QObject::connect(&n_access_manager,SIGNAL(finished(QNetworkReply*)),&n_event_loop,SLOT(quit()));


    QString s_url = QString("http://%1:%2/reg").arg(server_ip).arg(server_port);
    while(!write_queue.empty())
    {
        auto item = write_queue.front();
        write_queue.pop();
        s_url += QString("?%1?%2").arg(item.first).arg(item.second);
    }

    //XXX: just for UI debug network disabled
    return;

    QNetworkRequest req;
    req.setUrl(QUrl(s_url));
    req.setHeader(QNetworkRequest::ContentTypeHeader,QStringLiteral("text/plain; charset=utf-8"));

    QByteArray payload = QString("Hello").toUtf8();
    QNetworkReply *reply = n_access_manager.post(req,payload);
    n_event_loop.exec();

    if(reply->error() == QNetworkReply::NoError)
    {
        // the same thing
        // qDebug()<<"Ok then, your requested value is "<<reply->readAll();
    }
    else
    {
        qDebug()<<"Oh, no! ntwokr rqwets fiald!: "<<reply->error();
    }
}

bool DronoHttpDataBridge::waitReady()
{
    return true;
}

DronoHttpDataBridge::DronoHttpDataBridge()
{
//    QObject::connect(&n_access_manager,SIGNAL(finished(QNetworkReply*)),&n_event_loop,SLOT(quit()));
    QObject::connect(&alive,SIGNAL(timeout()),this,SLOT(ping()));
    alive.start(1000);
}


void DronoRfcommDataBridge::tryConnect()
{
    if(!connecting.tryLock())
        return; // connection routine has been already started

    qDebug()<<"Trying to connect single client \"Angel\"";

    //! [Connecting the socket]
    // memory leak!?
    socket = new QBluetoothSocket(QBluetoothServiceInfo::L2capProtocol);
    socket->connectToService(QBluetoothAddress(baddress),QBluetoothUuid(QBluetoothUuid::SerialPort));

    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(connected()), this, SLOT(serverConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));


//    QWaitCondition waiter;
//    waiter.wait(&connecting);

    qDebug()<<"Connection routine finished";
}

void DronoRfcommDataBridge::serverConnected()
{
    qDebug()<<"Server connected\twriting some data..";

//    QByteArray data;

//    data.append("Hello, world!\n");
//    data.append("Hello, world!\n");
//    socket->write(data.constData());

    qDebug()<<"\tdone";

    connected = true;
    connecting.unlock();
    etimer.start();
    send_timer.start(50);
}

void DronoRfcommDataBridge::serverDisconnected()
{
    qDebug()<<"Server disconnected";
    connected = false;
}

void DronoRfcommDataBridge::socketError(QBluetoothSocket::SocketError error)
{
    qDebug()<<"Socket error "<<error;
    connecting.unlock();
}

void DronoRfcommDataBridge::readSocket()
{
//    qDebug()<<"data received";

//    if (!socket)
//        return;

//    QByteArray line;
//    while (socket->canReadLine())
//    {
//        line = socket->readLine();
//        qDebug() << "Line: " << QString::fromUtf8(line.constData(), line.length());
//    }
}

uint8_t DronoRfcommDataBridge::readByte(int reg)
{
    if(!connected)
        return 0;

    uint8_t buf[] = {1,(uint8_t)reg,0,0};
    QString hexstr = "";
    for(uint q=0;q<sizeof(buf);q++)
        hexstr += QString::number(buf[q],16).rightJustified(2,'0');
    hexstr.append('\n');

    QByteArray ba_str = hexstr.toLocal8Bit();
    socket->write(ba_str.constData());
    socket->waitForBytesWritten(100);

    socket->waitForReadyRead(100);
    ba_str = socket->readLine(16);

    uint8_t val = ba_str[5]*16u+ba_str[6];

    return val;
}

void DronoRfcommDataBridge::endWrite()
{
    qDebug()<<"Writing data to BT";
    if(!connected)
    {
        qDebug()<<"BT device not connected";
        return;
    }

    // seems like a bug in socket->bytesToWrite() on android platform. Hmm, timeout?
    qDebug()<<"BTW "<<socket->bytesToWrite();
//    if(socket->bytesToWrite()>512)
////    if(etimer.elapsed()<100)
//    {
//        qDebug()<<"W: dropping packet";
//        return;
//    }

    etimer.restart();

    while(!write_queue.empty())
    {
        auto item=write_queue.front();
        write_queue.pop();

        uint8_t buf[] = {0,(uint8_t)item.first,item.second,0};
        QString hexstr = "";
        for(uint q=0;q<sizeof(buf);q++)
            hexstr += QString::number(buf[q],16).rightJustified(2,'0');
        hexstr.append('\n');

        QByteArray ba_str = hexstr.toLocal8Bit();
//        qDebug()<<"Sending write request...";
        socket->write(ba_str.constData());
//        socket->waitForBytesWritten(100);

//        qDebug()<<"waiting for response...";
//        socket->waitForReadyRead(300);
//        ba_str = socket->readLine(16);
//        qDebug()<<QString(ba_str);
    }
//    socket->waitForBytesWritten(100);
//    qDebug()<<"Write finished";
}

bool DronoRfcommDataBridge::waitReady()
{
    return true;
}

DronoRfcommDataBridge::DronoRfcommDataBridge()
{
    tryConnect();
}

DronoRfcommDataBridge::~DronoRfcommDataBridge()
{
    if(socket)
    {
        socket->close();
        delete socket;
    }
}
