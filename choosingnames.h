#ifndef CHOOSINGNAMES_H
#define CHOOSINGNAMES_H

#include <QWidget>
#include <QQuickWidget>
#include <QSortFilterProxyModel>

#include "mainlistmodel.h"
#include "sidelistmodel.h"

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
    QObject* objAge;
    QObject* objWeight;
    QObject* objDelegate;

private slots:
    void choiceAge(QString);
    void choiceWeight(QString);
    void removeItem(int);

};

#endif // CHOOSINGNAMES_H
