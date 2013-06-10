#include "Controller.h"
#include<QString>
#include<QTableWidget>
#include<QMessageBox>

#define _WIG_DIS(widget) widget->setEnabled(false)
#define _WIG_ROL(widget) widget->setReadOnly(true)
namespace Student
{

AdmissionsOfficeController::AdmissionsOfficeController(Ui::StudentMain *ui)
    :IController(ui), _studentBaseObj(new StudentBase)
{
    _WIG_DIS(_ui->ScoresGroupBox);
    _WIG_DIS(_ui->DegreeStat);
}

void AdmissionsOfficeController::loadStudentList() const
{
    _loadList(_studentBaseObj);
}

void AdmissionsOfficeController::aStudentSelected(long ID) const
{
    _loadBaseInformation(_studentBaseObj, ID);
}

bool AdmissionsOfficeController::saveAStudent()
{
    long id = _ui->IDBox->text().toLong();
    bool isAdding = _args.contains("adding") && _args["adding"].toInt();
    bool isExists = _studentBaseObj->checkIDExists(id);

    if(isAdding && isExists)//添加时发现学号已存在
        return false;

    Student newStudentObj(
                _ui->NameBox->text(),
                id,
                _ui->FemaleRadio->isCheckable(),
                _ui->AgeBox->value(),
                _ui->AddressBox->text());

    if(!isAdding)//修改现有
    {
        _studentBaseObj->saveAStudentDetails(newStudentObj);
        _ui->StudentList->item(_ui->StudentList->currentRow(), 1)
                ->setText(_ui->NameBox->text());//更新列表中的姓名
    }
    else//添加新学生
    {
        _studentBaseObj->addAStudent(
                    newStudentObj, _stringToFacultyName(_args["major"]), _stringToFacultyName(_args["minor"]));
        loadStudentList();//重新加载列表
    }
    return true;
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
    case FACULTY_NAME_NONE:
        break;
    }

    _WIG_ROL(_ui->NameBox);
    _WIG_ROL(_ui->AgeBox);
    _WIG_ROL(_ui->AddressBox);

    _WIG_DIS(_ui->MaleRadio);
    _WIG_DIS(_ui->FemaleRadio);
    //_WIG_DIS(_ui->SaveData);
    //_WIG_DIS(_ui->ResetData);
    _WIG_DIS(_ui->AddStudent);
    _WIG_DIS(_ui->DeleteStudent);
}

void FacultyController::loadStudentList() const
{
    _loadList(_facultyObj);
}

void FacultyController::aStudentSelected(long ID) const
{
    _loadBaseInformation(_facultyObj, ID);

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
        _loadScores(_facultyObj, ID, isMajor);
}

bool FacultyController::saveAStudent()
{
    long ID = _ui->IDBox->text().toLong();
    QMap<QString, int> temp;
    if(_facultyObj->isMajor(ID))
    {
        for(int i = 0; i < _ui->MajorScoreList->rowCount(); ++i)
        {
            auto item = _ui->MajorScoreList->item(i, 0);
            if(item)
                temp[item->text()] = _ui->MajorScoreList->item(i, 1)->text().toInt();
        }
        _facultyObj->saveAStudentScores(ID, true, temp);
    }
    else if(_facultyObj->isMinor(ID))
    {
        temp.clear();
        for(int i = 0; i < _ui->MinorScoreList->rowCount(); ++i)
        {
            auto item = _ui->MinorScoreList->item(i, 0);
            if(item)
                temp[item->text()] = _ui->MinorScoreList->item(i, 1)->text().toInt();
        }
        _facultyObj->saveAStudentScores(ID, false, temp);
    }
    return true;
}

QString FacultyController::displayReport() const
{
    return _facultyObj->makeReport(true);
}

void FacultyController::saveReport() const
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

void DegreesOfficeController::loadStudentList() const
{
    _loadList(_misObj);
}

void DegreesOfficeController::aStudentSelected(long ID) const
{
    _loadBaseInformation(_misObj, ID);
    _loadScores(_misObj, ID, true);
    _loadScores(_misObj, ID, false);
}

QString DegreesOfficeController::displayReport() const
{
    return _misObj->makeReport(true);
}

void DegreesOfficeController::saveReport() const
{
}

bool DegreesOfficeController::saveAStudent()
{
    throw QString("错误地调用了DegreesOfficeController::saveAStudent()方法！");
    return false;//这个方法不应该有任何实际的实现
}

QString AdmissionsOfficeController::displayReport() const
{
    throw QString("错误地调用了AdmissionsOfficeController::displayReport()方法！");
    return "";
}

void AdmissionsOfficeController::saveReport() const
{
    throw QString("错误地调用了AdmissionsOfficeController::saveReport()方法！");
}


}
