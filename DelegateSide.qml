import QtQuick 2.12
import QtQml.Models 2.1

Item {
    id: itemDelegateSide
    width: grid.cellWidth
    height: grid.cellHeight

    property int sizeFont: 10       //размер шрифта

    GridView.onRemove: SequentialAnimation {
        PropertyAction {
            target: itemDelegateSide
            property: "GridView.delayRemove"
            value: true
        }
        NumberAnimation { target: itemDelegateSide; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
        PropertyAction { target: itemDelegateSide; property: "GridView.delayRemove"; value: false }
    }

    Rectangle {
        id: dragItem
        radius: 10
        width: grid.cellWidth
        height: grid.cellHeight
        color: (index % 4 > 0 && (index - 1) % 4 > 0) ? "lightgray" : "transparent"
        Drag.active:  dragArea.drag.active

        Text {
            id: txt
            anchors.centerIn: parent
            renderType: Text.NativeRendering
            text: name + "; " + reg
            color: {
                index % 2 > 0 ? "blue" : "red"
            }
            font.pixelSize: sizeFontTable
        }
        MouseArea {
            id: dragArea
            property int itemPress: -1
            property int itemRelease: -1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.fill: parent
            onReleased: {
                let p = mapToItem(grid, mouseX, mouseY)
                itemRelease = grid.indexAt(p.x, p.y + grid.contentY)
                if(itemRelease!= -1 && itemPress != itemRelease)
                    grid.swapItems(itemPress, itemRelease)
            }
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onPressed: {
                if (mouse.button === Qt.RightButton) { // 'mouse' is a MouseEvent argument passed into the onClicked signal handler
                    if(sideModel.rowCount() > 1)
                        grid.moveItem(index)
                    mouse.accepted = false
                }
                itemPress = index
            }
            drag.target: dragItem
            drag.axis: Drag.XAndYAxis
            states: [
                State {
                    when: dragItem.Drag.active
                    ParentChange {
                        target: dragItem
                        parent: grid
                    }
                    AnchorChanges {
                        target: dragItem
                        anchors.horizontalCenter: undefined
                        anchors.verticalCenter: undefined
                    }
                    PropertyChanges {
                        target: dragItem
                        color: "darkGrey"
                        border.color: "black"
                        border.width: 2
                    }
                },
                State {
                    name: "s2"
                    when: dragItem.Drag.active === false
                    PropertyChanges {
                        target: dragItem
                        border.color: "white"
                    }
                }
            ]
        }
    }
} //Component

