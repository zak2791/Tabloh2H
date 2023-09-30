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
}

HelpForm::~HelpForm()
{
    delete ui;
}

void HelpForm::on_btnClose_clicked()
{
    close();
}

