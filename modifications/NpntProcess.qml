import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.1

import QGroundControl 1.0


 Rectangle{
        id:myrect
        property bool hardwareConnected:false
        property bool droneIsActive:false
        property bool keyRotationCompleted:false
        anchors.fill: parent
        color: "lightgrey"
        z:1
        Component.onCompleted: {
            npntcontroller.checkdeviceConnected();
        }

    Rectangle{
        id: rect1
        width:parent.width/2.5
        height:parent.height/1.5
        anchors.centerIn: parent
        radius: 80
        color:"lightgrey"

        MouseArea{
            anchors.fill:parent
            z:2
        }

        Column{
            spacing: rect1.height/10
            anchors.centerIn: rect1

           //START NPNT CHECKLIST
            Column{
            Text {
                id: init
                text: qsTr("STARTING INIT PROCESS..")
                font.bold: true
                font.pixelSize:(rect1.width/25 + rect1.height/25 )/2
            }
            }

            Column{
                    CheckBox {
                        id: c1
                        checked:myrect.hardwareConnected
                        text: qsTr("Hardware Connected")
                        font.pixelSize: (rect1.width/25 + rect1.height/25 )/2
                        checkable:false
                        indicator.width:rect1.width/20
                        indicator.height:rect1.height/20

                    }
                    CheckBox {
                        id:c2
                        checked: myrect.droneIsActive
                        text: qsTr("Check if Drone is Active")
                        font.pixelSize: (rect1.width/25 + rect1.height/25 )/2
                        checkable:false
                        indicator.width:rect1.width/20
                        indicator.height:rect1.height/20
                    }
                    CheckBox {
                        id:c3
                        checked: myrect.keyRotationCompleted
                        text: qsTr("Starting Key Rotation")
                        checkable:false
                        font.pixelSize: (rect1.width/25 + rect1.height/25 )/2
                        indicator.width:rect1.width/20
                        indicator.height:rect1.height/20

                    }
                    Timer {
                        interval: 10000//10s
                        onTriggered: text1.visible = true
                        running: true
                    }
                    Text {
                        id:text1
                        y:parent.height+100
                        visible: false
                        text: (myrect.hardwareConnected?"":"Error:Failed To Connect Hardware")
                        font.pixelSize:(rect1.width/25 + rect1.height/25 )/2
                        color: "red"
                    }
                    Timer {
                        interval: 14000//10s
                        onTriggered: text2.visible = true
                        running:(myrect.hardwareConnected?true:false)

                    }
                    Text{
                        id:text2
                        y:parent.height+100
                        visible: false
                        text: (myrect.droneIsActive?"":"Error:Drone is not active")
                        font.pixelSize:(rect1.width/25 + rect1.height/25)/2
                        color: "red"

                    }
                    Timer {
                        interval: 14000//10s
                        onTriggered: text3.visible = true
                        running:(!(myrect.hardwareConnected)?false:(myrect.droneIsActive?true:false))
                    }
                    Text {
                        y:parent.height+100
                        id:text3
                        visible: false
                        text: (myrect.keyRotationCompleted?"":"Error:Unable to rotate key")
                        font.pixelSize:(rect1.width/25 + rect1.height/25)/2
                        color: "red"

                    }

            }

        }

    }
    Image {
        id: logo
        source: "qrc:/qmlimages/modifications/aerobridge.png"
        fillMode: Image.PreserveAspectFit
        width: parent.width
        height: parent.height/3

    }

 }

