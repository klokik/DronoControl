import QtQuick 2.2

Item {
    id: root

    property alias text: label.text
    property int aux_id: 0

    state: "low"

    signal qmlAUXChanged(int id,string state)

    onStateChanged: {
        qmlAUXChanged(aux_id,state)
    }

    function click() {
        click_area.trigger()
    }

    Rectangle {
        id: block
        width: 110
        height: 48

        anchors.fill: parent

        Text {
            id: label
            font.bold: true
            anchors.centerIn: parent
        }

        MouseArea {
            id: click_area
            anchors.fill: parent

            function trigger() {
                switch(root.state)
                {
                    case "low": root.state = "high"; break;
                    case "mid": root.state = "high"; break;
                    case "high": root.state = "low"; break;
                }
            }

            onClicked: trigger()
        }
    }

    states: [
        State {
            name: "low"
            PropertyChanges { target: block; color: "#1d1d1d" }
            PropertyChanges { target: label; color: "#EEEEEE" }
        },
        State {
            name: "mid"
            PropertyChanges { target: block; color: "#4d4d4d" }
            PropertyChanges { target: label; color: "#EEEEEE" }
        },
        State {
            name: "high"
            PropertyChanges { target: block; color: "#EEEEEE" }
            PropertyChanges { target: label; color: "#1d1d1d" }
        }
    ]
}
