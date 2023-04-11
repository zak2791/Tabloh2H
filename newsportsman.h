#ifndef NEWSPORTSMAN_H
#define NEWSPORTSMAN_H

#include <QObject>
#include <QWidget>

#include "ui_new_sportsman.h"

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

using namespace QXlsx;

class NewSportsman : public QWidget, public Ui::frmSportsman
{
    Q_OBJECT

    Document* doc;
    Worksheet* wsheet;
    int maxRow;
    int maxCol;

private slots:
    void addSportsman(void);
    virtual void showEvent(QShowEvent *event);

public:
    explicit NewSportsman(QWidget *parent = nullptr);


signals:

};

#endif // NEWSPORTSMAN_H
