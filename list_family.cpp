#include "list_family.h"

#include <QDir>
#include <QMessageBox>
#include "pcscreen.h"

ListFamily::ListFamily(QWidget * parent) : QWidget(parent) {

    p = parent;

}

ListFamily::~ListFamily()
{
}

QStringList ListFamily::getSportsmens(){
    QStringList list;

    readFileSportsmens();

    foreach(QString s, sportsmens){
        s = s.remove(s.lastIndexOf(";"), 5);
        s.replace(";", "\n");
        list.append(s);
    }
    list.sort();
//    if(list.length() == 0)
//        list.append("\n\n\n\n");
    return list;
}

void ListFamily::readFileSportsmens(){
    QDir dir = QDir::current();

    QString path = static_cast<PCScreen*>(p)->mainwin->getFileSportsmens();

    //doc = new Document(path);
    Document doc(path);
    if (!doc.load()){
        QMessageBox::warning(this, "Внимание!", "Не найден файл со списком спортсменов", QMessageBox::Ok);
    }

    doc.workbook()->setActiveSheet(0);

    QVector<CellLocation> clList;
    int maxRow = -1;
    int maxCol = -1;
    Worksheet* wsheet = (Worksheet*)doc.workbook()->activeSheet();


    clList = wsheet->getFullCells( &maxRow, &maxCol );

    sportsmens.clear();
    lAge.clear();
    lWeight.clear();

    for(int i = 1; i <= maxRow; i++){
        if(doc.read(i, 1).toString() == "") break;
        sportsmens.append(doc.read(i, 1).toString() + ";"
                          + doc.read(i, 2).toString() + ";"
                          + doc.read(i, 3).toString() + ";"
                          + doc.read(i, 4).toString() + ";"
                          + doc.read(i, 5).toString());
        //l.append(doc.read(i, 1).toString() + ";" + doc.read(i, 2).toString() + ";" + doc.read(i, 5).toString());
        lAge.append(doc.read(i, 3).toString());
        lWeight.append(doc.read(i, 4).toString());
    }
    lAge.append("");
    lWeight.append("");
    lAge.removeDuplicates();
    lWeight.removeDuplicates();

    lAge.sort();

    lWeight.sort();

    //l.sort();

    //delete doc;
}
