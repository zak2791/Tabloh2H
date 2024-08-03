#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "helpform.h"
#include "ui_version.h"

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

private slots:
    void openFile(void);
    void choiceFile(void);
    void Variant(void);
    void selectLogo(bool);

signals:
    void newFile(void);
    void variant(int);  //0-без видео, 1-с видео
    void sigLogo(bool);

};

#endif // MAINWINDOW_H
