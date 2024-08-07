import QtQuick 2.12
import QtQuick.Controls 1.4

Rectangle {
    width: parent.width - 20
    height: parent.height - 20

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    property bool isSeparate: false //false - совместное, true - раздельное перемещение слайдеров
    property bool buttonLeft: false //false - была нажата правая кнопка, true - левая кнопка

    property int previewY: 22
    property int previewYNext: 52

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
        //(slider)=>{
            if(slider === 0){   //если верхний слайдер двигали
                if(((sldNext.y - 22) % 30) < 15){
                    sldNext.y = sldNext.y - (sldNext.y - 22) % 30
                }else{
                    sldNext.y = sldNext.y - (sldNext.y - 22) % 30 + 30
                }
                sldNext.txtYpos = 0
            }else{
                if(((sld.y - 22) % 30) < 15){
                    sld.y = sld.y - (sld.y - 22) % 30
                }else{
                    sld.y = sld.y - (sld.y - 22) % 30 + 30
                }
                sld.txtYpos = 0
            }
        //}
    }

    onSliderYChanged: {
        let _y, i, ypos;
        _y = y - 7;
        i = (_y - _y % 30) / 30
        ypos = (y - 22) % 30
        sld.txtYpos = ypos < 15 ? -ypos : 30 - ypos
        if(i < dataModel.count){
            sld.textRed = dataModel.get(i).red
            sld.textBlue = dataModel.get(i).blue
        }
        else{
            sld.textRed = ""
            sld.textBlue = ""
        }
        _y = y + 30 - 7;
        i = (_y - _y % 30) / 30
        ypos = (y - 22) % 30
        if(!isSeparate){
            sldNext.y = y + 30
            sldNext.txtYpos = ypos < 15 ? -ypos : 30 - ypos
            if(i < dataModel.count){
                sldNext.textRed = dataModel.get(i).red
                sldNext.textBlue = dataModel.get(i).blue
            }
            else{
                sldNext.textRed = ""
                sldNext.textBlue = ""
            }

        }
        else{
            if(previewY < y){   //движение вниз
                if(sldNext.y < y + 30){
                    sldNext.y = y + 30
                    sldNext.txtYpos = ypos < 15 ? -ypos : 30 - ypos
                    if(i < dataModel.count){
                        sldNext.textRed = dataModel.get(i).red
                        sldNext.textBlue = dataModel.get(i).blue
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
        i = (_y - _y % 30) / 30
        ypos = (y - 22) % 30
        sldNext.txtYpos = ypos < 15 ? -ypos : 30 - ypos
        if(i < dataModel.count){
            sldNext.textRed = dataModel.get(i).red
            sldNext.textBlue = dataModel.get(i).blue
        }
        else{
            sldNext.textRed = ""
            sldNext.textBlue = ""
        }
        _y = y - 30 - 7;
        i = (_y - _y % 30) / 30
        ypos = (y - 22) % 30
        if(!isSeparate){
            sld.y = y - 30
            sld.txtYpos = ypos < 15 ? -ypos : 30 - ypos
            if(i < dataModel.count){
                sld.textRed = dataModel.get(i).red
                sld.textBlue = dataModel.get(i).blue
            }
            else{
                sld.textRed = ""
                sld.textBlue = ""
            }
        }
        else{
            if(previewYNext > y){   //движение вверх
                if(sld.y > y - 30){
                    sld.y = y - 30
                    sld.txtYpos = ypos < 15 ? -ypos : 30 - ypos
                    if(i < dataModel.count){
                        sld.textRed = dataModel.get(i).red
                        sld.textBlue = dataModel.get(i).blue
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

    TableView {
        id: view
        objectName: "view"
        anchors.fill: parent
        model: sideModel
        clip: true

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

        headerDelegate: Rectangle {
            id: hd
            width: parent.width
            height: 22
            enabled: false

            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: styleData.value
                color: ["red", "blue"] [styleData.column % 2]
            }
        }






        TableViewColumn {
            id: c1
            width: parent.width * 0.5
            title: "Спортсмен с красным поясом"
            role: "name"
            resizable: false

        }

        TableViewColumn {
            width: parent.width * 0.5
            title: "Спортсмен с синим поясом"
            role: "name"
        }

        

        itemDelegate: Rectangle {
            color: (styleData.row % 2 > 0) ? "lightgray" : "transparent"
            Text {
                anchors.centerIn: parent
                renderType: Text.NativeRendering
                text: styleData.value
            }
            MouseArea{
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
            }
        }


        rowDelegate: Rectangle {
            id: rd
            height: 30

            readonly property int modelRow: styleData.row ? styleData.row : 0

            MouseArea{
                anchors.fill: parent


                onClicked: {
                    console.log(" rowDelegate clicked" + modelRow);
                    //lmodel.remove(index);
                }



            }

        }

        SliderNames{
            id: sld
        }

        SliderNamesNext{
            id: sldNext
        }

    }

}
