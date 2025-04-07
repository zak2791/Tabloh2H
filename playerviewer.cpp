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

    QString style = ".QSlider {min-height: 30px;"
                    "max-height: 30px;"
                    "}"

                    ".QSlider::groove:horizontal {border: 1px solid #000000;"
                    "height: 5px;"

                    "margin: 0 10px;}"

                    ".QSlider::handle:horizontal {background: rgba(100, 100, 100, 200);"
                    "border: 2px solid #000000;"
                    "border-radius: 5px;"
                    "width: 20px;"

                    "margin: -10px 0;}";

    slider->setStyleSheet(style);

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

    connect(player, SIGNAL(sigImage(QImage)), this, SLOT(draw_image(QImage)));
    connect(player, SIGNAL(sigParam(int,int,int)), this, SLOT(parametersMedia(int,int,int)));
    connect(player, SIGNAL(sigFrame(int)), slider, SLOT(setValue(int)));
    connect(player, SIGNAL(sigBuffer(int,int)), this, SLOT(drawBuffer(int,int)));
    connect(player, SIGNAL(sigStartStopPlay(bool)), this, SLOT(playEnable(bool)));


    btnStream1 = new QPushButton("Камера 1", this);
    connect(btnStream1, &QPushButton::clicked, this, [this](){
        player->setCamera(0);
        btnStream1->setStyleSheet("QPushButton{background-color: green}");
        if(countStreams > 1)
            btnStream2->setStyleSheet("QPushButton{background-color: yellow}");
        if(countStreams > 2)
            btnStream3->setStyleSheet("QPushButton{background-color: yellow}");
    });
    btnStream1->setEnabled(false);
    btnStream1->setStyleSheet("QPushButton{background-color: lightgray}");

    btnStream2 = new QPushButton("Камера 2", this);
    connect(btnStream2, &QPushButton::clicked, this, [this](){
        player->setCamera(1);
        btnStream2->setStyleSheet("QPushButton{background-color: green}");
        btnStream1->setStyleSheet("QPushButton{background-color: yellow}");
        if(countStreams > 2)
            btnStream3->setStyleSheet("QPushButton{background-color: yellow}");
    });
    btnStream2->setEnabled(false);
    btnStream2->setStyleSheet("QPushButton{background-color: lightgray}");

    btnStream3 = new QPushButton("Камера 3",  this);
    connect(btnStream3, &QPushButton::clicked, this, [this](){
        player->setCamera(2);
        btnStream3->setStyleSheet("QPushButton{background-color: green}");
        btnStream1->setStyleSheet("QPushButton{background-color: yellow}");
        btnStream2->setStyleSheet("QPushButton{background-color: yellow}");
     });
    btnStream3->setEnabled(false);
    btnStream3->setStyleSheet("QPushButton{background-color: lightgray}");

    connect(player, &Player::sigCountStreams, this, [this](int count){
        countStreams = count;
        if(count == 1){
            btnStream1->setEnabled(true);
            btnStream1->setStyleSheet("QPushButton{background-color: green}");
        }
        else if(count == 2){
            btnStream1->setEnabled(true);
            btnStream2->setEnabled(true);
            btnStream1->setStyleSheet("QPushButton{background-color: green}");
            btnStream2->setStyleSheet("QPushButton{background-color: yellow}");
        }
        else if(count == 3){
            btnStream1->setEnabled(true);
            btnStream2->setEnabled(true);
            btnStream3->setEnabled(true);
            btnStream1->setStyleSheet("QPushButton{background-color: green}");
            btnStream2->setStyleSheet("QPushButton{background-color: yellow}");
            btnStream3->setStyleSheet("QPushButton{background-color: yellow}");
        }
    });

    thread->start();

    lblTime = new QLabel("time", this);
    lblTime->setStyleSheet("color: red; background-color: white; font-size: 20px");
    connect(player, SIGNAL(sigTime(QString)), lblTime, SLOT(setText(QString)));

    showFullScreen();
    //show();

    //player->turnPlay();


    player->nextFrame();

}

void PlayerViewer::playEnable(bool b){
    slider->setEnabled(b);
}

void PlayerViewer::closeEvent(QCloseEvent*){
    player->turnOff();
    thread->quit();
    thread->wait();
    emit sigClose();
}

void PlayerViewer::resizeEvent(QResizeEvent*){
    qDebug()<<"resize0";
    slider->setGeometry(0, height() - 100, width(), 30);
    btnPlay->setGeometry(10, height() - 55, 100, 40);
    btnNextFrame->setGeometry(230, height() - 55, 100, 40);
    btnPreviewFrame->setGeometry(120, height() - 55, 100, 40);
    btnClose->setGeometry(width() - 110, height() - 55, 100, 40);
    qDebug()<<"resize1";
    btnStream1->setGeometry(400, height() - 55, 100, 40);
    qDebug()<<"resize2";
    btnStream2->setGeometry(520, height() - 55, 100, 40);
    qDebug()<<"resize3";
    btnStream3->setGeometry(640, height() - 55, 100, 40);
    qDebug()<<"resize4";
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
    lblTime->adjustSize();
    emit sigImage(img);
}

void PlayerViewer::parametersMedia(int numberFrames, int avgFps, int durationMediaInSecunds){
    qDebug()<<"!3";
    maxLenghtBuffer = avgFps * 2;
    qDebug()<<"!4";
    slider->setMaximum(numberFrames);
    qDebug()<<"!5";
    slider->setTickPosition(QSlider::TicksAbove);
    if(durationMediaInSecunds < 60)
        slider->setTickInterval(avgFps);        //tick every second
    else
        slider->setTickInterval(avgFps * 60);   //tick every minute
    qDebug()<<"!6";
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
