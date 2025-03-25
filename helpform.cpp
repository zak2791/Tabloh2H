#include "helpform.h"
#include "ui_helpform.h"

//#include <QDebug>

HelpForm::HelpForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpForm)
{
    ui->setupUi(this);


    QString fileHelp = "docs/help.html";


    help = ui->textBrowser;
    help->setSource(QUrl::fromLocalFile(fileHelp));
    setWindowFlags(Qt::WindowStaysOnTopHint);
    //help->setOpenExternalLinks(false);
    //connect(help, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchors_clicked(QUrl)));

}

HelpForm::~HelpForm()
{
    delete ui;
}

void HelpForm::on_btnClose_clicked()
{
    close();
}

// void HelpForm::anchors_clicked(QUrl url)
// {
//     qDebug()<<url;
// }

