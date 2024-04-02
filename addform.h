#ifndef ADDFORM_H
#define ADDFORM_H

#include <QWidget>

#include <QLineEdit>
#include <QListView >
#include <QStandardItemModel>
#include <QSettings>
#include <QMouseEvent>

namespace Ui {
class addForm;
}

class addForm : public QWidget
{
    Q_OBJECT

public:
    explicit addForm(QWidget *parent = nullptr);
    ~addForm();

private:
    Ui::addForm *ui;

    QLineEdit* leName;
    QLineEdit* leTeam;

    QListView * listView;
    //QList<QListViewItem *> listItem;
    QStandardItemModel* model;
    QSettings* settings;
    QString sportsmens;
    QString selectedSportsmen;

private slots:
    void addSportsman(void);
    void mouse();
    void sel(QItemSelection s, QItemSelection);
    void clearList(void);

signals:
    void selSportsman(QString);

};

#endif // ADDFORM_H
