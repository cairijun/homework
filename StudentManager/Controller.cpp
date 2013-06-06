#include "Controller.h"
#include<QString>
#include<QTableWidget>

#define _WIG_DIS(widget) widget->setEnabled(false)
#define _WIG_ROL(widget) widget->setReadOnly(true)
namespace Student
{

AdmissionsOfficeController::AdmissionsOfficeController(Ui::StudentMain *ui)
    :IController(ui), _studentBaseObj(new StudentBase)
{
    _WIG_DIS(_ui->ScoresGroupBox);
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


FacultyController::FacultyController(Ui::StudentMain *ui, FacultyName facultyName)
    :IController(ui)
{
    switch(facultyName)
    {
    case FACULTY_NAME_A:
        _facultyObj = new FacultyA;
        break;
    case FACULTY_NAME_B:
        _facultyObj = new FacultyB;
        break;
    case FACULTY_NAME_C:
        _facultyObj = new FacultyC;
        break;
    }

    _WIG_ROL(_ui->NameBox);
    _WIG_ROL(_ui->AgeBox);
    _WIG_ROL(_ui->AddressBox);

    _WIG_DIS(_ui->MaleRadio);
    _WIG_DIS(_ui->FemaleRadio);
    _WIG_DIS(_ui->SaveData);
    _WIG_DIS(_ui->ResetData);
    _WIG_DIS(_ui->AddStudent);
    _WIG_DIS(_ui->DeleteStudent);
}

void FacultyController::loadStudentList()
{
    loadList(_facultyObj);
}

void FacultyController::aStudentSelected(long ID)
{
    loadBaseInformation(_facultyObj, ID);

    bool isMajor = _facultyObj->isMajor(ID);
    if(isMajor)
    {
        _ui->ScoresTab->setTabEnabled(0, true);
        _ui->ScoresTab->setTabEnabled(1, false);
    }
    else
    {
        _ui->ScoresTab->setTabEnabled(1, true);
        _ui->ScoresTab->setTabEnabled(0, false);
    }

    if(!isMajor && !_facultyObj->isMinor(ID))
    {
        _ui->ScoresTab->setTabEnabled(1, false);//既非主修又非辅修
        _ui->MajorScoreList->clearContents();
        _ui->MinorScoreList->clearContents();
    }
    else
        loadScores(_facultyObj, ID, isMajor);
}

void FacultyController::saveAStudent()
{

}

DegreesOfficeController::DegreesOfficeController(Ui::StudentMain *ui)
    :IController(ui), _misObj(new StudentMIS)
{
    //禁用相关控件
    _WIG_ROL(_ui->NameBox);
    _WIG_ROL(_ui->AgeBox);
    _WIG_ROL(_ui->AddressBox);

    _WIG_DIS(_ui->MaleRadio);
    _WIG_DIS(_ui->FemaleRadio);
    _WIG_DIS(_ui->SaveData);
    _WIG_DIS(_ui->ResetData);
    _WIG_DIS(_ui->AddStudent);
    _WIG_DIS(_ui->DeleteStudent);

    _WIG_DIS(_ui->AddMajorScore);
    _WIG_DIS(_ui->AddMinorScore);
    _WIG_DIS(_ui->DeleteMajorScore);
    _WIG_DIS(_ui->DeleteMinorScore);
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
