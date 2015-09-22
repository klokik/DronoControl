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
#endif // SJOYSTICKHANDLER_H
