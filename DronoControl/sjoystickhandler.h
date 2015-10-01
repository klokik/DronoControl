#ifndef SJOYSTICKHANDLER_H
#define SJOYSTICKHANDLER_H

#define DRONOSERIAL
#if defined(DRONOSERIAL)

#include <QtCore>
#include <QtSerialPort/QSerialPort>

class SJoystickHandler: public QThread
{
    Q_OBJECT

protected:
    QSerialPort serial;

    // read loop
    bool done = false;
    void run();

protected slots:
    void tryConnect();
    void serial_error(QSerialPort::SerialPortError err);

signals:
    void AxisChange(QVariant id,QVariant x,QVariant y);
    void ButtonChange(QVariant id,QVariant state,QVariant is_button);

public:
    SJoystickHandler();
    ~SJoystickHandler();
};

#endif

#include <QtBluetooth>

class BTJoystickHandler: public QThread
{
    Q_OBJECT

protected:
    QBluetoothSocket *socket;
    QMutex data_writing;
    QMutex connecting;

    bool connected = false;
    bool done = false;

    QString baddress = "30:14:07:31:47:82";

    void run();

protected slots:
    void tryConnect();

    void serverConnected();
    void serverDisconnected();
    void socketError(QBluetoothSocket::SocketError error);
    void readSocket();

signals:
    void AxisChange(QVariant id,QVariant x,QVariant y);
    void ButtonChange(QVariant id,QVariant state,QVariant is_button);

public:
    BTJoystickHandler();
    ~BTJoystickHandler();
};

#endif // SJOYSTICKHANDLER_H
