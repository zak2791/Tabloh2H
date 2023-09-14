#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

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
    QString getFileSportsmens(void);

private:
    Ui::MainWindow *ui;
    virtual void closeEvent(QCloseEvent*);
    QString lastDir;            //последняя директория, из которой выбирали файл со спортсменами
    QList<QString> listFiles;   //список последних файлов
    QString fileSportsmens;     //текущий файл со списком спортсменов
    QMenu* lastFiles;

private slots:
    void openFile(void);
    void choiceFile(void);

signals:
    void newFile(void);

};

#endif // MAINWINDOW_H
