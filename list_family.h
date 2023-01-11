#pragma once
#include <QTableView>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTextStream>
#include <QStandardItemModel>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QItemSelection>
#include <QCheckBox>
#include "togglebutton.h"

class FamilyView : public QTableView {
	Q_OBJECT

public:
	explicit FamilyView(QWidget * parent = 0);

	~FamilyView();

signals:
	void mouse_button(int);

private:
	virtual void mouseReleaseEvent(QMouseEvent *);
};

class ListFamily : public QWidget {
	Q_OBJECT

public:
	explicit ListFamily(QWidget * parent = 0);

	~ListFamily();

    QComboBox * weight;

signals:
    void sig_hide(QString, QString, QString, QString);
	//void family();
    //void sig_category(QString);

private slots:
    void selectWeight(QString);
    void selectAge(QString);
    void allowSorting(int);

public slots:
	virtual void textEdited(QString);
	void sel(QItemSelection, QItemSelection);
	void sell(int);
	void _hide();
    void addWeight(QList<QString>);
    void set_state_toggle(void);

private:
	virtual void showEvent(QShowEvent *);
	//virtual void hideEvent(QEvent *);
	virtual void resizeEvent(QResizeEvent *);
	
	QList<QString> l;
	QLineEdit * inFam;
	QLabel * lbl_blue;
	QLabel * lbl_red;
    QLabel * lbl_next_blue;
    QLabel * lbl_next_red;
    QString b, r, r_next, b_next, sel_data;

	int col;				//количество колонок в таблице
	FamilyView * tbl;

    QComboBox * age;
    QCheckBox * cBox;
    QList<QString> sportsmens;
    bool just_opened_red;   //флаг открытия окна при вводе фамилии готовящегося спортсмена с красным поясом
    bool just_opened_blue;  //флаг открытия окна при вводе фамилии готовящегося спортсмена с синим поясом
    bool state_toggle;      //состояние переключателя: false - выбор вызываемого спортсмена;
                            //                         true - выбор готовящегося спортсмена.
    ToggleButton* toggleButton;
};
