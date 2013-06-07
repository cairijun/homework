#include "Student.h"
#include<QString>
/**
 * @file 提供数据模型层的类定义
 * @author Richard Tsai
 */
namespace Student
{

/**
 * @brief _loadDataFromFile 从@var StudentBase::_FILENAME 指定的文件名读取学生基本信息。
 * @return 读取成功返回true，失败返回false。
 */
bool StudentBase::_loadDataFromFile()
{
    //const QString &t = "abc";
    _studentList[12348003] = Student("Richard Tsai", 12348003, false, 19, "SYSU");
    _studentList[12348004] = Student("Someone", 12348004, true, 18, "SYSU");
    _studentList[12348005] = Student("AAA", 12348005, false, 19, "ABC");
    _studentList[12348006] = Student("XXX", 12348006, false, 20, "frlkajsl");
}

/**
 * @brief _saveDataToFile 把数据保存到本地文件中
 * @return 保存成功返回true，失败返回false
 */
bool StudentBase::_saveDataToFile()
{

}

/**
 * @brief saveAStudentDetails 保存一个学生的详细信息
 * @param s @class Student "Student.h" 对象，待保存的学生信息。
 */
void StudentBase::saveAStudentDetails(Student &s)
{
    auto i = _studentList.find(s.getID());
    if(i != _studentList.end())
        *i = s;
}

void StudentBase::addAStudent(Student &s, FacultyName majorFaculty, FacultyName minorFaculty)
{
    _studentList[s.getID()] = s;
    //To-do: 写入专业信息
}

void StudentBase::deleteAStudent(long ID)
{
    _studentList.remove(ID);
}

/**
 * @brief testStudentID 检查指定的学号是否存在当前的学生列表中
 * @param ID 指定的学号
 * @return 存在返回true，不存在返回false
 */
bool StudentBase::checkIDExists(long ID) const
{
    return _studentList.find(ID) != _studentList.end();
}



bool IFaculty::_loadDataFromFile(const char *majorFileName, const char *minorFileName)
{
    if(majorFileName == QString("AMajor.dat"))
    {
        _majorStudentScores[12348003] = QMap<QString, int>();
        auto p = &_majorStudentScores[12348003];
        p->operator []("C1") = 100;
        p->operator []("C2") = 50;
        _minorStudentScores[12348004] = QMap<QString, int>();
        p = &_minorStudentScores[12348004];
        p->operator []("C1") = 60;
        p->operator []("C2") = 50;
    }
    if(majorFileName == QString("BMajor.dat"))
    {
        _majorStudentScores[12348004] = QMap<QString, int>();
        auto p = &_majorStudentScores[12348004];
        p->operator []("C1") = 76;
        p->operator []("C2") = 9;
        _minorStudentScores[12348005] = QMap<QString, int>();
        p = &_minorStudentScores[12348005];
        p->operator []("C1") = 90;
        p->operator []("C2") = 15;
    }
    if(majorFileName == QString("CMajor.dat"))
    {
        _majorStudentScores[12348005] = QMap<QString, int>();
        auto p = &_majorStudentScores[12348005];
        p->operator []("C1") = 46;
        p->operator []("C2") = 80;
        _minorStudentScores[12348003] = QMap<QString, int>();
        p = &_minorStudentScores[12348003];
        p->operator []("C1") = 90;
        p->operator []("C2") = 90;
    }
}

bool IFaculty::_saveDataToFile(const char *majorFileName, const char *minorFileName)
{

}

void IFaculty::makeReport()
{

}

void IFaculty::saveAStudentScores(long ID, bool isMajor, QMap<QString, int> scores)
{

}

const char * StudentBase::_FILENAME = "BaseFile.dat";
const char * FacultyA::_FILENAME_MAJOR = "AMajor.dat";
const char * FacultyA::_FILENAME_MINOR = "AMinor.dat";
const char * FacultyB::_FILENAME_MAJOR = "BMajor.dat";
const char * FacultyB::_FILENAME_MINOR = "BMinor.dat";
const char * FacultyC::_FILENAME_MAJOR = "CMajor.dat";
const char * FacultyC::_FILENAME_MINOR = "CMinor.dat";

}
