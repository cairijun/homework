#include "StudentMain.h"
#include "ui_StudentMain.h"

StudentMain::StudentMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StudentMain),
    statDialog(0),
    pBase(0), pFA(0), pFB(0), pFC(0), pMIS(0)
{
    ui->setupUi(this);
}

StudentMain::~StudentMain()
{
    delete ui;

    _CHK_DEL(statDialog);
    _CHK_DEL(pBase);
    _CHK_DEL(pMIS);
    _CHK_DEL(pFA);
    _CHK_DEL(pFB);
    _CHK_DEL(pFC);
}

StudentMain::StudentMain(Student::Role role, QWidget *parent)
    :StudentMain(parent)
{
    switch(role)
    {
    case Student::ADMISSIONS_OFFICE:
        pBase = new Student::StudentBase;
        break;
    case Student::DEGREES_OFFICE:
        pMIS = new Student::StudentMIS;
        break;
    case Student::FACULTY_A:
        pFA = new Student::FacultyA;
        break;
    case Student::FACULTY_B:
        pFB = new Student::FacultyB;
        break;
    case Student::FACULTY_C:
        pFC = new Student::FacultyC;
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
