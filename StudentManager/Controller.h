#ifndef CONTROLLER_H
#define CONTROLLER_H

/**
 * @file 提供控制器层的类定义
 * @author Richard Tsai
 */

#include "Student.h"
#include "ui_StudentMain.h"
#include<QMap>


namespace Student
{

class IController
{
protected:
    Ui::StudentMain *_ui = 0;

    /**
     * @brief _args 用于保存额外的参数
     *
     *为了保持接口一致，视图层与控制器层交互时，如果有某些独立于兄弟类型的
     *特殊参数需要传递，应通过重载的[]运算符使用这一保护成员进行传递。
     */
    QMap<QString, QString> _args;

    template<typename T>
    void _loadList(T *modelObj) const
    {
        _ui->StudentList->setSortingEnabled(false);
        _ui->StudentList->setRowCount(modelObj->getStudentCount());
        int rowNo = 0;
        modelObj->loadStudentList([&rowNo, this](long ID, const QString &name){
            _ui->StudentList->setItem(rowNo, 0, new QTableWidgetItem(QString::number(ID)));
            _ui->StudentList->setItem(rowNo, 1, new QTableWidgetItem(name));
            ++rowNo;
        });
        _ui->StudentList->setSortingEnabled(true);
        _ui->StudentList->sortItems(0);
    }

    template<typename T>
    void _loadBaseInformation(T *modelObj, long ID) const
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
    void _loadScores(T *modelObj, long ID, bool isMajor) const
    {
        if(isMajor)
            _ui->MajorScoreList->clearContents();
        else
            _ui->MinorScoreList->clearContents();

        int rowNo = 0;
        auto pTableObj = isMajor ?
                    _ui->MajorScoreList :
                    _ui->MinorScoreList;

        //pTableObj->setRowCount(modelObj->getScoreListCount(ID, isMajor));

        pTableObj->setSortingEnabled(false);
        pTableObj->setRowCount(20);//预先设置足够的行，防止无法写入
        QString facultyName = modelObj->loadAStudentScores(
                    ID, isMajor,
                    [pTableObj, &rowNo](const QString &courseName, int score){
            pTableObj->setItem(rowNo, 0, new QTableWidgetItem(courseName));
            pTableObj->setItem(rowNo, 1, new QTableWidgetItem(QString::number(score)));
            pTableObj->setItem(rowNo, 2, new QTableWidgetItem(score >= 60 ? "过" : "挂"));
            ++rowNo;
        });
        pTableObj->setSortingEnabled(true);
        pTableObj->sortItems(0);
        pTableObj->setRowCount(rowNo);

        if(isMajor)
            _ui->MajorLabel->setText(facultyName);
        else
            _ui->MinorLabel->setText(facultyName);
    }

public:
    IController(Ui::StudentMain *ui): _ui(ui) {}
    IController(const IController &) = delete;
    virtual ~IController() {}

    virtual void loadStudentList() const = 0;
    virtual void aStudentSelected(long ID) const = 0;
    virtual bool saveAStudent() = 0;
    virtual QString displayReport() const = 0;
    virtual bool saveReport() const = 0;

    /**
     * @brief operator [] 重载的[]运算符，用于访问额外参数
     * @param argKey 参数的键
     * @return 对参数值的引用
     */
    QString & operator[](const QString &argKey)
    {
        return _args[argKey];
    }

    virtual bool checkIDExists(long ID) = 0;
};

class AdmissionsOfficeController: public IController
{
private:
    StudentBase *_studentBaseObj = 0;
    QString displayReport() const;
    bool saveReport() const;

public:
    AdmissionsOfficeController(Ui::StudentMain *ui);

    ~AdmissionsOfficeController()
    {
        _CHK_DEL(_studentBaseObj);
    }

    void loadStudentList() const;
    void aStudentSelected(long ID) const;
    bool saveAStudent();
    bool checkIDExists(long ID)
    {
        return _studentBaseObj->checkIDExists(ID);
    }

    void deleteStudent(long ID)
    {
        _studentBaseObj->deleteAStudent(ID);
    }
};

class FacultyController: public IController
{
private:
    IFaculty *_facultyObj = 0;

public:

    FacultyController(Ui::StudentMain *ui, FacultyName facultyName);

    ~FacultyController()
    {
        _CHK_DEL(_facultyObj);
    }

    void loadStudentList() const;
    void aStudentSelected(long ID) const;
    bool saveAStudent();
    QString displayReport() const;
    bool saveReport() const;
    bool checkIDExists(long ID)
    {
        return _facultyObj->checkIDExists(ID);
    }
};

class DegreesOfficeController: public IController
{
private:
    StudentMIS *_misObj = 0;
    bool saveAStudent();

public:
    DegreesOfficeController(Ui::StudentMain *ui);

    ~DegreesOfficeController()
    {
        _CHK_DEL(_misObj);
    }

    void loadStudentList() const;
    void aStudentSelected(long ID) const;
    QString displayReport() const;
    bool saveReport() const;
    bool checkIDExists(long ID)
    {
        return _misObj->checkIDExists(ID);
    }
};
}

#endif // CONTROLLER_H
