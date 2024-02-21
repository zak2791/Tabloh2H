import QtQuick 2.12
//import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12

Item {
    id: dragItem
    property string textRed: "red"
    property string textBlue: "blue"
    property real txtYpos
    property real heightRow

    width: parent.width
    height: heightRow + 1
    x: 0
    y : heightRow + 22
    Drag.active: dragArea.drag.active
    Drag.hotSpot.y: 22
    property point beginDrag

    Rectangle {
        width: parent.width
        height: parent.height
        color: "orange"
        Rectangle {
            id: textArea
            width: parent.width - 10
            height: parent.height - 10
            x:5
            y: 5
            color: "white"
            radius: 10
            clip: true
            Rectangle{
                x: 0
                y: 0
                width: parent.width / 2
                height: parent.height
                radius: 10
                Text {
                    id: txtRed
                    width: parent.width
                    color: "red"
                    text: textRed
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    y: txtYpos + 2
                }
            }
            Rectangle{
                x: parent.width / 2
                y: 0
                width: parent.width / 2
                height: parent.height
                radius: 10
                Text {
                    id: txtBlue
                    x: 15
                    width: parent.width
                    color: "blue"
                    text: textBlue
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    y: txtYpos + 2
                }
            }
            Canvas {
                anchors.fill: parent
                onPaint: {
                    const context = getContext("2d");
                    context.beginPath();
                    context.moveTo(parent.width / 2 ,0);
                    context.lineTo(parent.width / 2, parent.height);
                    context.closePath();
                    context.stroke();
                }
            }
        }
    }
    MouseArea {
        id: dragArea
        anchors.fill: parent
        drag.target: parent
        drag.minimumX: 0
        drag.maximumX: 0
        drag.minimumY: heightRow + 22
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
            sliderPressed(pressedButtons)
            beginDrag = Qt.point(dragItem.x, dragItem.y)
        }
        propagateComposedEvents: true
        onReleased: {
            if(((dragItem.y - 22) % heightRow) < heightRow / 2){
                dragItem.y = dragItem.y - (dragItem.y - 22) % heightRow
            }else{
                dragItem.y = dragItem.y - (dragItem.y - 22) % heightRow + heightRow
            }
            txtYpos = 0
            sliderReleased(1)
        }
        onMouseYChanged: {
            sliderYChangedNext(dragItem.y)
        }
    }
}
