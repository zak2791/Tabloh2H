import QtQuick 2.12
import QtQuick.Controls 1.4

Rectangle {
    width: parent.width - 20
    height: parent.height - 20

    property int heightRows: 30     //высота строк

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    property bool isSeparate: false //false - совместное, true - раздельное перемещение слайдеров
    property bool buttonLeft: false //false - была нажата правая кнопка, true - левая кнопка

    property int previewY: 22
    property int previewYNext: heightRows + 22

    signal sliderYChanged(int y)
    signal sliderYChangedNext(int y)

    signal sliderReleased(int slider)
    signal sliderPressed(int button)

    onSliderPressed: {
        if(button === Qt.RightButton){
            isSeparate = true
            buttonLeft = false
        }
        else
            buttonLeft = true
    }

    onSliderReleased: {
        if(slider === 0){   //если верхний слайдер двигали
            if(((sldNext.y - 22) % heightRows) < heightRows / 2){
                sldNext.y = sldNext.y - (sldNext.y - 22) % heightRows
            }else{
                sldNext.y = sldNext.y - (sldNext.y - 22) % heightRows + heightRows
            }
            sldNext.txtYpos = 0
        }else{
            if(((sld.y - 22) % heightRows) < heightRows / 2){
                sld.y = sld.y - (sld.y - 22) % heightRows
            }else{
                sld.y = sld.y - (sld.y - 22) % heightRows + heightRows
            }
            sld.txtYpos = 0
        }
    }

    onSliderYChanged: {
        let _y, i, ypos;
        _y = y - 7;
        i = (_y - _y % heightRows) / heightRows
        ypos = (y - 22) % heightRows
        sld.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos
        if(i < sideModel.rowCount() * 2){
            sld.textRed = sideModel.getNameRegion(i * 2)
            sld.textBlue = sideModel.getNameRegion(i * 2 + 1)
        }
        else{
            sld.textRed = ""
            sld.textBlue = ""
        }
        _y = y + heightRows - 7;
        i = (_y - _y % heightRows) / heightRows
        ypos = (y - 22) % heightRows
        if(!isSeparate){
            sldNext.y = y + heightRows
            sldNext.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos
            if(i < sideModel.rowCount() * 2){
                sldNext.textRed = sideModel.getNameRegion(i * 2)
                sldNext.textBlue = sideModel.getNameRegion(i * 2 + 1)
            }
            else{
                sldNext.textRed = ""
                sldNext.textBlue = ""
            }

        }
        else{
            if(previewY < y){   //движение вниз
                if(sldNext.y < y + heightRows){
                    sldNext.y = y + heightRows
                    sldNext.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos
                    if(i < sideModel.rowCount() * 2){
                        sldNext.textRed = sideModel.getNameRegion(i * 2)
                        sldNext.textBlue = sideModel.getNameRegion(i * 2 + 1)
                    }
                    else{
                        sldNext.textRed = ""
                        sldNext.textBlue = ""
                    }
                    if(buttonLeft)
                        isSeparate = false
                }
            }
        }
        previewY = y
    }

    onSliderYChangedNext: {
        let _y, i, ypos;
        _y = y - 7;
        i = (_y - _y % heightRows) / heightRows
        ypos = (y - 22) % heightRows
        sldNext.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos
        if(i < sideModel.rowCount() * 2){
            sldNext.textRed = sideModel.getNameRegion(i * 2)
            sldNext.textBlue = sideModel.getNameRegion(i * 2 + 1)
        }
        else{
            sldNext.textRed = ""
            sldNext.textBlue = ""
        }
        _y = y - heightRows - 7;
        i = (_y - _y % heightRows) / heightRows
        ypos = (y - 22) % heightRows
        if(!isSeparate){
            sld.y = y - heightRows
            sld.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos
            //console.log("getNameRegion " + sideModel.getNameRegion(i))
            if(i < sideModel.rowCount() * 2){
                //console.log("sideModel.countRows() " + sideModel.countRows())
                sld.textRed = sideModel.getNameRegion(i * 2)
                sld.textBlue = sideModel.getNameRegion(i * 2 + 1)
                //console.log("sideModel.get(i) " + sideModel.get())
            }
            else{
                sld.textRed = ""
                sld.textBlue = ""
            }
        }
        else{
            if(previewYNext > y){   //движение вверх
                if(sld.y > y - heightRows){
                    sld.y = y - heightRows
                    sld.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos
                    if(i < sideModel.rowCount() * 2){
                        sld.textRed = sideModel.getNameRegion(i * 2)
                        sld.textBlue = sideModel.getNameRegion(i * 2 + 1)
                    }
                    else{
                        sld.textRed = ""
                        sld.textBlue = ""
                    }
                    if(buttonLeft)
                        isSeparate = false
                }
            }
        }
        previewYNext = y
    }

    GridView {
        id: grid
        objectName: "gridSide"
        property int draggedItemIndex: -1
        anchors.fill: parent
        model: sideModel
        //model: mod
        cellWidth: parent.width / 2
        cellHeight: heightRows
        clip: true

        Drag.active:  dragArea.drag.active
        //Drag.active: visible

        //Drag.hotSpot.x: 15

        Item {
            id: dndConteiner
        }

        Canvas {
            anchors.fill: parent
            onPaint: {
                const context = getContext("2d");
                context.beginPath();
                context.moveTo(parent.width * 0.5, 0);
                context.lineTo(parent.width * 0.5, parent.height);
                context.moveTo(0, 21);
                context.lineTo(parent.width, 21);
                context.closePath();
                context.stroke();
            }
        }

        header: Rectangle {
            width:  parent.width
            height:  22

            Rectangle{
                width: parent.width / 2
                height: 22
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: "red"
                    text: qsTr("Спортсмен с красным поясом")
                }
            }
            Rectangle{
                width: parent.width / 2
                height: 22
                x: parent.width / 2
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: "blue"
                    text: qsTr("Спортсмен с синим поясом")
                }
            }
        }

        delegate: DelegateSide{id: del}

//        MouseArea {
//            id: dragArea

//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.fill: parent
//            //onReleased: dragItem.Drag.drop()



//            onReleased: {
//                grid.Drag.drop()
//                console.log("indexR = " + grid.indexAt(mouseX, mouseY - 22))
//            }
//            drag.target: dragItem

//            drag.axis: Drag.XAndYAxis
//            hoverEnabled: true
//            onPressed: {
//                console.log("indexP = " + grid.indexAt(mouseX, mouseY - 22))
//            }

//            states: [
//                State {
//                    when: dragItem.Drag.active
//                    ParentChange {
//                        target: dragItem
//                        parent: grid
//                    }
//                    StateChangeScript {
//                        name: "firstScript"
//                        script: console.log("entering first state")
//                    }
//                    AnchorChanges {
//                        target: dragItem
//                        anchors.horizontalCenter: undefined
//                        anchors.verticalCenter: undefined
//                    }
//                }
//            ]
       // }



        highlightFollowsCurrentItem: true
        focus: true

        SliderNames{
            id: sld
            heightRow: heightRows
        }

        SliderNamesNext{
            id: sldNext
            heightRow: heightRows
        }


//                    DropArea {
//                        id: dr
//                        anchors.fill: parent

//                        onDropped: console.log("dropped")
//                        onEntered: {
//                            //grid.indexAt(dragArea.mouseX, dragArea.mouseY)
//                            console.log("entered " + grid.indexAt(dragArea.mouseX, dragArea.mouseY))
//                            console.log("dragItem.index = " + index)
//                        }
//                    }


    }



}
