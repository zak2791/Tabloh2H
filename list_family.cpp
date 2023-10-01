#include "list_family.h"
//#include <QMessageBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include <QDir>
#include <QMessageBox>
#include "pcscreen.h"


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
    currentAge = "";
    currentWeight = "";
    nextAge = "";
    nextWeight = "";

    fAdd = new addForm;

	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
	//setWindowState(Qt::WindowFullScreen);
	sel_data = "";

	//выбор весовой категории
    weight = new QComboBox(this);

	//фамилия "синего"
	lbl_blue = new QLabel("<font color=\"Blue\">blue fam</font>", this);
	lbl_blue->setAlignment(Qt::AlignCenter);
    lbl_blue->setStyleSheet("color: blue;");
	//фамилия красного
	lbl_red = new QLabel("<font color=\"Red\">red fam</font>", this);
	lbl_red->setAlignment(Qt::AlignCenter);
    lbl_red->setStyleSheet("color: red;");

    //фамилия "синего" следующего
    lbl_next_blue = new QLabel("<font color=\"Blue\">blue fam next</font>", this);
    lbl_next_blue->setAlignment(Qt::AlignCenter);
    lbl_next_blue->setStyleSheet("color: blue;");
    //фамилия красного следующего
    lbl_next_red = new QLabel("<font color=\"Red\">red fam next</font>", this);
    lbl_next_red->setAlignment(Qt::AlignCenter);
    lbl_next_red->setStyleSheet("color: red;");

    //кнопка добавления спортсмена
    QPushButton * btnAdd = new QPushButton(u8"Добавить спортсмена");
    connect(btnAdd, SIGNAL(clicked()), fAdd, SLOT(show()));

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



    QDir dir = QDir::current();

    //dir.cdUp();
    QString path = static_cast<PCScreen*>(parent)->mainwin->getFileSportsmens();
    //QString path(dir.path() + "/sportsmens.xlsx");

    //QString path1(dir.path() + "/bin");
    //dir.setCurrent(path1);  //закомментить для работы из Qt Creator

    //QMessageBox::warning(this, "Внимание!", path, QMessageBox::Ok);

    doc = new Document(path);
    if (!doc->load()){
        QMessageBox::warning(this, "Внимание!", "Не найден файл со списком спортсменов", QMessageBox::Ok);
        //return;
    }

    doc->workbook()->setActiveSheet(0);

    //QList<QString>* qL;
    QVector<CellLocation> clList;
    int maxRow = -1;
    int maxCol = -1;
    Worksheet* wsheet = (Worksheet*)doc->workbook()->activeSheet();


    clList = wsheet->getFullCells( &maxRow, &maxCol );

    QList<QString> lAge;

    QList<QString> lWeight;
    //lAge = QList<QString>();

    //QList<QString> sportsmens;

    for(int i = 1; i <= maxRow; i++){
        if(doc->read(i, 1).toString() == "") break;
        sportsmens.append(doc->read(i, 1).toString() + ";"
                          + doc->read(i, 2).toString() + ";"
                          + doc->read(i, 3).toString() + ";"
                          + doc->read(i, 4).toString() + ";"
                          + doc->read(i, 5).toString());
        l.append(doc->read(i, 1).toString() + ";" + doc->read(i, 2).toString() + ";" + doc->read(i, 5).toString());
        lAge.append(doc->read(i, 3).toString());
        lWeight.append(doc->read(i, 4).toString());
    }

    //qDebug()<<sportsmens;

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
    cbNum = new QCheckBox(this);

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
    QHBoxLayout * hboxBottom = new QHBoxLayout();
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
    hboxBottom->addWidget(new QLabel("Выбор для текущего боя ->"), 1);
    hboxBottom->addWidget(toggleButton, 1);
    hboxBottom->addWidget(new QLabel("<- Выбор для следующего боя"));
    hboxBottom->addWidget(lineMiddle);

    hboxBottom->addWidget(new QLabel(u8"Выбор по номерам"), 1);
    hboxBottom->addWidget(cbNum, 1);

    QFrame* line1 = new QFrame(this);
    line1->setFrameShape(QFrame::VLine); // Horizontal line
    line1->setFrameShadow(QFrame::Sunken);
    line1->setLineWidth(1);
    hboxBottom->addWidget(line1);

    hboxBottom->addWidget(new QLabel(u8"Сортировать по возрасту и весу"), 1);
    hboxBottom->addWidget(cBox, 1);

    QFrame* line2 = new QFrame(this);
    line2->setFrameShape(QFrame::VLine); // Horizontal line
    line2->setFrameShadow(QFrame::Sunken);
    line2->setLineWidth(1);
    hboxBottom->addWidget(line2);

    hboxBottom->addWidget(new QLabel(u8"Вес:"), 1);
    hboxBottom->addWidget(weight, 2);

    QFrame* line0 = new QFrame(this);
    line0->setFrameShape(QFrame::VLine); // Horizontal line
    line0->setFrameShadow(QFrame::Sunken);
    line0->setLineWidth(1);
    hboxBottom->addWidget(line0);

    hboxBottom->addWidget(new QLabel(u8"Возраст:"), 1);
    hboxBottom->addWidget(age, 2);
    //hboxBottom->addSpacing(10);

    QFrame* line3 = new QFrame(this);
    line3->setFrameShape(QFrame::VLine); // Horizontal line
    line3->setFrameShadow(QFrame::Sunken);
    line3->setLineWidth(1);
    hboxBottom->addWidget(line3);

    hboxBottom->addSpacing(10);
    hboxBottom->addWidget(btnAdd, 2);

    hboxBottom->addSpacing(20);
    hboxBottom->addWidget(btnHide, 2);
    hboxBottom->addStretch(3);
    //hboxBottom->addSpacing(50);

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

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine); // Horizontal line
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);
    mainbox->addWidget(line);

    mainbox->addLayout(hboxBottom);
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
    connect(cbNum, SIGNAL(stateChanged(int)), this, SLOT(sortByNum(int)));

    connect(fAdd, SIGNAL(selSportsman(QString, int)), this, SLOT(addedSportsmens(QString, int)));

    //showFullScreen();
}
 
ListFamily::~ListFamily()
{
}

void ListFamily::initListSportsmens(){

}

void ListFamily::allowSorting(int i){
    if(i){
        selectWeight(weight->currentText());
    }else{
        l.clear();
        for(int i = 0; i < sportsmens.count(); i++){
            //if(cbNum->isChecked() == false){
                l.append(sportsmens.at(i).split(";")[0] + QString(";") + sportsmens.at(i).split(";")[1] + QString(";")
                        + sportsmens.at(i).split(";")[4]);
            //}else{
            //    l.append(doc->read(i, 1).toString() + ";" + doc->read(i, 2).toString() + " (" + doc->read(i, 5).toString() + ")");
                //1qDebug()<<"l = "<<l;
            //}
        }
        l.prepend("-");
        textEdited(inFam->text());
    }
}

void ListFamily::sortByNum(int i){
    if(i == 0){
        age->setEnabled(true);
        weight->setEnabled(true);
        cBox->setEnabled(true);
        //if(cBox->isChecked())
        //    allowSorting(1);
        //connect(cBox, SIGNAL(stateChanged(int)), this, SLOT(allowSorting(int)));
    }else{
        age->setCurrentText("");
        age->setEnabled(false);
        weight->setCurrentText("");
        weight->setEnabled(false);
        //disconnect(cBox);
        cBox->setChecked(false);
        cBox->setEnabled(false);
        //allowSorting(0);
        //qDebug()<<cBox->isCheckable();
        //if(cBox->isChecked())
        //    cBox->toggle();     //setChecked(false);

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
        if(cbNum->isChecked() == false){
            if(str.startsWith(s , Qt::CaseInsensitive))
                inL << str;
        }else{
            QString ss = "";
            if(str != "-"){
                //qDebug()<<"str = "<<str;
                ss = str.split(";").at(2);
                if(ss == s)
                    inL << str;
            }
        }
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
    connect(tbl->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(sel(QItemSelection, QItemSelection)));
}

void ListFamily::_hide()
{
    emit sig_hide(r, b, r_next, b_next);
	hide();
    //deleteLater();
    //qDebug()<<r<< b<< r_next<< b_next;
}

void ListFamily::sel(QItemSelection a, QItemSelection )
{
    sel_data = a.indexes()[0].data().toString();
    //qDebug()<<sel_data;
}

void ListFamily::sell(int _sel){
    qDebug()<<"sel_data = "<<sel_data;
    QString num;
    if(cbNum->isChecked() && sel_data != "-"){
        if(sel_data.split(";").length() > 2)
            num = sel_data.split(";").at(2);
        else
            return;


        //num = ss.remove(")");

        foreach(QString s, sportsmens){
            if(s.split(";").at(4) == num){
                if(!state_toggle){
                    currentAge = s.split(";").at(2);
                    currentWeight = s.split(";").at(3);
                    age->setCurrentText(currentAge);
                    weight->setCurrentText(currentWeight);
                }else{
                    if (_sel){
                        if(just_opened_blue){
                            currentAge = nextAge;
                            currentWeight = nextWeight;
                            age->setCurrentText(currentAge);
                            weight->setCurrentText(currentWeight);
                        }
                        nextAge = s.split(";").at(2);
                        nextWeight = s.split(";").at(3);
                    }
                }
            }
        }

    }
    //qDebug()<<"sel_data = "<<sel_data;

    //qDebug()<<"weight = "<<doc->
	if (_sel) {
        if(!state_toggle){  //если выбираем вызываемого спортсмена
            lbl_blue->setText(sel_data);
            b = sel_data;
            //currentAge =
            //currentWeight =

        }else{
            if(just_opened_blue){
                just_opened_blue = false;
                lbl_blue->setText(lbl_next_blue->text());
                b = lbl_next_blue->text();


            }
            lbl_next_blue->setText(sel_data);
            b_next = sel_data;
        }
	}
	else {
        if(!state_toggle){  //если выбираем вызываемого спортсмена
            lbl_red->setText(sel_data);
            r = sel_data;
        }else{
            if(just_opened_red){
                just_opened_red = false;
                lbl_red->setText(lbl_next_red->text());
                r = lbl_next_red->text();

            }
            lbl_next_red->setText(sel_data);
            r_next = sel_data;
        }
	}
    inFam->setFocus();
}

void ListFamily::set_state_toggle(){
    state_toggle = toggleButton->isChecked();
    just_opened_blue = false;
    just_opened_red = false;
}

void ListFamily::addedSportsmens(QString s, int i){
    if (i == 1) {
        if(!state_toggle){  //если выбираем вызываемого спортсмена
            lbl_blue->setText(s);
            b = s;
        }else{
            if(just_opened_blue){
                just_opened_blue = false;
                lbl_blue->setText(lbl_next_blue->text());
                b = lbl_next_blue->text();
            }
            lbl_next_blue->setText(s);
            b_next = s;
        }
    }
    else {
        if(!state_toggle){  //если выбираем вызываемого спортсмена
            lbl_red->setText(s);
            r = s;
        }else{
            if(just_opened_red){
                just_opened_red = false;
                lbl_red->setText(lbl_next_red->text());
                r = lbl_next_red->text();

            }
            lbl_next_red->setText(s);
            r_next = s;
        }
    }
}
