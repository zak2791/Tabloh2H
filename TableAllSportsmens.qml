import QtQuick 2.12

Rectangle {
    id: main
    width: parent.width - 20
    height: parent.height - 20
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    clip: true

    Component  {
        id: _highlight

        Rectangle {
            id: rec

            width: grid.cellWidth; height: grid.cellHeight
            opacity: 1
            color: "lightsteelblue"; radius: 5
            x: {
                if(grid.currentItem != null)
                    return grid.currentItem.x
                else
                    return -1000
            }
            y: {
                if(grid.currentItem != null)
                    return grid.currentItem.y
                else
                    return -1000
            }
            Behavior on x { SpringAnimation { spring: 5; damping: 0.3; mass: 0.8 } }
            Behavior on y { SpringAnimation { spring: 5; damping: 0.3; mass: 0.8 } }
        }
    }

    GridView {
        id: grid
        objectName: "grid"
        anchors.fill: parent
        model: myModel
        cellWidth: parent.width / 6

        signal moveItem(int i)

        highlight: _highlight
        delegate: Item {
            id: rectDel
            width: grid.cellWidth
            height: grid .cellHeight

            GridView.onRemove: SequentialAnimation {
                PropertyAction {
                    target: rectDel
                    property: "GridView.delayRemove"
                    value: true
                }
                NumberAnimation { target: rectDel; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                PropertyAction { target: rectDel; property: "GridView.delayRemove"; value: false }
            }

            Text {
                id: txt
                padding: 5
                text: name + "\n" + region + "\n" + index + "\n" + age + weight
            }
            MouseArea{
                id: mousearea
                hoverEnabled: true
                onHoveredChanged: {
                    grid.currentIndex = index;
                }
                anchors.fill: parent
                onPressed: {
                    if(index != -1){
                        txt.text = txt.text
                        grid.moveItem(index)
                    }
                }
            }
        }
        MouseArea{
            id: gridarea
            anchors.fill: parent
            propagateComposedEvents: true
            onPressed: {mouse.accepted = false}
            onReleased: {mouse.accepted = false}
        }
    }
}
