#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "helpform.h"
#include "ui_mainwindow.h"
#include "ui_version.h"
//#include "ui_settingsOBS.h"
#include <QSettings>
#include "ui_code.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QLabel* lblStatus;
    QAction* winSettings;
    QAction* closeProg;
    QAction* time;
    QAction* actSbros;
    QAction* video;
    QAction* no_video;
    QAction* winVideoSettings;
    //QAction* tvSettings;
    QString getFileSportsmens(void);
    //Ui::dlgObs uiObs;
    bool getStatusRegistration(void){return registration->isEnabled();};

private:
    Ui::MainWindow *ui;
    virtual void closeEvent(QCloseEvent*);
    QString lastDir;            //последняя директория, из которой выбирали файл со спортсменами
    QList<QString> listFiles;   //список последних файлов
    QString fileSportsmens;     //текущий файл со списком спортсменов
    QMenu* lastFiles;
    QString fileSettings;
    HelpForm* hf;
    QAction* winHelp;
    QAction* registration;

    Ui::dlgVersion uiVersion;
    QDialog* dlg;
    QSettings* settings;
    QString serialNumberMac;
    QString serialNumberBios;
    int func(int);
    QLineEdit* leId;
    QDialog* frmCode;
    Ui::dlgCode ui_code;
    QString readCode = "";
    QString serialNumber = "";

private slots:
    void openFile(void);
    void choiceFile(void);
    void Variant(void);
    void selectLogo(bool);
    // void slotAcceptSettingsObs(void);
    // void slotRejectSettingsObs(void);
    void slotRegistration(void);
    QString calculateCode(QString);
    void slotChangeId(bool);

signals:
    void newFile(void);
    void variant(int);  //0-без видео, 1-с видео
    void sigLogo(bool);
    void sigExit(void);
    //void sigRegistration(void);

};

#endif // MAINWINDOW_H
