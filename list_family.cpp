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
    r_next = "";
    b_next = "";
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

    //фамилия "синего" следующего
    lbl_next_blue = new QLabel("<font color=\"Blue\">blue fam next</font>", this);
    lbl_next_blue->setAlignment(Qt::AlignCenter);
    //фамилия красного следующего
    lbl_next_red = new QLabel("<font color=\"Red\">red fam next</font>", this);
    lbl_next_red->setAlignment(Qt::AlignCenter);

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

    toggleButton = new ToggleButton(10, 8);
    connect(toggleButton, SIGNAL(clicked()), this, SLOT(set_state_toggle()));
    state_toggle = false;

    QFrame* lineLeft = new QFrame(this);
    lineLeft->setFrameShape(QFrame::VLine); // Horizontal line
    lineLeft->setFrameShadow(QFrame::Sunken);
    lineLeft->setLineWidth(1);

    QFrame* lineMiddle = new QFrame(this);
    lineMiddle->setFrameShape(QFrame::VLine); // Horizontal line
    lineMiddle->setFrameShadow(QFrame::Sunken);
    lineMiddle->setLineWidth(1);

    QFrame* lineRight = new QFrame(this);
    lineRight->setFrameShape(QFrame::VLine); // Horizontal line
    lineRight->setFrameShadow(QFrame::Sunken);
    lineRight->setLineWidth(1);

	QHBoxLayout * hbox = new QHBoxLayout();
    QVBoxLayout * red_box = new QVBoxLayout();
    red_box->addWidget(lbl_red);
    QFrame* lineRed = new QFrame(this);
    lineRed->setFrameShape(QFrame::HLine); // Horizontal line
    lineRed->setFrameShadow(QFrame::Sunken);
    lineRed->setLineWidth(1);
    red_box->addWidget(lineRed);
    red_box->addWidget(lbl_next_red);
    hbox->addLayout(red_box, 10);
    //hbox->addWidget(lbl_red, 10);
    hbox->addWidget(lineLeft);
    hbox->addWidget(new QLabel("Выбор для текущего боя ->"));
    hbox->addWidget(toggleButton, 10);
    hbox->addWidget(new QLabel("<- Выбор для следующего боя"));
    hbox->addWidget(lineMiddle);
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
    hbox->addWidget(lineRight);
    QVBoxLayout * blue_box = new QVBoxLayout();
    blue_box->addWidget(lbl_blue);
    QFrame* lineBlue = new QFrame(this);
    lineBlue->setFrameShape(QFrame::HLine); // Horizontal line
    lineBlue->setFrameShadow(QFrame::Sunken);
    lineBlue->setLineWidth(1);
    blue_box->addWidget(lineBlue);
    blue_box->addWidget(lbl_next_blue);
    hbox->addLayout(blue_box, 10);


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

    connect(sel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(sel(QItemSelection, QItemSelection)));
    connect(tbl, SIGNAL(mouse_button(int)), this, SLOT(sell(int)));
    connect(inFam, SIGNAL(textEdited(QString)), this, SLOT(textEdited(QString)));
    //connect(weight, SIGNAL(activated(const QString&)), this, SLOT(set_weight(QString)));
    connect(weight, SIGNAL(activated(QString)), this, SLOT(selectWeight(QString)));
    connect(age, SIGNAL(activated(QString)), this, SLOT(selectAge(QString)));
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
    just_opened_red = true;
    just_opened_blue = true;
    state_toggle = toggleButton->isChecked();
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
    emit sig_hide(r, b, r_next, b_next);
	hide();
    qDebug()<<r<< b<< r_next<< b_next;
}

void ListFamily::sel(QItemSelection a, QItemSelection )
{
	sel_data = a.indexes()[0].data().toString();
}

void ListFamily::sell(int _sel)
{
	if (_sel) {
        if(!state_toggle){  //если выбираем вызываемого спортсмена
            lbl_blue->setText("<font color='Blue'size=3>" + sel_data + "</font>");
            b = sel_data;
        }else{
            if(just_opened_blue){
                just_opened_blue = false;
                lbl_blue->setText("<font color='Blue'size=3>" + lbl_next_blue->text() + "</font>");
                b_next = lbl_next_blue->text();

            }
            lbl_next_blue->setText("<font color='Blue'size=3>" + sel_data + "</font>");
            b_next = sel_data;
            qDebug()<<"b_next = "<<b_next;
        }
        //lbl_blue->setText("<font color='Blue'size=3>" + sel_data + "</font>");
        //b = sel_data;
	}
	else {
        if(!state_toggle){  //если выбираем вызываемого спортсмена
            lbl_red->setText("<font color='Red' size=3>" + sel_data + "</font>");
            r = sel_data;
        }else{
            if(just_opened_red){
                just_opened_red = false;
                lbl_red->setText("<font color='Red' size=3>" + lbl_next_red->text() + "</font>");
                r_next = lbl_next_red->text();

            }
            lbl_next_red->setText("<font color='Red' size=3>" + sel_data + "</font>");
            r_next = sel_data;
            qDebug()<<"r_next = "<<r_next;
        }
        //lbl_red->setText("<font color='Red' size=3>" + sel_data + "</font>");
        //r = sel_data;
	}
    //inFam->setFocus();
}

void ListFamily::set_state_toggle(){
    state_toggle = toggleButton->isChecked();
    just_opened_blue = false;
    just_opened_red = false;
}
