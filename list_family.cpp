#include "list_family.h"
//#include <QMessageBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
using namespace QXlsx;

FamilyView::FamilyView(QWidget * parent) : QTableView(parent) {

}

FamilyView::~FamilyView()
{
}

void FamilyView::mouseReleaseEvent(QMouseEvent * e)
{
	if (e->button() == Qt::LeftButton)
		emit mouse_button(0);
	else if (e->button() == Qt::RightButton)
		emit mouse_button(1);
}

ListFamily::ListFamily(QWidget * parent) : QWidget(parent) {
	b = "";
	r = "";
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
	//setWindowState(Qt::WindowFullScreen);
	sel_data = "";

	//выбор весовой категории
    weight = new QComboBox(this);

	//фамилия "синего"
	lbl_blue = new QLabel("<font color=\"Blue\">blue fam</font>", this);
	lbl_blue->setAlignment(Qt::AlignCenter);
	//фамилия красного
	lbl_red = new QLabel("<font color=\"Red\">red fam</font>", this);
	lbl_red->setAlignment(Qt::AlignCenter);
	//кнопка скрытия
	QPushButton * btnHide = new QPushButton(u8"СКРЫТЬ");
	connect(btnHide, SIGNAL(clicked()), this, SLOT(_hide()));
	//ввод фамилии
	inFam = new QLineEdit(this);
	inFam->setMaxLength(7);
	l = QList<QString> ({ "-" });
	QFont f;
	f.setPointSize(10);
	inFam->setFont(f);
	weight->setFont(f);

    Document doc("sportsmens.xlsx");
    if (doc.load()){

    }

    doc.workbook()->setActiveSheet(0);

    //QList<QString>* qL;
    QVector<CellLocation> clList;
    int maxRow = -1;
    int maxCol = -1;
    Worksheet* wsheet = (Worksheet*)doc.workbook()->activeSheet();
    clList = wsheet->getFullCells( &maxRow, &maxCol );

    QList<QString> lAge;

    QList<QString> lWeight;
    //lAge = QList<QString>();

    //QList<QString> sportsmens;

    for(int i = 1; i <= maxRow; i++){
        sportsmens.append(doc.read(i, 1).toString() + ";"
                          + doc.read(i, 2).toString() + ";"
                          + doc.read(i, 3).toString() + ";"
                          + doc.read(i, 4).toString());
        l.append(doc.read(i, 1).toString() + ";" + doc.read(i, 2).toString());
        lAge.append(doc.read(i, 3).toString());
        lWeight.append(doc.read(i, 4).toString());
    }
    lAge.removeDuplicates();
    lWeight.removeDuplicates();

    age = new QComboBox(this);
    lAge.sort();
    age->addItem(QString(""));
    age->addItems(lAge);
    lWeight.sort();
    weight->addItem(QString(""));
    weight->addItems(lWeight);

    cBox = new QCheckBox(this);

	l.sort();

	int num_fam = l.length();   //количество фамилий в списке
	col = 6;				//количество колонок в таблице
	int	raw = num_fam / col;
	if (num_fam % col)
		raw += 1;				//количество строк в таблице

	QStandardItemModel * mdl = new QStandardItemModel(raw, col);

	int i = 0;
	int _c, _r;
	QStandardItem * item;
	for (_c = 0; _c < col; _c++) {
		for (_r = 0; _r < raw; _r++) {
			i++;
			if (i > num_fam) break;
			item = new QStandardItem(l[i - 1]);
			//QMessageBox::question(0, l[i - 1], "item->data().toString()");
			mdl->setItem(_r, _c, item);
		}
	}

	QHBoxLayout * hbox = new QHBoxLayout();
	hbox->addWidget(lbl_red, 10);
    hbox->addWidget(new QLabel(u8"Сортировать по возрасту и весу"));
    hbox->addWidget(cBox, 1);
    hbox->addWidget(new QLabel(u8"Вес:"));
    hbox->addWidget(weight, 1);
    hbox->addWidget(new QLabel(u8"Возраст:"));
    hbox->addWidget(age, 1);
	hbox->addSpacing(10);
	hbox->addWidget(btnHide);
	hbox->addSpacing(10);
	hbox->addWidget(new QLabel(u8"Ввод"));
	hbox->addWidget(inFam, 2);
	hbox->addWidget(lbl_blue, 10);


	tbl = new FamilyView();
	QVBoxLayout * mainbox = new QVBoxLayout();
	tbl->setModel(mdl);
	QItemSelectionModel * sel = tbl->selectionModel();
	tbl->setGridStyle(static_cast<Qt::PenStyle>(2));
    //int w = tbl->width();		//self.col
	tbl->resizeRowsToContents();
	//QHeaderView * hv = 
	tbl->horizontalHeader()->hide();
	//hv->hide();
	tbl->verticalHeader()->hide();
	tbl->setShowGrid(true);
	tbl->setSelectionMode(QAbstractItemView::SingleSelection);

	mainbox->addLayout(hbox);
    //mainbox->addWidget(box);
	mainbox->addWidget(tbl);

	setLayout(mainbox);

	connect(sel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(sel(QItemSelection, QItemSelection)));
    connect(tbl, SIGNAL(mouse_button(int)), this, SLOT(sell(int)));
	connect(inFam, SIGNAL(textEdited(const QString&)), this, SLOT(textEdited(QString)));
    //connect(weight, SIGNAL(activated(const QString&)), this, SLOT(set_weight(QString)));
    connect(weight, SIGNAL(activated(const QString&)), this, SLOT(selectWeight(QString)));
    connect(age, SIGNAL(activated(const QString&)), this, SLOT(selectAge(QString)));
    connect(cBox, SIGNAL(stateChanged(int)), this, SLOT(allowSorting(int)));

}
 
ListFamily::~ListFamily()
{
}

void ListFamily::allowSorting(int i){
    if(i){
        selectWeight(weight->currentText());
    }else{
        l.clear();
        for(int i = 0; i < sportsmens.count(); i++){
            l.append(sportsmens.at(i).split(";")[0] + QString(";") + sportsmens.at(i).split(";")[1]);
        }
        l.prepend("-");
        textEdited(inFam->text());
    }
}

void ListFamily::selectWeight(QString s){
    if(cBox->isChecked()){
        l.clear();
        for(int i = 0; i < sportsmens.count(); i++){
            if(sportsmens.at(i).split(";")[3] == s || s == "")
                if(sportsmens.at(i).split(";")[2] == age->currentText() || age->currentText() == "")
                    l.append(sportsmens.at(i).split(";")[0] + QString(";") + sportsmens.at(i).split(";")[1]);
        }
        l.prepend("-");
        textEdited(inFam->text());
    }
}

void ListFamily::selectAge(QString s){
    if(cBox->isChecked()){
        l.clear();
        for(int i = 0; i < sportsmens.count(); i++){
            if(sportsmens.at(i).split(";")[2] == s || s == "")
                if(sportsmens.at(i).split(";")[3] == weight->currentText() || weight->currentText() == "")
                    l.append(sportsmens.at(i).split(";")[0] + QString(";") + sportsmens.at(i).split(";")[1]);
        }
        l.prepend("-");
        textEdited(inFam->text());
    }
}

void ListFamily::addWeight(QList<QString> s){
    for(int i = weight->count() - 1; i >= 0; i--)
        weight->removeItem(i);
    weight->addItems(s);

}

void ListFamily::showEvent(QShowEvent  * )
{
    inFam->setFocus();
}

void ListFamily::resizeEvent(QResizeEvent * )
{
	for (int i = 0; i < col; i++)
		tbl->setColumnWidth(i, (tbl->rect().width() - 20) / col);
}

void ListFamily::textEdited(QString s){

	QStringList inL = QStringList();

	foreach (QString str, l){
		if(str.startsWith(s , Qt::CaseInsensitive))
			inL << str;
	}
	QStringList lst;
	if (s.compare("") == 0)
		lst = l;
	else
		lst = inL;

	int num_fam = lst.count();   //количество фамилий в списке
	int raw = num_fam / col;
	if (num_fam % col)
		raw++;        //количество строк в таблице

	QStandardItemModel * mdl = new QStandardItemModel(raw, col);
	QStandardItem * item;
	int i = 0;

	int _c, _r;
	for (_c = 0; _c < col; _c++) {
		for (_r = 0; _r < raw; _r++) {
			i++;
			if (i > num_fam) break;
			item = new QStandardItem(lst[i - 1]);
			mdl->setItem(_r, _c, item);
		}
	}

	tbl->setModel(mdl);
	connect(tbl->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(sel(QItemSelection, QItemSelection)));
}

void ListFamily::_hide()
{
	emit sig_hide(r, b);
	hide();
}

void ListFamily::sel(QItemSelection a, QItemSelection )
{
	sel_data = a.indexes()[0].data().toString();
}

void ListFamily::sell(int _sel)
{
	if (_sel) {
        lbl_blue->setText("<font color='Blue'size=3>" + sel_data + "</font>");
		b = sel_data;
	}
	else {
        lbl_red->setText("<font color='Red' size=3>" + sel_data + "</font>");
		r = sel_data;
	}
    //inFam->setFocus();
}
