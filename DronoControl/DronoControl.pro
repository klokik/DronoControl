TEMPLATE = app

QT += qml quick bluetooth

linux:!android {
    QT += serialport
    INCLUDEPATH += /usr/include
    LIBS += -lSDL2
}

android {
    DEFINES += "__ANDROID__"
}

CONFIG += c++11

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

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
