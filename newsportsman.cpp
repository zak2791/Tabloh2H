#include "newsportsman.h"

#include <QDir>
#include <QDebug>
#include <QMessageBox>

NewSportsman::NewSportsman(QWidget *parent) : QWidget(parent)
{

    setupUi(this);
    setWindowModality(Qt::WindowModality::ApplicationModal);
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(btnAdd, SIGNAL(clicked()), this, SLOT(addSportsman()));

    QDir dir = QDir::current();

    QString path(dir.path() + "/sportsmens.xlsx");

    QString path1(dir.path() + "/bin");
    dir.setCurrent(path1);  //закомментить для работы из Qt Creator

    doc = new Document(path);
    if (doc->load()){

    }

    doc->workbook()->setActiveSheet(0);



    //QVector<CellLocation> clList = wsheet->getFullCells( &maxRow, &maxCol );

}

void NewSportsman::addSportsman(){
    if(leName->text().remove(" ") == "" || leRegion->text().remove(" ") == "" || leAge->text().remove(" ") == "" || leWeight->text().remove(" ") == ""){
        QMessageBox::warning(this, "Внимание!", "Заполните все поля", QMessageBox::Ok);
        return;
    }
    doc->write(maxRow + 1, 1, leName->text());
    doc->write(maxRow + 1, 2, leRegion->text());
    doc->write(maxRow + 1, 3, leAge->text());
    doc->write(maxRow + 1, 4, leWeight->text());
    doc->write(maxRow + 1, 5, maxRow + 1);
    doc->save();
    close();
}

void NewSportsman::showEvent(QShowEvent*){
    leAge->clear();
    leWeight->clear();
    leRegion->clear();
    leName->clear();
    maxRow = -1;
    maxCol = -1;
    wsheet = (Worksheet*)doc->workbook()->activeSheet();
    QVector<CellLocation> clList = wsheet->getFullCells( &maxRow, &maxCol );
    qDebug()<<maxRow;
    lblNumber->setText(QString::number(maxRow));
}
