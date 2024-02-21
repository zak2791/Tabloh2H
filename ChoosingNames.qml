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



                    TextInput {
                        id: ti
                        width: 20
                        font.pointSize: 15
                        focus: true
                        text: "Text"
                        cursorVisible: true
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
            Layout.preferredHeight: 19

            Layout.columnSpan: 3
            Layout.rowSpan: 19

            Layout.column: 8
            Layout.row: 2
//            TableChoosingNames {
//                objectName: "tableSide"
//            }
            TableSide {
                objectName: "tableSide"
            }
        }

    }

}
