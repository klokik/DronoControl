TEMPLATE = app

QT += qml quick bluetooth

#DEFINES += "DRONOSERIAL"
#QMAKE_CXXFLAGS += -DDRONOSERIAL
QT += serialport

CONFIG += c++11

LIBS += -lSDL2

INCLUDEPATH += /usr/include

SOURCES += main.cpp \
    dronostateinfo.cpp \
    dronodatabridge.cpp \
    joystickiohandler.cpp \
    sjoystickhandler.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    VJoystickIOHandler.h \
    dronostateinfo.h \
    dronodatabridge.h \
    joystickiohandler.h \
    sjoystickhandler.h
