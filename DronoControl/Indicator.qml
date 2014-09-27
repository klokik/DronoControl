import QtQuick 2.2

Item {
    id: root

    property alias text: label.text
//    property alias width: block.width
//    property alias height: block.height


    width: 48
    height: 48

    property bool activated: false
    state: activated?"on":"off"

    Rectangle {
        id: block
        color: "red"

//        width: 48
//        height: 48

        anchors.fill: parent

        Text {
            id: label
            color: "white"
            font.bold: true
            anchors.centerIn: parent
        }
    }

    states: [
        State {
            name: "on"
            PropertyChanges { target: block; color: "lime" }
        },
        State {
            name: "off"
            PropertyChanges { target: block; color: "red" }
        }
    ]
}
