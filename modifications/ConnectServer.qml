import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.1

import QGroundControl 1.0
import QGroundControl.Controls      1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controllers   1.0
import QGroundControl.ScreenTools   1.0


Rectangle{

        id:myrect
        anchors.fill: parent
        z:1
        color: "lightgrey"
        visible: true
        Text {
           id: load
           text: qsTr("Connecting To Server")
           font.bold: true
           font.pixelSize: myrect.width/50
           anchors.centerIn: parent
           anchors.verticalCenterOffset: - myrect.height/10

        }

Image {
    id: logo
    source: "qrc:/qmlimages/modifications/aerobridge.png"
    fillMode: Image.PreserveAspectFit
    width: parent.width
    height: parent.height/3

}

AnimatedImage{
    id: gif
    source: "qrc:/qmlimages/modifications/loading.gif"
    fillMode: Image.PreserveAspectFit
    anchors.centerIn: parent
    anchors.verticalCenterOffset: myrect.height/10
}

}


