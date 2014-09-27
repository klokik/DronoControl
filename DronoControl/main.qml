import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1

Window {
    id: root
    visible: true
    minimumWidth: 854
    minimumHeight: 480
    flags: Qt.Dialog

    color: "#cdcdcd"

    function qmlUpdateInfo(jsonData)
    {
        var jso = eval('('+jsonData+')')

        console.log("Bob said: "+jso.greeting)
        rcNRF.activated = jso.connection.nrf;
        rcRPI.activated = jso.connection.rpi;
        rcBattery.activated = jso.battery.present;
        rcArmed.activated = jso.arm;
        rcSonar.activated = (jso.sonar !== -1);
        rcGPS.activated = jso.gps.fix;
    }

    RowLayout {
        id: activeFeatures

        anchors.top: parent.top
        anchors.topMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: 4

        property int indicatorSize: parent.width * 0.05

        Indicator {
            id: rcNRF
            text: "NRF"

            Layout.fillHeight: true
            Layout.minimumWidth: 48
            Layout.minimumHeight: 48
            Layout.preferredWidth: parent.indicatorSize
            Layout.preferredHeight: parent.indicatorSize
        }
        Indicator {
            id: rcRPI
            text: "RPI"

            Layout.fillHeight: true
            Layout.minimumWidth: 48
            Layout.minimumHeight: 48
            Layout.preferredWidth: parent.indicatorSize
            Layout.preferredHeight: parent.indicatorSize
        }
        Indicator {
            id: rcArmed
            text: "ARM"

            Layout.fillHeight: true
            Layout.minimumWidth: 48
            Layout.minimumHeight: 48
            Layout.preferredWidth: parent.indicatorSize
            Layout.preferredHeight: parent.indicatorSize
        }
        Indicator {
            id: rcSonar
            text: "SNR"

            Layout.fillHeight: true
            Layout.minimumWidth: 48
            Layout.minimumHeight: 48
            Layout.preferredWidth: parent.indicatorSize
            Layout.preferredHeight: parent.indicatorSize
        }
        Indicator {
            id: rcBattery
            text: "BAT"

            Layout.fillHeight: true
            Layout.minimumWidth: 48
            Layout.minimumHeight: 48
            Layout.preferredWidth: parent.indicatorSize
            Layout.preferredHeight: parent.indicatorSize
        }
        Indicator {
            id: rcGPS
            text: "GPS"

            Layout.fillHeight: true
            Layout.minimumWidth: 48
            Layout.minimumHeight: 48
            Layout.preferredWidth: parent.indicatorSize
            Layout.preferredHeight: parent.indicatorSize
        }
    }

    GridLayout {
        columns: 2
        rows: 2
        rowSpacing: 16

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 32
        anchors.leftMargin: 48

        AUXButton {
            id: aux_1
            aux_id: 1
            text: "Arm" //"AUX" + aux_id
            objectName: text

            Layout.minimumWidth: 110
            Layout.minimumHeight: 48
            Layout.preferredWidth: root.width * 0.15
            Layout.preferredHeight: root.height * 0.15
        }

        AUXButton {
            id: aux_2
            aux_id: 2
            text: "AltLock"//"AUX" + aux_id
            objectName: text

            anchors.right: parent.right

            Layout.minimumWidth: 110
            Layout.minimumHeight: 48
            Layout.preferredWidth: root.width * 0.15
            Layout.preferredHeight: root.height * 0.15
        }

        Rectangle {
            id: leftInfoBar
            color: leftStick.color
            width: 240

            Layout.row: 2
            Layout.column: 0
            Layout.columnSpan: 2

            anchors.topMargin: 16

            ColumnLayout {
                Text { text: "Not connected" }
                Text { text: "Not armed" }
                Text { text: "Sonar unavailable" }
                Text { text: "Battery state unknown" }
            }
        }
    }

    GridLayout {
        columns: 2
        rows: 2
        rowSpacing: 16

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 32
        anchors.rightMargin: 48

        AUXButton {
            id: aux_3
            aux_id: 3
            text: "CareFree"//"AUX" + aux_id
            objectName: text

            Layout.minimumWidth: 110
            Layout.minimumHeight: 48
            Layout.preferredWidth: root.width * 0.15
            Layout.preferredHeight: root.height * 0.15
        }

        AUXButton {
            id: aux_4
            aux_id: 4
            text: "GoHome"//"AUX" + aux_id
            objectName: text

            anchors.right: parent.right

            Layout.minimumWidth: 110
            Layout.minimumHeight: 48
            Layout.preferredWidth: root.width * 0.15
            Layout.preferredHeight: root.height * 0.15
        }

        Rectangle {
            id: rightInfoBar
            color: leftStick.color
            width: 240

            Layout.row: 2
            Layout.column: 0
            Layout.columnSpan: 2

            anchors.topMargin: 16

            ColumnLayout {
                Text { text: "Altitude unknown" }
                Text { text: "Speed unknown" }
                Text { text: "GPS data unavailable" }
            }
        }
    }

    RowLayout {
        id: controlPanel
        width: parent.width
        height: 336
       Layout.minimumHeight: 240
       Layout.preferredHeight: 240

       anchors.left: parent.left
       anchors.right: parent.right
       anchors.bottom: parent.bottom
       anchors.leftMargin: 48
       anchors.rightMargin: 48
       anchors.bottomMargin: 48

       focus: true

       Keys.onPressed: {

           switch(event.key)
           {
           case Qt.Key_J: rightStick.shift.y -= 0.05; break;
           case Qt.Key_K: rightStick.shift.y += 0.05; break;
           }

           if(event.isAutoRepeat)
               return;

           switch(event.key)
           {
           case Qt.Key_A: leftStick.shift.x -= .1; break;
           case Qt.Key_S: leftStick.shift.y -= .1; break;
           case Qt.Key_W: leftStick.shift.y += .1; break;
           case Qt.Key_D: leftStick.shift.x += .1; break;

           case Qt.Key_H: rightStick.shift.x -= .1; break;
           case Qt.Key_L: rightStick.shift.x += .1; break;

           case Qt.Key_0:
           case Qt.Key_1:
           case Qt.Key_2:
           case Qt.Key_3:
           case Qt.Key_4:
           case Qt.Key_5:
           case Qt.Key_6:
           case Qt.Key_7:
           case Qt.Key_8:
           case Qt.Key_9:
               rightStick.shift.y = (event.key-Qt.Key_0-5)*0.1*2; break;

           case Qt.Key_Y: aux_1.click(); break;
           case Qt.Key_U: aux_2.click(); break;
           case Qt.Key_I: aux_3.click(); break;
           case Qt.Key_O: aux_4.click(); break;
           }

           event.accepted = true;
       }

       Keys.onReleased: {
           if(event.isAutoRepeat)
               return;

           switch(event.key)
           {
           case Qt.Key_A: leftStick.shift.x += .1; break;
           case Qt.Key_S: leftStick.shift.y += .1; break;
           case Qt.Key_W: leftStick.shift.y -= .1; break;
           case Qt.Key_D: leftStick.shift.x -= .1; break;

           case Qt.Key_H: rightStick.shift.x += .1; break;
//           case Qt.Key_J: rightStick.shift.y -= 1; break;
//           case Qt.Key_K: rightStick.shift.y += 1; break;
           case Qt.Key_L: rightStick.shift.x -= .1; break;
           }

           event.accepted = true;
       }


       JStick {
           id: leftStick
           objectName: "leftStick"
           width: root.width * 0.25
           height: width

           anchors.left: parent.left
           anchors.bottom: parent.bottom

           signal qmlJShifted(int id,real x,real y)

           onShiftChanged: {
               qmlJShifted(0,shift.x,shift.y)
           }
       }

       JStick {
           id: rightStick
           objectName: "rightStick"
           width: root.width * 0.25
           height: width

           anchors.right: parent.right
           anchors.bottom: parent.bottom

           shift: Qt.vector2d(0,-1)

           axisFreeY: true

           signal qmlJShifted(int id,real x,real y)

           onShiftChanged: {
               qmlJShifted(1,shift.x,shift.y)
           }
       }

       Rectangle {
           id: throttleBar

           width: 72

           anchors.top: parent.top
           anchors.bottom: parent.bottom
           anchors.left: leftStick.right
           anchors.leftMargin: 36

           color: leftStick.color
       }

       Rectangle {
           id: otherBar

           width: 72

           anchors.top: parent.top
           anchors.bottom: parent.bottom
           anchors.right: rightStick.left
           anchors.rightMargin: throttleBar.anchors.leftMargin

           color: leftStick.color
       }
    }
}
