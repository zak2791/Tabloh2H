
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
    model = new MainListModel;
    QQmlContext* pcon = quickWidget->rootContext();

    proxyAge = new QSortFilterProxyModel;
    proxyAge->setSourceModel(model);
    proxyAge->setFilterRole(MainListModel::AgeRole);

    proxyWeight = new QSortFilterProxyModel;
    proxyWeight->setSourceModel(proxyAge);
    proxyWeight->setFilterRole(MainListModel::WeightRole);

    pcon->setContextProperty("myModel", proxyWeight);

    slmodel = new SideListModel;
    pcon->setContextProperty("sideModel", slmodel);

    QQuickItem* qItem = quickWidget->rootObject();
    if(qItem){
        objAge = qItem->findChild<QObject*>("cmbAge");
        if(objAge)
            connect(objAge, SIGNAL(ageClicked(QString)), this, SLOT(choiceAge(QString)));

        objWeight = qItem->findChild<QObject*>("cmbWeight");
        if(objWeight)
            connect(objWeight, SIGNAL(weightClicked(QString)), this, SLOT(choiceWeight(QString)));

        QObject* objTable = qItem->findChild<QObject*>("tableAll");
        if(objTable){
            QObject* objGrid = qItem->findChild<QObject*>("grid");
            if(objGrid){
                //objDelegate = qItem->findChild<QObject*>("delegate");
                //qDebug() << "objDelegate "<<objDelegate;
                connect(objGrid, SIGNAL(moveItem(int)), this, SLOT(removeItem(int)));
            }
        }

//        QObject* objTableSide = qItem->findChild<QObject*>("tableSide");
//        if(objTable){
//            QObject* objGrid = qItem->findChild<QObject*>("grid");
//            if(objGrid){
//                //objDelegate = qItem->findChild<QObject*>("delegate");
//                //qDebug() << "objDelegate "<<objDelegate;
//                connect(objGrid, SIGNAL(moveItem(int)), this, SLOT(removeItem(int)));
//            }
//        }
    }

}

void ChoosingNames::setNames(QStringList list){
    //    QQuickItem* qItem = quickWidget->rootObject();
    //    if(qItem){
    //        QObject* obj = qItem->findChild<QObject*>("tableAll");
    //        //obj->setProperty("color", "blue");
    //        QObject* objList = obj->findChild<QObject*>("listModel");


    //        //objGrid->setProperty("model", QVariant::fromValue(list));

    //        if (objList) {
    //            QVariant varRet;
    //            QJsonObject object;
    //            QList<QJsonObject> lgo;
    //            foreach(QString s, list){
    //                object["name"] = s.split("\n")[0];
    //                object["region"] = s.split("\n")[1];
    //                object["weight"] = s.split("\n")[2];
    //                object["age"] = s.split("\n")[3];
    //                lgo.append(object);
    //            }
    model->setList(list);
    //qDebug()<<model->rowCount();
    //objList->setProperty("model", QVariant::fromValue(lgo));
    //            QMetaObject::invokeMethod(objList,
    //                                      "setData",
    //                                      Q_RETURN_ARG(QVariant, varRet),
    //                                      Q_ARG(QVariant, object)
    //                                      );
    //        }
    //    }
}

void ChoosingNames::setAge(QStringList list){
    //    connect(this,   SIGNAL(setAge(QString) ),
    //            obj,    SLOT(ageClicked(QString)));
    QVariant varRet;
    //list.prepend(" ");
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
    quickWidget->rootObject()->setWidth(e->size().width());
    quickWidget->rootObject()->setHeight(e->size().height());
}

void ChoosingNames::choiceAge(QString age){
    proxyAge->setFilterRegExp(age);
}

void ChoosingNames::choiceWeight(QString weight){
    proxyWeight->setFilterRegExp(weight);

}

void ChoosingNames::removeItem(int item){
    qDebug() << "item = " << item;
    auto modIndexWeight = proxyWeight->mapToSource(proxyWeight->index(item, 0, QModelIndex()));
    qDebug() << "modIndexWeight = " << modIndexWeight.row();
    auto modIndexAge = proxyAge->mapToSource(modIndexWeight);
    qDebug() << "modIndexAge = " << modIndexAge.row();

    QString str = model->data(modIndexAge, model->NameRole).toString()   + "\n" +
                  model->data(modIndexAge, model->RegionRole).toString() + "\n" +
                  model->data(modIndexAge, model->AgeRole).toString()    + "\n" +
                  model->data(modIndexAge, model->WeightRole).toString();
    qDebug() << "model->data = " << str;

    model->moveItem(modIndexAge.row());
    slmodel->insertData(str);

}
