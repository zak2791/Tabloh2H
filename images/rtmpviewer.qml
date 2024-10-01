import QtQuick 2.15
import QtMultimedia

Video {
    id: video
    // width : 800
    // height : 600
    source: "rtmp://127.0.0.1/live/stream"
    autoPlay: true
    fillMode: Image.Stretch
    // MouseArea {
    //     anchors.fill: parent
    //     onClicked: {
    //         video.play()
    //     }
    // }

    // focus: true
    // Keys.onSpacePressed: video.playbackState == MediaPlayer.PlayingState ? video.pause() : video.play()
    // Keys.onLeftPressed: video.position = video.position - 5000
    // Keys.onRightPressed: video.position = video.position + 5000
}
