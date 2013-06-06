#include "Controller.h"
#include<QString>
#include<QTableWidget>

namespace Student
{

AdmissionsOfficeController::AdmissionsOfficeController(Ui::StudentMain *ui)
    :IController(ui), _studentBaseObj(new StudentBase)
{
    _ui->ScoresGroupBox->setDisabled(true);
}

void AdmissionsOfficeController::loadStudentList()
{
    loadList(_studentBaseObj);
}

void AdmissionsOfficeController::aStudentSelected(long ID)
{
    loadBaseInformation(_studentBaseObj, ID);
}

void AdmissionsOfficeController::saveAStudent()
{

}


void FacultyController::loadStudentList()
{
    loadList(_facultyObj);
}

void FacultyController::aStudentSelected(long ID)
{
    loadBaseInformation(_facultyObj, ID);

    bool isMinor = _facultyObj->isMinor(ID);//使用isMinor是为了保证major和minor同时为false时选择major
    //bool isMajor = _facultyObj->isMajor(ID);
    if(isMinor)
    {
        _ui->ScoresTab->setTabEnabled(1, true);
        _ui->ScoresTab->setTabEnabled(0, false);
    }
    else
    {
        _ui->ScoresTab->setTabEnabled(0, true);
        _ui->ScoresTab->setTabEnabled(1, false);
    }

    loadScores(_facultyObj, ID, !isMinor);
}

void FacultyController::saveAStudent()
{

}

DegreesOfficeController::DegreesOfficeController(Ui::StudentMain *ui)
    :IController(ui), _misObj(new StudentMIS)
{
    _ui->MajorScoreList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _ui->MinorScoreList->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void DegreesOfficeController::loadStudentList()
{
    loadList(_misObj);
}

void DegreesOfficeController::aStudentSelected(long ID)
{
    loadBaseInformation(_misObj, ID);
    loadScores(_misObj, ID, true);
    loadScores(_misObj, ID, false);
}

void DegreesOfficeController::saveAStudent()
{

}
}
