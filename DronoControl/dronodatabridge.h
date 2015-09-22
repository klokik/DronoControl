#ifndef DRONODATAREADER_H
#define DRONODATAREADER_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUdpSocket>
#include <QtBluetooth/QBluetoothSocket>

#include <cstdint>
#include <queue>

class DronoDataBridge: public QObject
{
    Q_OBJECT

protected:
    uint16_t ping;

    std::queue< std::pair<uint8_t,uint8_t> > write_queue;
    std::map<uint8_t,uint8_t> send_timer_queue;
    std::map<uint8_t,uint8_t> last_val_set;
    std::vector<uint8_t> auto_update_regs;

    QTimer send_timer;

protected slots:
    virtual void flushTimerQueue();

public:
    bool readBool(int reg);
    virtual uint8_t readByte(int reg) = 0;
    uint16_t readUInt16(int reg);
    uint32_t readUInt32(int reg);
    float readFloat(int reg);

    void writeBool(int reg,bool val);
    virtual void writeByte(int reg,uint8_t val);
    virtual void writeUInt16(int reg,uint16_t val);
    virtual void writeUInt32(int reg,uint32_t val);
    virtual void writeFloat(int reg,float val);

    void startWrite();
    virtual void endWrite() = 0;
    virtual bool waitReady() = 0;

    uint16_t getLastPing();

    virtual void enqueueTimerWriteByte(int reg,uint8_t val);
    virtual void enqueueTimerWriteUint16(int reg,uint16_t val);

    DronoDataBridge();
};

//#define DRONOSERIAL
#if defined(DRONOSERIAL)

#include <QtSerialPort/QSerialPort>

class DronoSerialDataBridge: public DronoDataBridge//, public QObject
{
    Q_OBJECT

protected:
    QSerialPort serial;

protected slots:
    void tryConnect();
    void serial_error(QSerialPort::SerialPortError err);

public:
    virtual uint8_t readByte(int reg) override;
//    virtual void writeByte(int reg,uint8_t val) override;

    virtual void endWrite() override;
    virtual bool waitReady();

    DronoSerialDataBridge();
    ~DronoSerialDataBridge();
};
#endif /* DRONOSERIAL */

class DronoHttpDataBridge: public DronoDataBridge
{
    Q_OBJECT

protected:
//    QNetworkAccessManager n_access_manager;
//    QEventLoop n_event_loop;

protected slots:
//    void try_connect();
//    void serial_error(QSerialPort::SerialPortError err);
    void ping();

public:

    uint16_t server_port = 8000;
    QString server_ip = "192.168.0.102";
    QTimer alive;

    virtual uint8_t readByte(int reg) override;
//    virtual void writeByte(int reg,uint8_t val) override;

    virtual void endWrite() override;
    virtual bool waitReady();

    DronoHttpDataBridge();
};

class DronoUDPDataBridge: public DronoDataBridge
{
    Q_OBJECT

protected slots:
    void ping();

protected:
    QUdpSocket *usocket;

    QElapsedTimer etimer;

public:

    uint16_t server_port = 8081;
    QString server_ip = "192.168.1.1";
    QTimer alive;

    virtual uint8_t readByte(int reg) override;

    virtual void endWrite() override;
    virtual bool waitReady();

    DronoUDPDataBridge(int port);
    ~DronoUDPDataBridge();
};

class DronoRfcommDataBridge: public DronoDataBridge
{
    Q_OBJECT

protected:
    QBluetoothSocket *socket;
    QMutex data_writing;
    QMutex connecting;

    bool connected = false;

    QString baddress = "00:11:10:17:05:52";

    QElapsedTimer etimer;


protected slots:
    void tryConnect();

    void serverConnected();
    void serverDisconnected();
    void socketError(QBluetoothSocket::SocketError error);
    void readSocket();

public:
    virtual uint8_t readByte(int reg) override;
//    virtual void writeByte(int reg,uint8_t val) override;

    virtual void endWrite() override;
    virtual bool waitReady();

    DronoRfcommDataBridge();
    ~DronoRfcommDataBridge();
};

// 4
#define REG_VER				0x00
#define REG_READY			0x01
#define REG_LED				0x02
#define REG_I2C_ADDR		0x03

// 2+2*8=18
#define REG_MOTOR_UPD_RATE	0x0A
#define REG_MOTOR_NUM		0x0B
#define REG_MOTOR_VAL0		0x10

// 2+2*8=17
#define REG_RC_AVAIL		0x0C
#define REG_RC_NUM			0x0D
#define REG_RC_VAL0			0x20

// 3
#define REG_SONAR_AVAIL		0x0E
#define REG_SONAR_VAL0		0x30

// 3+5=8
#define REG_BATT_AVAIL		0x0F
#define REG_BATT_CELL_NUM	0x32
#define REG_BATT_LOW		0x33
#define REG_BATT_VAL0		0x34

// 2
#define REG_CMD_CALIBRATE	0x3C
#define REG_CMD_RESET		0x3D

// 3+4=7
#define REG_GOD_MODE		0x3E
#define REG_SEED			0x3F
#define REG_KEY				0x40
#define REG_SYS_ADDR0		0x41
#define REG_SYS_VAL0		0x43

// 1+3=4
#define REG_USART_ENABLE	0x4A
#define REG_USART_BR_VAL0	0x4B

// 6+10 = 16
#define REG_NRF_AVAIL		0x50
#define REG_NRF_ENABLE		0x51
#define REG_NRF_CAHNNEL		0x52
#define REG_NRF_SIG_POW		0x53
#define REG_NRF_READY		0x54
#define REG_NRF_PAY_SIZE	0x55
#define REG_NRF_CLT_ADDR0	0x56
#define REG_NRF_SRV_ADDR0	0x5B

// 2+2+8+2+4+2 = 20
#define REG_GPS_AVAIL		0x60
#define REG_GPS_FIX			0x61
#define REG_GPS_LAT_VAL0	0x68
#define REG_GPS_LONG_VAL0	0x6C
#define REG_GPS_SATS		0x62
#define REG_GPS_HOME_SET	0x63
#define REG_GPS_DIST_HOME_VAL0	0x64
#define REG_GPS_SPEED_VAL0	0x70

//
#define REG_BARO_AVAIL		0x72
#define REG_BARO_VAL0		0x74
#define REG_BARO_TEMP		0x73
#define REG_EST_ALT_VAL0	0x78


#define REG_NEXT			0x7C

#endif // DRONODATAREADER_H
