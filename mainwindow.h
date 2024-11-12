#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "helpform.h"
#include "ui_version.h"
#include "ui_settingsOBS.h"
#include <QSettings>

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
    //QAction* tvSettings;
    QString getFileSportsmens(void);
    Ui::dlgObs* uiObs;

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

    Ui::dlgVersion uiVersion;
    QDialog* dlg;
    QSettings* settings;


private slots:
    void openFile(void);
    void choiceFile(void);
    void Variant(void);
    void selectLogo(bool);
    void slotAcceptSettingsObs(void);
    void slotRejectSettingsObs(void);

signals:
    void newFile(void);
    void variant(int);  //0-без видео, 1-с видео
    void sigLogo(bool);
    void sigExit(void);

};

#endif // MAINWINDOW_H
