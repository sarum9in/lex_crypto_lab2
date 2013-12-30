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
                    text: "Key"
                }
                TextField {
                    id: key
                    Layout.fillWidth: true
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                }
                Label {
                    text: "Mode of operation"
                }
                ComboBox {
                    id: modeOfOperation
                    onCurrentIndexChanged: ivLabel.visible = iv.visible = currentIndex
                    model: [
                        "ECB",
                        "CBC"
                    ]
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
                Label {
                    text: "Key derivation"
                }
                ComboBox {
                    id: keyDerivationFunction
                    model: [
                        "MD5",
                        "SHA-256",
                        "SHA-512"
                    ]
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
                    id: cipherText
                    wrapMode: TextEdit.WrapAnywhere
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                Button {
                    id: crypt
                    objectName: "crypt"
                    text: "Crypt"
                    Layout.fillWidth: true

                    signal crypt(string keyDerivationFunction,
                                 string key,
                                 string modeOfOperation,
                                 string iv,
                                 string text)
                    signal crypted(string t)
                    onCrypted: cipherText.text = t
                    onClicked: crypt.crypt(
                                   keyDerivationFunction.currentText,
                                   key.text,
                                   modeOfOperation.currentText,
                                   iv.text,
                                   text.text)
                }
                Button {
                    id: decrypt
                    objectName: "decrypt"
                    text: "Decrypt"
                    Layout.fillWidth: true

                    signal decrypt(string keyDerivationFunction,
                                   string key,
                                   string modeOfOperation,
                                   string iv,
                                   string cipherText)
                    signal decrypted(string t)

                    onDecrypted: text.text = t
                    onClicked: decrypt.decrypt(keyDerivationFunction.currentText,
                                               key.text,
                                               modeOfOperation.currentText,
                                               iv.text,
                                               cipherText.text)
                }
            }
        }
    }
}
