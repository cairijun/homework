#ifndef STUDENTMAIN_H
#define STUDENTMAIN_H

#include "Student.h"
#include "StatisticsDialog.h"
#include "Controller.h"
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

    void on_StudentList_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_SaveData_clicked();

    void on_ResetData_clicked();

private:
    Ui::StudentMain *ui;
    StatisticsDialog *statDialog = 0;

    Student::IController *_controller = 0;
};

#endif // STUDENTMAIN_H
