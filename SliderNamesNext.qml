import QtQuick 2.12
//import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12

Item {
    id: dragItem
    property string textRed: ""
    property string textBlue: ""
    property real txtYpos
    property real heightRow

    property real maxY: 0

    property real textOffset // heightRow - 42
    property int szFont

    function setTextOffset(){
        textOffset = heightRow / 2 - txtRed.contentHeight / 2 - 5
    }

    width: parent.width
    height: heightRow + 1
    x: 0
    y : heightRow
    Drag.active: dragArea.drag.active
    Drag.hotSpot.y: 0
    property point beginDrag

    Rectangle {
        width: parent.width
        height: parent.height
        color: "orange"
        radius: 10
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
                x: parent.width / 4
                y: 0
                width: parent.width / 2
                height: parent.height
                color: "khaki"
            }
            Rectangle{
                x: 0
                y: 0
                width: parent.width / 2
                height: parent.height
                radius: 10
                color: "khaki"
                Text {
                    id: txtRed
                    width: parent.width
                    color: "red"
                    text: textRed
                    font.pixelSize: szFont
                    horizontalAlignment: Text.AlignHCenter
                    y: txtYpos + textOffset
                }
            }
            Rectangle{
                x: parent.width / 2
                y: 0
                width: parent.width / 2
                height: parent.height
                radius: 10
                color: "khaki"
                Text {
                    id: txtBlue
                    width: parent.width
                    color: "blue"
                    text: textBlue
                    font.pixelSize: szFont
                    horizontalAlignment: Text.AlignHCenter
                    y: txtYpos + textOffset
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
        drag.minimumY: heightRow
        drag.maximumX: 0
        drag.maximumY: maxY
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
            sliderPressed(pressedButtons)
            beginDrag = Qt.point(dragItem.x, dragItem.y)
        }
        propagateComposedEvents: true
        onReleased: {
            if((dragItem.y % heightRow) < heightRow / 2){
                dragItem.y = dragItem.y - dragItem.y % heightRow
            }else{
                dragItem.y = dragItem.y - dragItem.y % heightRow + heightRow
            }
            txtYpos = 0
            sliderReleased(1)
        }
        onMouseYChanged: {
            sliderYChangedNext(dragItem.y)
        }
    }
}
