import QtQuick 2.2

Item {
    id: root
    width: 240
    height: 240

    property vector2d shift: Qt.vector2d(0,0)
    property alias color: stickBlock.color

    signal positionChanged

    property bool axisFreeX: false
    property bool axisFreeY: false

    onShiftChanged: {
        if(Math.abs(shift.x) > 1) shift.x = (shift.x>0) ? 1 : -1
        if(Math.abs(shift.y) > 1) shift.y = (shift.y>0) ? 1 : -1
        positionChanged()
    }

    Rectangle {
        id: stickBlock
        color: "#454545"
        radius: width*0.5
        anchors.fill: parent
        border.width: 2

        Rectangle {
            id: stickPin
            width: 64
            height: 64
            radius: width*0.5
            color: "#1d1d1d"

            anchors.centerIn: parent
            anchors.horizontalCenterOffset: root.shift.normalized().x*(root.width-width)*Math.min(root.shift.length(),1)*0.5
            anchors.verticalCenterOffset: root.shift.normalized().y*(root.height-height)*Math.min(root.shift.length(),1)*(-0.5)

            Text {
                id: lblPos
                color: "white"
                anchors.centerIn: parent

                text: root.shift.x.toFixed(2) + "\n" + root.shift.y.toFixed(2)
            }
        }

        MultiPointTouchArea {
            anchors.fill:  parent

            onReleased: {
                if(!axisFreeX)
                    root.shift.x = 0
                if(!axisFreeY)
                    root.shift.y = 0
            }

            onUpdated: {
                root.shift.x = (touchPoints[0].x/width*2-1)
                root.shift.y = (touchPoints[0].y/height*(-2)+1)
            }
        }
    }
}
