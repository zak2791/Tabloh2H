#pragma once

#include <QWidget>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

using namespace QXlsx;

class ListFamily : public QWidget {

public:
    explicit ListFamily(QWidget * parent = 0);

	~ListFamily();

    QStringList getSportsmens(void);
    QStringList lAge;
    QStringList lWeight;

    Document* doc;
	
	QList<QString> l;

    QList<QString> sportsmens;

};
