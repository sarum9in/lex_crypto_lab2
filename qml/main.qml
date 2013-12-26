import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1

ApplicationWindow {
    title: "Lab 2: DES"
    visible: true
    minimumWidth: 640
    minimumHeight: 480

    menuBar: MenuBar {
        id: menuBar
        Menu {
            title: "File"
            MenuItem {
                text: "&Quit"
                onTriggered: Qt.quit()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4

        GroupBox {
            title: "Settings"
            id: settingsBox
            Layout.fillWidth: true
            GridLayout {
                columns: 2
                anchors.fill: parent
                Label {
                    id: shiftLabel
                    text: "Key"
                }
                TextField {
                    id: key
                    Layout.fillWidth: true
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                }
                Label {
                    id: ivLabel
                    text: "IV"
                }
                TextField {
                    id: iv
                    Layout.fillWidth: true
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                }
            }
        }

        GroupBox {
            title: "Data"
            Layout.fillWidth: true
            Layout.fillHeight: true

            GridLayout {
                anchors.fill: parent
                columns: 2
                TextArea {
                    wrapMode: TextEdit.WordWrap
                    id: text
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                TextArea {
                    id: cypherText
                    wrapMode: TextEdit.WordWrap
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                Button {
                    id: crypt
                    text: "Crypt"
                    Layout.fillWidth: true
                }
                Button {
                    id: decrypt
                    text: "Decrypt"
                    Layout.fillWidth: true
                }
            }
        }
    }
}
