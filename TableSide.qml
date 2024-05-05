import QtQuick 2.12
import QtQuick.Controls 1.4

Rectangle {
    id: rect
    width: parent.width - 20
    height: parent.height - 20
    radius: 10

    property int heightRows: 60     //высота строк
    property int sizeFont: 14       //размер шрифта слайдера
    property int sizeFontTable: 10  //размер шрифта таблицы

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    property bool isSeparate: false //false - совместное, true - раздельное перемещение слайдеров
    property bool buttonLeft: false //false - была нажата правая кнопка, true - левая кнопка

    property int countRemovedRows: 0

    property int previewY: 0
    property int previewYNext: heightRows

    signal sliderYChanged(int y)
    signal sliderYChangedNext(int y)

    signal sliderReleased(int slider)
    signal sliderPressed(int button)

    onHeightRowsChanged: {
        sld.y = 0
        sldNext.y = heightRows
        grid.updateSlider()
    }

    onSizeFontChanged: {
        sld.szFont = sizeFont
        sldNext.szFont = sizeFont
        grid.updateSlider()
    }

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
            if((sldNext.y % heightRows) < heightRows / 2){
                sldNext.y = sldNext.y - sldNext.y % heightRows
            }else{
                sldNext.y = sldNext.y - sldNext.y % heightRows + heightRows
            }
            sldNext.txtYpos = 0
        }else{
            if((sld.y % heightRows) < heightRows / 2){
                sld.y = sld.y - sld.y % heightRows
            }else{
                sld.y = sld.y - sld.y % heightRows + heightRows
            }
            sld.txtYpos = 0
        }
    }

    onSliderYChanged: {
        let _y, i, ypos;
        _y = y + heightRows / 2 + grid.contentY - countRemovedRows * heightRows;
        //_y = y + heightRows / 2;
        i = (_y - _y % heightRows) / heightRows
        console.log("onSliderYChanged i = ", i)
        ypos = y % heightRows
        sld.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos
        //sld.textRed = ypos < heightRows / 2 ? sideModel.getNameRegion(i * 2) : sideModel.getNameRegion((i + 1) * 2)
        //console.log("i = ", i, "sideModel.rowCount() = ", sideModel.rowCount())
        //console.log("rect.height = ", rect.height, "grid.height = ", grid.height, ' ',  "grid.contentHeight = ", grid.contentHeight)

        if(i < sideModel.rowCount() / 2){
            sld.textRed = sideModel.getNameRegion(i * 2)
            sld.ageRed = sideModel.getAge(i * 2)
            sld.weightRed = sideModel.getWeight(i * 2)
        }else{
            sld.textRed = ""
            sld.ageRed = ""
            sld.weightRed = ""
        }
        if(i <= sideModel.rowCount() / 2 - 1){
            sld.textBlue = sideModel.getNameRegion(i * 2 + 1)
            sld.ageBlue = sideModel.getAge(i * 2 + 1)
            sld.weightBlue = sideModel.getWeight(i * 2 + 1)
        }else{
            sld.textBlue = ""
            sld.ageBlue = ""
            sld.weightBlue = ""
        }

        //_y = y + heightRows  + heightRows / 2 + grid.contentY - countRemovedRows * heightRows
//        _y = y + heightRows / 2;
//        i = (_y - _y % heightRows) / heightRows
//        console.log("i = ", i)
//        ypos = y % heightRows
        if(!isSeparate){
            sldNext.y = y + heightRows
            sldNext.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos

            if(i + 1 < sideModel.rowCount() / 2)
                sldNext.textRed = sideModel.getNameRegion((i + 1) * 2)
            else
                sldNext.textRed = ""
            if(i + 1 <= sideModel.rowCount() / 2 - 1)
                sldNext.textBlue = sideModel.getNameRegion((i + 1) * 2 + 1)
            else
                sldNext.textBlue = ""

        }else{
            if(previewY < y){   //движение вниз
                if(sldNext.y < y + heightRows){
                    sldNext.y = y + heightRows
                    sldNext.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos

                    if(i + 1 < sideModel.rowCount() / 2)
                        sldNext.textRed = sideModel.getNameRegion((i + 1) * 2)
                    else
                        sldNext.textRed = ""
                    if(i + 1 <= sideModel.rowCount() / 2 - 1)
                        sldNext.textBlue = sideModel.getNameRegion((i + 1) * 2 + 1)
                    else
                        sldNext.textBlue = ""

                    if(buttonLeft)
                        isSeparate = false
                }
            }
        }
        previewY = y

    }

    onSliderYChangedNext: {
        let _y, i, ypos;
        _y = y + heightRows / 2 + grid.contentY - countRemovedRows * heightRows;
        //_y = y + heightRows / 2;

        i = (_y - _y % heightRows) / heightRows
        console.log("onSliderYChangedNext i = ", i)
        ypos = y % heightRows
        sldNext.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos

        if(i < sideModel.rowCount() / 2)
            sldNext.textRed = sideModel.getNameRegion(i * 2)
        else
            sldNext.textRed = ""
        if(i <= sideModel.rowCount() / 2 - 1)
            sldNext.textBlue = sideModel.getNameRegion(i * 2 + 1)
        else
            sldNext.textBlue = ""

        //_y = y - heightRows + heightRows / 2 + grid.contentY -  countRemovedRows * heightRows
//        _y = y + heightRows / 2;
//        i = (_y - _y % heightRows) / heightRows
//        ypos = y % heightRows
        if(!isSeparate){
            sld.y = y - heightRows
            sld.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos

            if(i - 1 < sideModel.rowCount() / 2){
                sld.textRed = sideModel.getNameRegion((i - 1) * 2)
                sld.ageRed = sideModel.getAge((i - 1) * 2)
                sld.weightRed = sideModel.getWeight((i - 1) * 2)
            }else{
                sld.textRed = ""
                sld.ageRed = ""
                sld.weightRed = ""
            }
            if(i - 1 <= sideModel.rowCount() / 2 - 1){
                sld.textBlue = sideModel.getNameRegion((i - 1) * 2 + 1)
                sld.ageBlue = sideModel.getAge((i - 1) * 2 + 1)
                sld.weightBlue = sideModel.getWeight((i - 1) * 2 + 1)
            }else{
                sld.textBlue = ""
                sld.ageBlue = ""
                sld.weightBlue = ""
            }

        }
        else{
            if(previewYNext > y){   //движение вверх
                if(sld.y > y - heightRows){
                    sld.y = y - heightRows
                    sld.txtYpos = ypos < heightRows / 2 ? -ypos : heightRows - ypos

                    if(i < sideModel.rowCount() / 2){
                        sld.textRed = sideModel.getNameRegion(i * 2)
                        sld.ageRed = sideModel.getAge(i * 2)
                        sld.weightRed = sideModel.getWeight(i * 2)
                    }else{
                        sld.textRed = ""
                        sld.ageRed = ""
                        sld.weightRed = ""
                    }
                    if(i <= sideModel.rowCount() / 2 - 1){
                        sld.textBlue = sideModel.getNameRegion(i * 2 + 1)
                        sld.ageBlue = sideModel.getAge(i * 2 + 1)
                        sld.weightBlue = sideModel.getWeight(i * 2 + 1)
                    }else{
                        sld.textBlue = ""
                        sld.ageBlue = ""
                        sld.weightBlue = ""
                    }

                    if(buttonLeft)
                        isSeparate = false
                }
            }
        }
        previewYNext = y
    }

    Timer {
        id: timer
    }

    function delay(delayTime, cb) {
        timer.interval = delayTime;
        timer.repeat = false;
        timer.triggered.connect(cb);
        timer.start();
    }

    function delUpper(){
        let ind = grid.indexAt(0, sld.y + grid.contentY)
        //console.log("contentY = ", grid.contentY)
        if(ind > 0){
//            for(let i = 0; i < ind; i++){
//                //console.log("moveItem " + i)
//                grid.moveItem(0)

//            }
            grid.moveItems(ind)
            countRemovedRows += ind / 2
            parent.parent.adjustableRowHeight = false
            //console.log(parent.parent.objectName)
        }
    }

    GridView {
        id: grid
        objectName: "gridSide"
        property int currentYSldider: 0
        property int currentYSldiderNext: 0

        signal swapItems(int i1, int i2)
        signal moveItem(int i)
        signal moveItems(int i)


        function updateSlider(){
            let _y, i;

            _y = sld.y;
            //_y = sld.y + heightRows / 2 + grid.contentY - countRemovedRows * heightRows;
            i = (_y - _y % heightRows) / heightRows
            console.log("update", i, _y)
            if(i < sideModel.rowCount() / 2){
                sld.textRed = sideModel.getNameRegion(i * 2)
                sld.ageRed = sideModel.getAge(i * 2)
                sld.weightRed = sideModel.getWeight(i * 2)
            }else{
                sld.textRed = ""
                sld.ageRed = ""
                sld.weightRed = ""
            }
            if(i <= sideModel.rowCount() / 2 - 1){
                sld.textBlue = sideModel.getNameRegion(i * 2 + 1)
                sld.ageBlue = sideModel.getAge(i * 2 + 1)
                sld.weightBlue = sideModel.getWeight(i * 2 + 1)
            }else{
                sld.textBlue = ""
                sld.ageBlue = ""
                sld.weightBlue = ""
            }

            _y = sldNext.y;
            i = (_y - _y % heightRows) / heightRows
            if(i < sideModel.rowCount() / 2)
                sldNext.textRed = sideModel.getNameRegion(i * 2)
            else
                sldNext.textRed = ""
            if(i <= sideModel.rowCount() / 2 - 1)
                sldNext.textBlue = sideModel.getNameRegion(i * 2 + 1)
            else
                sldNext.textBlue = ""
            sld.setTextOffset()
            sldNext.setTextOffset()
            console.log("updateSlider")
        }

        anchors.fill: parent
        model: sideModel
        cellWidth: parent.width / 2
        cellHeight: heightRows
        clip: true

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
                //context.moveTo(0, 21);
                //context.lineTo(parent.width, 21);
                context.closePath();
                context.stroke();
            }
        }

        delegate: DelegateSide{}

        MouseArea {
            id: dragArea
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.fill: parent
            //acceptedButtons: Qt.LeftButton | Qt.RightButton
            onPressed: {
                //                if (mouse.button === Qt.RightButton) { // 'mouse' is a MouseEvent argument passed into the onClicked signal handler
                //                    //grid.moveItem(index)
                //                    console.log(grid.currentIndex)

                //                }
                mouse.accepted = false
            }

        }

        onFlickStarted: {
            currentYSldider = sld.y + contentY
            currentYSldiderNext = sldNext.y + contentY
        }

        onFlickingChanged: {
            //updateSlider()
            console.log("onFlickingChanged")
        }

        onContentYChanged: {
            //if(contentY)
            //    contentY = 0
            console.log("onContentYChanged = ", contentY, " currentYSldider = ", currentYSldider)
            sld.y = 0; //currentYSldider - contentY
            sldNext.y = heightRows; //currentYSldiderNext - contentY
            //updateSlider()
//            if(contentY == 0)
//                countRemovedRows = 0;
            sliderYChanged(0)
            sliderYChangedNext(heightRows)
            updateSlider()
        }

        onFlickEnded: {
            let remains = contentY % heightRows
            if(remains != 0){
                if(remains < heightRows / 2){
                    contentY = contentY - remains
                }else{
                    contentY = contentY - remains + heightRows
                }
            }
            console.log("end", ' ', contentY, ' ', remains)
            //updateSlider()
        }

        onHeightChanged: {
            sld.maxY = height - heightRows * 2
            sldNext.maxY = height - heightRows
            sliderYChanged(0)
            sliderYChangedNext(heightRows)
        }

        SliderNames{
            id: sld
            objectName: "sld"
            heightRow: heightRows
            szFont: sizeFont
        }

        SliderNamesNext{
            id: sldNext
            objectName: "sldNext"
            heightRow: heightRows
            szFont: sizeFont
        }
    }
}
