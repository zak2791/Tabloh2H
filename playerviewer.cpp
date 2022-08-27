#include "playerviewer.h"
#include <QLabel>
#include <QDebug>

PlayerViewer::PlayerViewer(QString file, QWidget* parent) : QGraphicsView(parent){
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    videoFile = file;

    scene = new QGraphicsScene(this);
    pi = new QGraphicsPixmapItem();

    sizeBufferFrames = new QFrame(this);
    sizeBufferFrames->setFrameShape(QFrame::HLine);
    sizeBufferFrames->setGeometry(0, 0, 0, 0);
    sizeBufferFrames->setLineWidth(4);

    markerCurrentFrame = new QFrame(this);
    markerCurrentFrame->setFrameShape(QFrame::Box);
    markerCurrentFrame->setStyleSheet("QFrame{color: red; border: 2px solid red; border-radius: 6px;}");

    slider = new QSlider( Qt::Horizontal, this);
    slider->setFocusPolicy(Qt::NoFocus);
    slider->setMaximum(300);

    slider->setTickPosition(QSlider::TicksAbove);
    slider->setTickInterval(60);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(setSeek(int)));

    btnPlay = new QPushButton("Play/Stop", this);
    btnPlay->setFocusPolicy(Qt::NoFocus);
    connect(btnPlay,         SIGNAL(clicked()), this, SLOT(turnPlay()));

    btnNextFrame = new QPushButton(">", this);
    btnNextFrame->setFocusPolicy(Qt::NoFocus);
    connect(btnNextFrame,    SIGNAL(clicked()), this, SLOT(nextFrame()));

    btnPreviewFrame = new QPushButton("<", this);
    btnPreviewFrame->setFocusPolicy(Qt::NoFocus);
    connect(btnPreviewFrame, SIGNAL(clicked()), this, SLOT(previewFrame()));

    btnClose = new QPushButton("Close", this);
    btnClose->setFocusPolicy(Qt::NoFocus);
    connect(btnClose, SIGNAL(clicked()), this, SLOT(close()));

    player = new Player(videoFile);
    thread = new QThread();
    player->moveToThread(thread);

    connect(thread, SIGNAL(started()), player, SLOT(Play()));

    connect(player, SIGNAL(sigImage(QImage)), this, SLOT(draw_image (QImage)));
    connect(player, SIGNAL(sigParam(int, int, int)), this, SLOT(parametersMedia(int, int, int)));
    connect(player, SIGNAL(sigFrame(int)), slider, SLOT(setValue(int)));
    connect(player, SIGNAL(sigBuffer(int, int)), this, SLOT(drawBuffer(int, int)));

    thread->start();

    showFullScreen();

    player->turnPlay();

}

void PlayerViewer::closeEvent(QCloseEvent*){
    player->turnOff();
    thread->quit();
    thread->wait();
    emit sigClose();
}

void PlayerViewer::resizeEvent(QResizeEvent*){
    slider->setGeometry(0, height() - 100, width(), 30);
    btnPlay->setGeometry(10, height() - 55, 100, 40);
    btnNextFrame->setGeometry(230, height() - 55, 100, 40);
    btnPreviewFrame->setGeometry(120, height() - 55, 100, 40);
    btnClose->setGeometry(width() - 110, height() - 55, 100, 40);
}

void PlayerViewer::setSeek(int s){
    player->seek(s);
}

void PlayerViewer::draw_image(QImage img){
    if(pi)
        delete pi;
    pi = scene->addPixmap(QPixmap::fromImage(img.scaled(this->width(), this->height() - 100)));
    scene->setSceneRect(this->rect());
    setScene(scene);
}

void PlayerViewer::parametersMedia(int numberFrames, int avgFps, int durationMediaInSecunds){
    maxLenghtBuffer = avgFps * 2;
    slider->setMaximum(numberFrames);

    slider->setTickPosition(QSlider::TicksAbove);
    if(durationMediaInSecunds < 60)
        slider->setTickInterval(avgFps);        //tick every second
   else
        slider->setTickInterval(avgFps * 60);   //tick every minute
}

void PlayerViewer::turnPlay(){
    player->turnPlay();
}

void PlayerViewer::nextFrame(){
    player->nextFrame();
}

void PlayerViewer::previewFrame(){
    player->previewFrame();
}

void PlayerViewer::keyPressEvent(QKeyEvent* e){
    if(e->key() == Qt::Key_Left)
        player->previewFrame();
    else if (e->key() == Qt::Key_Right)
        player->nextFrame();
}

void PlayerViewer::drawBuffer(int lenBuf, int posBuf){
    int x = 5;
    int y =  height() - 68;
    int w = (width() - 2 * x) * (lenBuf - 1) / (maxLenghtBuffer - 1);
    int h = 4;
    sizeBufferFrames->setGeometry(x, y, w, h);
    markerCurrentFrame->setGeometry((width() - 10) * posBuf / (maxLenghtBuffer - 1) + x - 7, y - 4, 12, 12);
}
