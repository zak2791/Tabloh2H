import QtQuick 2.0

Rectangle {

    width: parent.width - 20
    height: parent.height - 20

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    clip: true

    GridView {
        id: grid
        objectName: "grid"
        anchors.fill: parent
        model: myModel
        cellWidth: parent.width / 6

        //highlight: Component { Rectangle { color: "lightsteelblue"; radius: 5 }}
        //highlightRangeMode: grid.ApplyRange
        signal moveItem(int i)
        delegate: Rectangle{
            id: rectDel

            width: grid.cellWidth
            height: grid .cellHeight
            color: mousearea.containsMouse ? "lightgray" : "white"

            Text {
                text: name + "\n" + region + "\n" + index + "\n" + age + weight
                //text: index


            }
            MouseArea{
                id: mousearea
                //objectName: "delegate"

                hoverEnabled: true
                anchors.fill: parent
                onClicked: {
                    //contactModel.append({"number": "5.95", "name":"Pizza"})
                    grid.moveItem(index)

                    console.log("hello" + index + name + objectName)
                }
                //                    onEntered: {


                //                        console.debug("Will print on pressed event")
                //                    }

                //                    onExited: {

                //                        console.debug("Will print on released event")
                //                    }
            }

        }



        highlightFollowsCurrentItem: true
        focus: true

        //                MouseArea{
        //                    hoverEnabled: false
        //                    anchors.fill: parent
        //                    onClicked: {
        //                        //contactModel.append({"number": "5.95", "name":"Pizza"})

        //                        console.log(grid.currentIndex)
        //        //                var obj = grid.childAt(mouse.x, mouse.y)
        //        //                           if ( !obj )
        //        //                               return;
        //        //                           console.log(obj.parent)
        //                        mouse.accepted = false

        //                }
        //                  propagateComposedEvents: true
        //               }


    }

    //    GridView {
    //        id: gridv

    //        anchors.fill: parent



    //        model: modelAll
    //        delegate: Text {text: modelData}
    //    }

}
