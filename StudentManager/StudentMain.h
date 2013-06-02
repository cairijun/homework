#ifndef STUDENTMAIN_H
#define STUDENTMAIN_H

#include "Student.h"
#include "StatisticsDialog.h"
#include <QWidget>

namespace Ui {
class StudentMain;
}

class StudentMain : public QWidget
{
    Q_OBJECT
    
public:
    explicit StudentMain(QWidget *parent = 0);
    StudentMain(Student::Role role, QWidget *parent = 0);
    ~StudentMain();
    
private slots:
    void on_DegreeStat_clicked();

private:
    Ui::StudentMain *ui;
    StatisticsDialog *statDialog;
};

#endif // STUDENTMAIN_H
