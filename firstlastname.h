#ifndef FIRSTLASTNAME_H
#define FIRSTLASTNAME_H

#include "fam_reg.h"


class FirstLastName : public Fam
{

public:
    FirstLastName(QString _cl = "yellow",
                  QString _tx = "text",
                  int _wt = 10,
                  QString fn = "",
                  QWidget *parent = 0);

private:
    virtual void paintEvent(QPaintEvent *);

};

#endif // FIRSTLASTNAME_H
