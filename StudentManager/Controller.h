#ifndef CONTROLLER_H
#define CONTROLLER_H

/**
 * @file 提供控制器层的类定义
 * @author Richard Tsai
 */

#include "Student.h"
#include "ui_StudentMain.h"


namespace Student
{

class IController
{
protected:
    Ui::StudentMain *_ui = 0;

    template<typename T>
    void loadList(T *modelObj)
    {
        _ui->StudentList->setSortingEnabled(false);
        int rowNo = 0;
        modelObj->loadStudentList([&rowNo, this](long ID, const QString &name){
            _ui->StudentList->setItem(rowNo, 0, new QTableWidgetItem(QString::number(ID)));
            _ui->StudentList->setItem(rowNo, 1, new QTableWidgetItem(name));
            ++rowNo;
        });
        _ui->StudentList->setSortingEnabled(true);
    }

    template<typename T>
    void loadBaseInformation(T *modelObj, long ID)
    {
        modelObj->loadAStudentDetails(ID, [this](const Student &studentObj){
            _ui->NameBox->setText(studentObj.getName());
            _ui->IDBox->setText(QString::number(studentObj.getID()));
            _ui->AgeBox->setValue(studentObj.getAge());
            if(studentObj.getSex())
                _ui->FemaleRadio->setChecked(true);
            else
                _ui->MaleRadio->setChecked(true);
            _ui->AddressBox->setText(studentObj.getAddress());
        });
    }

    template<typename T>
    void loadScores(T *modelObj, long ID, bool isMajor)
    {
        if(isMajor == true)
            _ui->MajorScoreList->clear();
        else
            _ui->MinorScoreList->clear();

        int rowNo = 0;
        auto pTableObj = isMajor ?
                    _ui->MajorScoreList :
                    _ui->MinorScoreList;

        pTableObj->setSortingEnabled(false);
        QString facultyName = modelObj->loadAStudentScores(
                    ID, isMajor,
                    [pTableObj, &rowNo](const QString &courseName, int score){
            pTableObj->setItem(rowNo, 0, new QTableWidgetItem(courseName));
            pTableObj->setItem(rowNo, 1, new QTableWidgetItem(QString::number(score)));
            pTableObj->setItem(rowNo, 2, new QTableWidgetItem(score >= 60 ? "PASS" : "FAIL"));
            ++rowNo;
        });
        pTableObj->setSortingEnabled(true);

        if(isMajor)
            _ui->MajorLabel->setText(facultyName);
        else
            _ui->MinorLabel->setText(facultyName);
    }

public:
    IController(Ui::StudentMain *ui): _ui(ui) {}
    IController(const IController &) = delete;
    virtual ~IController() {}

    virtual void loadStudentList() = 0;
    virtual void aStudentSelected(long ID) = 0;
    virtual void saveAStudent() = 0;
};

class AdmissionsOfficeController: public IController
{
private:
    StudentBase *_studentBaseObj = 0;

public:
    AdmissionsOfficeController(Ui::StudentMain *ui);

    ~AdmissionsOfficeController()
    {
        _CHK_DEL(_studentBaseObj);
    }

    void loadStudentList();
    void aStudentSelected(long ID);
    void saveAStudent();
};

class FacultyController: public IController
{
private:
    IFaculty *_facultyObj = 0;

public:
    enum FacultyName{FACULTY_NAME_A, FACULTY_NAME_B, FACULTY_NAME_C};

    FacultyController(Ui::StudentMain *ui, FacultyName facultyName)
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
    }

    ~FacultyController()
    {
        _CHK_DEL(_facultyObj);
    }

    void loadStudentList();
    void aStudentSelected(long ID);
    void saveAStudent();
};

class DegreesOfficeController: public IController
{
private:
    StudentMIS *_misObj = 0;

public:
    DegreesOfficeController(Ui::StudentMain *ui);

    ~DegreesOfficeController()
    {
        _CHK_DEL(_misObj);
    }

    void loadStudentList();
    void aStudentSelected(long ID);
    void saveAStudent();
};
}

#endif // CONTROLLER_H
