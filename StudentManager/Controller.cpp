#include "Controller.h"
#include<QString>

namespace Student
{

AdmissionsOfficeController::AdmissionsOfficeController(Ui::StudentMain *ui)
    :IController(ui), _studentBaseObj(new StudentBase)
{
    _ui->ScoresGroupBox->setEnabled(false);
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
}

void FacultyController::saveAStudent()
{

}


void DegreesOfficeController::loadStudentList()
{
    loadList(_misObj);
}

void DegreesOfficeController::aStudentSelected(long ID)
{
    loadBaseInformation(_misObj, ID);
}

void DegreesOfficeController::saveAStudent()
{

}
}
