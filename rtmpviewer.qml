import QtQuick 2.12
import QtMultimedia

Video {
    id: video
    anchors.fill: parent.fill
    // width:  100
    // height: 75
    source: "rtmp://192.168.1.100/live/stream"
    //source: "F:/Qt_projects/build-TabloRB-Desktop_Qt_6_7_0_MinGW_64_bit-Release/camera1/1.mp4"
    fillMode: VideoOutput.Stretch
    //autoPlay: true
    MouseArea {
            anchors.fill: parent
            onClicked: {
                video.play()
            }
        }


}
