#include "helpform.h"
#include "ui_helpform.h"

//#include <QDebug>

HelpForm::HelpForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpForm)
{
    ui->setupUi(this);

#ifdef APP_LAUNCH_FROM_IDE
    QString fileHelp = "docs/help.html";
#else
    QString fileHelp = "bin/docs/help.html";
#endif

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

