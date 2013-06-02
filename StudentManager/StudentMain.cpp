#include "StudentMain.h"
#include "ui_StudentMain.h"

StudentMain::StudentMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StudentMain),
    statDialog(0)
{
    ui->setupUi(this);
}

StudentMain::~StudentMain()
{
    delete ui;

    if(statDialog)
        delete statDialog;
}

StudentMain::StudentMain(Student::Role role, QWidget *parent)
    :StudentMain(parent)
{
    switch(role)
    {
    case Student::ADMISSIONS_OFFICE:
        break;
    case Student::DEGREES_OFFICE:
        break;
    case Student::FACULTY_A:
        break;
    case Student::FACULTY_B:
        break;
    case Student::FACULTY_C:
        break;
    }
}

void StudentMain::on_DegreeStat_clicked()
{
    statDialog = new StatisticsDialog;
    statDialog->show();
    statDialog->exec();
    delete statDialog;
    statDialog = 0;
}
