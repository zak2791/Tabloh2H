#ifndef HELPFORM_H
#define HELPFORM_H

#include <QWidget>
#include <QTextBrowser>

namespace Ui {
class HelpForm;
}

class HelpForm : public QWidget
{
    Q_OBJECT

public:
    explicit HelpForm(QWidget *parent = nullptr);
    ~HelpForm();

private slots:
    void on_btnClose_clicked();

private:
    Ui::HelpForm *ui;
    QTextBrowser* help;

};

#endif // HELPFORM_H
