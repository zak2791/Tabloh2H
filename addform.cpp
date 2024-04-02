#include "addform.h"
#include "ui_addform.h"

#include <QDebug>
#include "listview.h"
#include <QMessageBox>

ListView::ListView(QWidget * parent) : QListView(parent) {

}

ListView::~ListView()
{
}

void ListView::mouseReleaseEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
        emit mouse_button(0);
    else if (e->button() == Qt::RightButton)
        emit mouse_button(1);
}

addForm::addForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addForm)
{
    ui->setupUi(this);

#ifdef APP_LAUNCH_FROM_IDE
    settings = new QSettings("settings.ini", QSettings::IniFormat);
#else
    settings = new QSettings("bin/settings.ini", QSettings::IniFormat);
#endif

    settings->beginGroup("addedSportsmens");
    sportsmens = settings->value("sportsmens", "").toString();
    settings->endGroup();

    QList<QString> lSportsmens;
    if(sportsmens != "")
        lSportsmens = sportsmens.split(";");

    leName = ui->leName;
    leTeam = ui->leTeam;

    listView = ui->list;

    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(addSportsman()));

    ui->hBox->setStretchFactor(ui->vbName, 5);
    ui->hBox->setStretchFactor(ui->vbTeam, 4);

    model = new QStandardItemModel;
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    foreach(QString str, lSportsmens)
        model->appendRow(new QStandardItem(str));

    listView->setModel(model);

    connect(listView, SIGNAL(mouse_button(int)), this, SLOT(mouse()));
    connect(listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(sel(QItemSelection, QItemSelection)));
    connect(ui->btnClear, SIGNAL(clicked()), this, SLOT(clearList()));
}

addForm::~addForm()
{
    delete ui;
}

void addForm::addSportsman(){
    if(leName->text() == "" || leTeam->text() == "")
        return;

    model->appendRow(new QStandardItem(leName->text() + "," + leTeam->text()));
    listView->setModel(model);

    if(sportsmens == "")
        sportsmens = leName->text() + "," + leTeam->text();
    else
        sportsmens = sportsmens + ";" + leName->text() + "," + leTeam->text();

    settings->beginGroup("addedSportsmens");
    settings->setValue("sportsmens", sportsmens);
    settings->endGroup();

}

void addForm::mouse(){
    emit selSportsman(selectedSportsmen);
}

void addForm::sel(QItemSelection s, QItemSelection){
    QList<QString> lS = s.indexes()[0].data().toString().split(",");
    selectedSportsmen = lS.at(0) + ";" + lS.at(1);
}

void addForm::clearList(){
    int ret = QMessageBox::warning(this, tr("Очистка списка"),
                                   tr("Вы уверены?"),
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if(ret == QMessageBox::Cancel)
        return;

    model->clear();
    //model->appendRow(new QStandardItem("-"));

    sportsmens = "";

    settings->beginGroup("addedSportsmens");
    settings->setValue("sportsmens", sportsmens);
    settings->endGroup();

    listView->setModel(model);

}
