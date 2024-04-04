#ifndef CHOOSINGNAMES_H
#define CHOOSINGNAMES_H

#include <QWidget>
#include <QQuickWidget>
#include <QSortFilterProxyModel>

#include "mainlistmodel.h"
#include "sidelistmodel.h"

#include "addform.h"

class ChoosingNames : public QWidget
{
    Q_OBJECT
public:
    explicit ChoosingNames(QWidget *parent = nullptr);

    void setNames(QStringList);
    void setAge(QStringList);
    void setWeight(QStringList);

private:
    QQuickWidget* quickWidget;
    virtual void resizeEvent(QResizeEvent*);
    MainListModel* model;
    SideListModel* slmodel;
    QSortFilterProxyModel* proxyAge;
    QSortFilterProxyModel* proxyWeight;
    QSortFilterProxyModel* proxyName;
    QObject* objAge;
    QObject* objWeight;
    QObject* objFilterNames;
    QObject* objDelegate;
    QObject* objGridSide;
    virtual void closeEvent(QCloseEvent*);
    addForm* fAdd;

private slots:
    void choiceAge(QString);
    void choiceWeight(QString);
    void filterName(QString);
    void fromAllToSide(int);
    void fromSideToAll(int);
    void addSportsman(QString);
    void removeSportsman(int);
    //void fromSideToAllManyItems(int);

signals:
    void close(QString, QString, QString, QString, QString, QString);

};

#endif // CHOOSINGNAMES_H
