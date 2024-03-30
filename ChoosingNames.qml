import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    GridLayout{
        id: grid
        columns: 10
        rows: 20
        anchors.fill: parent
        anchors.margins: 10

        Rectangle {
            color: "antiquewhite"
            Layout.fillHeight: true
            Layout.fillWidth: true

            Layout.preferredWidth: 10
            Layout.preferredHeight: 1

            Layout.columnSpan: 10
            Layout.rowSpan: 1

            Layout.column: 1
            Layout.row: 1



            RowLayout {
                height:  parent.height
                x: 0
                y: 0
                spacing: 6
                RowLayout {
                    spacing: 6
                    Text{

                        text: "Возраст:"
                    }

                    ComboBox {
                        id: cmbAge
                        focus: false
                        objectName: "cmbAge"
                        signal ageClicked(string str)
                        function setAge(age){
                            model = age
                        }
                        onActivated: {
                            ageClicked(cmbAge.currentText)
                        }


                    }
                }
                RowLayout {
                    spacing: 6
                    Text{
                        text: "Вес:"
                    }

                    ComboBox {
                        id: cmbWeight

                        focus: false
                        objectName: "cmbWeight"
                        signal weightClicked(string str)
                        function setWeight(weight){
                            model = weight
                        }
                        onActivated: {
                            weightClicked(cmbWeight.currentText)
                        }
                    }
                }
                RowLayout {
                    spacing: 6
                    Text{
                        text: "Фильтр по фамилии:"
                    }

                Rectangle {
                    width: 150
                    height: 30
                    color: "white"
                    border.color: "gray"

                    TextInput {
                        id: ti
                        objectName: "tiName"
                        signal filterName(string str)
                        anchors.fill: parent
                        anchors.margins: 2
                        width: 20
                        font.pointSize: 15
                        focus: true

                        //text: "Text"
                        onTextEdited: filterName(text)

                        cursorVisible: true

                    }
                }
                }
            }

        }

        Rectangle {
            color: "antiquewhite"
            Layout.fillHeight: true
            Layout.fillWidth: true

            Layout.preferredWidth: 7
            Layout.preferredHeight: 19

            Layout.columnSpan: 7
            Layout.rowSpan: 19

            Layout.column: 1
            Layout.row: 2

            TableAllSportsmens {
                objectName: "tableAll"

            }
        }

        Rectangle {
            color: "antiquewhite"
            Layout.fillHeight: true
            Layout.fillWidth: true

            Layout.preferredWidth: 3
            Layout.preferredHeight: 18

            Layout.columnSpan: 3
            Layout.rowSpan: 18

            Layout.column: 8
            Layout.row: 2
            TableSide {
                id: tableSide
                objectName: "tableSide"
            }
        }

        Rectangle {
            color: "antiquewhite"
            Layout.fillHeight: true
            Layout.fillWidth: true

            Layout.preferredWidth: 3
            Layout.preferredHeight: 1

            Layout.columnSpan: 3
            Layout.rowSpan: 1

            Layout.column: 8
            Layout.row: 20

            Button {
                id: btnDel
                objectName: "button"
                text: "Удалить отработанные фамилии"
                anchors.centerIn: parent
                onClicked: {
                    tableSide.delUpper()
                }
            }
        }

    }

}
