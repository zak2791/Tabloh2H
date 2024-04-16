
#include "choosingnames.h"
#include <QQuickItem>
//#include <QJsonObject>
//#include <QJsonDocument>
#include <QQmlContext>
#include <QStringListModel>

ChoosingNames::ChoosingNames(QWidget *parent) : QWidget(parent)
{
    quickWidget = new QQuickWidget(this) ;
    quickWidget->setSource(QUrl("qrc:/ChoosingNames.qml"));
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    model = new MainListModel;
    QQmlContext* pcon = quickWidget->rootContext();

    proxyAge = new QSortFilterProxyModel;
    proxyAge->setSourceModel(model);
    proxyAge->setFilterRole(MainListModel::AgeRole);

    proxyWeight = new QSortFilterProxyModel;
    proxyWeight->setSourceModel(proxyAge);
    proxyWeight->setFilterRole(MainListModel::WeightRole);

    proxyName = new QSortFilterProxyModel;
    proxyName->setSourceModel(proxyWeight);
    //proxyName->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyName->setFilterRole(MainListModel::NameRole);
    proxyName->setSortRole(MainListModel::NameRole);

    pcon->setContextProperty("myModel", proxyName);

    slmodel = new SideListModel;
    pcon->setContextProperty("sideModel", slmodel);

    fAdd = new addForm;

    QQuickItem* qItem = quickWidget->rootObject();
    if(qItem){

        auto mainGrid = qItem->findChild<QObject*>("mainGrid");
        if(mainGrid)
            connect(mainGrid, SIGNAL(addSportsman()), fAdd, SLOT(show()));

        objAge = qItem->findChild<QObject*>("cmbAge");
        if(objAge)
            connect(objAge, SIGNAL(ageClicked(QString)), this, SLOT(choiceAge(QString)));

        objWeight = qItem->findChild<QObject*>("cmbWeight");
        if(objWeight)
            connect(objWeight, SIGNAL(weightClicked(QString)), this, SLOT(choiceWeight(QString)));

        objFilterNames = qItem->findChild<QObject*>("tiName");
        if(objFilterNames)
            connect(objFilterNames, SIGNAL(filterName(QString)), this, SLOT(filterName(QString)));

        QObject* objTable = qItem->findChild<QObject*>("tableAll");
        if(objTable){
            QObject* objGrid = qItem->findChild<QObject*>("grid");
            if(objGrid){
                connect(objGrid, SIGNAL(moveItem(int)), this, SLOT(fromAllToSide(int)));
            }
        }

        objGridSide = qItem->findChild<QObject*>("gridSide");
        if(objGridSide){
            connect(objGridSide, SIGNAL(swapItems(int, int)), slmodel, SLOT(swapData(int, int)));
            connect(objGridSide, SIGNAL(moveItem(int)), this, SLOT(fromSideToAll(int)));
            //connect(objGridSide, SIGNAL(moveItems(int)), this, SLOT(fromSideToAllManyItems(int)));
        }
    }
    connect(fAdd, SIGNAL(selSportsman(QString)), this, SLOT(addSportsman(QString)));
    //setWindowFlags(Qt::Widget | Qt::WindowCloseButtonHint);
}

void ChoosingNames::setNames(QStringList list){
    model->clearModel();
    model->setList(list);
    slmodel->clearModel();
    QMetaObject::invokeMethod(objGridSide, "updateSlider");
    proxyAge->setFilterRegExp("");
    proxyWeight->setFilterRegExp("");
}

void ChoosingNames::setAge(QStringList list){
    QVariant varRet;
    QMetaObject::invokeMethod(objAge, "setAge",
                              Q_RETURN_ARG(QVariant, varRet),
                              Q_ARG(QVariant, list));
}

void ChoosingNames::setWeight(QStringList list){
    QVariant varRet;
    QMetaObject::invokeMethod(objWeight, "setWeight",
                              Q_RETURN_ARG(QVariant, varRet),
                              Q_ARG(QVariant, list));
}



void ChoosingNames::resizeEvent(QResizeEvent* e){
    quickWidget->resize(e->size());
}

void ChoosingNames::closeEvent(QCloseEvent *){
    auto slider = objGridSide->findChild<QObject*>("sld");
    QStringList strRed = slider->property("textRed").toString().split(";");
    QString NameRed(""), NameBlue(""), RegionRed(""), RegionBlue("");
    if(strRed.length() == 2){
        NameRed = strRed[0].toUpper();
        RegionRed = strRed[1].toUpper();
    }
    QStringList strBlue = slider->property("textBlue").toString().split(";");
    if(strBlue.length() == 2){
        NameBlue = strBlue[0].toUpper();
        RegionBlue = strBlue[1].toUpper();
    }

    auto sliderNext = objGridSide->findChild<QObject*>("sldNext");
    QStringList strRedNext = sliderNext->property("textRed").toString().split(";");
    QString NameRedNext(""), NameBlueNext("");
    if(strRedNext.length() == 2){
        NameRedNext = strRedNext[0].toUpper();
    }
    QStringList strBlueNext = sliderNext->property("textBlue").toString().split(";");
    if(strBlueNext.length() == 2){
        NameBlueNext = strBlueNext[0].toUpper();
    }

    QString age = slider->property("ageRed").toString();
    if(age == "")
        age = slider->property("ageBlue").toString();

    QString weight = slider->property("weightRed").toString();
    if(weight == "")
        weight = slider->property("weightBlue").toString();
    emit close(NameRed, RegionRed, NameBlue, RegionBlue, NameRedNext, NameBlueNext, age, weight);
}

void ChoosingNames::choiceAge(QString age){
    proxyAge->setFilterRegExp(age);
}

void ChoosingNames::choiceWeight(QString weight){
    proxyWeight->setFilterRegExp(weight);

}

void ChoosingNames::filterName(QString name){
    QRegExp regExp( "^" + name, Qt::CaseInsensitive );
    proxyName->setFilterRegExp(regExp);
}

void ChoosingNames::fromAllToSide(int item){
    auto modIndexName =  proxyName ->mapToSource(proxyName->index(item, 0, QModelIndex()));
    auto modIndexWeight = proxyWeight->mapToSource(modIndexName);
    auto modIndexAge = proxyAge->mapToSource(modIndexWeight);
    QString str = model->data(modIndexAge, model->NameRole).toString()   + "\n" +
            model->data(modIndexAge, model->RegionRole).toString() + "\n" +
            model->data(modIndexAge, model->AgeRole).toString()    + "\n" +
            model->data(modIndexAge, model->WeightRole).toString();
    model->moveItem(modIndexAge.row());
    slmodel->insertData(str);
    QVariant varRet;
    QMetaObject::invokeMethod(objGridSide, "updateSlider");
}

void ChoosingNames::fromSideToAll(int item){
    auto mod = slmodel->index(item, 0, QModelIndex());
    QString str = slmodel->data(mod, model->NameRole).toString()   + "\n" +
            slmodel->data(mod, model->RegionRole).toString() + "\n" +
            slmodel->data(mod, model->AgeRole).toString()    + "\n" +
            slmodel->data(mod, model->WeightRole).toString();

    if(slmodel->data(mod, model->AgeRole).toString() != "")
        model->insertData(str);
    slmodel->removeData(item);

    proxyName->sort(0);
    QMetaObject::invokeMethod(objGridSide, "updateSlider");
}

void ChoosingNames::addSportsman(QString sportsman){
    QStringList lStr = sportsman.split(";");
    if(lStr.length() != 2)
        return;
    QString str = lStr[0] + "\n" + lStr[1]  + "\n" +  + "\n";
    slmodel->insertData(str);
    QMetaObject::invokeMethod(objGridSide, "updateSlider");
}

void ChoosingNames::removeSportsman(int){

}
