TEMPLATE = app

QT += qml quick serialport

DEFINES += "DRONOSERIAL"
QMAKE_CXXFLAGS += -DDRONOSERIAL

CONFIG += c++11

SOURCES += main.cpp \
    dronostateinfo.cpp \
    dronodatabridge.cpp \
    joystickiohandler.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    TriggerHandler.h \
    AUXHandler.h \
    VJoystickIOHandler.h \
    dronostateinfo.h \
    dronodatabridge.h \
    joystickiohandler.h
