import QtQuick 2.12
import QtQml.Models 2.1

Item {
    id: i
    width: grid.cellWidth
    height: grid.cellHeight
    //    Rectangle{
    //        id: dragConteiner
    //        width: grid.cellWidth
    //        height: grid.cellHeight
    //    }
    //property int draggedItemIndex: -1
    //Drag.active: visible // Add this line of code

    Rectangle {
        id: dragItem
        width: grid.cellWidth
        height: grid.cellHeight
        //anchors.fill: parent
        color: (index % 4 > 0 && (index - 1) % 4 > 0) ? "lightgray" : "transparent"

        Drag.active:  dragArea.drag.active
        //Drag.active: visible

        //Drag.hotSpot.x: 15
        //Drag.hotSpot.y: 15

        //        states: State {
        //            when: dragArea.drag.active
        //            AnchorChanges {
        //                target: parent
        //                anchors {
        //                    verticalCenter: undefined
        //                    horizontalCenter: undefined
        //                }
        //            }
        //        }

        Text {
            id: txt
            anchors.centerIn: parent
            //Drag.active:  dragArea.drag.active
            renderType: Text.NativeRendering
            text: name + "; " + reg + "\n" + index
            color: {
                index % 2 > 0 ? "blue" : "red"
            }
        }





        MouseArea {
            id: dragArea

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.fill: parent
            //onReleased: dragItem.Drag.drop()
            onReleased: {
                dragItem.Drag.drop()
                console.log("indexR = " + index)
            }
            drag.target: dragItem

            drag.axis: Drag.XAndYAxis
            hoverEnabled: true
            onPressed: {
                console.log("indexP = " + index)
            }

            states: [
                State {
                    when: dragItem.Drag.active
                    ParentChange {
                        target: dragItem
                        parent: grid

                    }
                    StateChangeScript {
                                    name: "firstScript"
                                    script: console.log("entering first state")
                                }
                    AnchorChanges {
                        target: dragItem
                        anchors.horizontalCenter: undefined
                        anchors.verticalCenter: undefined
                    }

                }
            ]






               Rectangle {
                   id: rr
                   parent: grid
                    anchors.fill: grid


                        //visible: parent.containsDrag
                    }
               }

            DropArea {
                id: dr
                anchors.fill: parent

                onDropped: {

                    console.log("dropped")
                    console.log(grid.indexAt(dragArea.mouseX, dragArea.mouseY))
                }
                onEntered: {
                    console.log("entered")
                    //grid.indexAt(dragArea.mouseX, dragArea.mouseY)
                    //console.log("entered " + grid.indexAt(dragArea.mouseX, dragArea.mouseY))
                    //console.log("dragItem.index = " + index)
                }
            }

        //}

    }



} //Component

