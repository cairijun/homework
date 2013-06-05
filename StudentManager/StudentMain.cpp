#include "ui_StudentMain.h"
#include "StudentMain.h"

#include<cassert>

StudentMain::StudentMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StudentMain)
{
    ui->setupUi(this);
}

StudentMain::~StudentMain()
{
    delete ui;

    _CHK_DEL(_controller);
}

StudentMain::StudentMain(Student::Role role, QWidget *parent)
    :StudentMain(parent)
{
    switch(role)
    {
    case Student::ADMISSIONS_OFFICE:
        _controller = new Student::AdmissionsOfficeController(ui);
        break;
    case Student::DEGREES_OFFICE:
        _controller = new Student::DegreesOfficeController(ui);
        break;
    case Student::FACULTY_A:
        _controller = new Student::FacultyController(ui, Student::FacultyController::FACULTY_NAME_A);
        break;
    case Student::FACULTY_B:
        _controller = new Student::FacultyController(ui, Student::FacultyController::FACULTY_NAME_B);
        break;
    case Student::FACULTY_C:
        _controller = new Student::FacultyController(ui, Student::FacultyController::FACULTY_NAME_C);
        break;
    }

    assert(_controller);

    _controller->loadStudentList();
    ui->StudentList->selectRow(0);
}

void StudentMain::on_DegreeStat_clicked()
{
    statDialog = new StatisticsDialog;
    statDialog->show();
    statDialog->exec();
    delete statDialog;
    statDialog = 0;
}

void StudentMain::on_StudentList_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if(currentRow != previousRow)
    {
        QTableWidgetItem *item = ui->StudentList->item(currentRow, 0);
        if(item)
        {
            long ID = item->text().toLong();
            _controller->aStudentSelected(ID);
        }
    }
}

void StudentMain::on_SaveData_clicked()
{
    _controller->saveAStudent();
}

void StudentMain::on_ResetData_clicked()
{
    QTableWidgetItem *item = ui->StudentList->item(ui->StudentList->currentRow(), 0);
    _controller->aStudentSelected(item->text().toLong());
}
