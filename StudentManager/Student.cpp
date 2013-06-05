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
    const QString &t = "abc";
    _studentList[12348003] = Student("Richard Tsai", 12348003, false, 19, "SYSU");
    _studentList[12348004] = Student("Someone", 12348004, true, 18, "SYSU");
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
 * @param s @class Student "Student.h" 对象，待保存的学生信息。若该
 *学号在学生列表中不存在，则添加到学生列表中。
 */
void StudentBase::saveAStudentDetails(Student &s)
{

}

/**
 * @brief testStudentID 检查指定的学号是否存在当前的学生列表中
 * @param ID 指定的学号
 * @return 存在返回true，不存在返回false
 */
bool StudentBase::testStudentID(long ID)
{

}



bool IFaculty::_loadDataFromFile(const char *majorFileName, const char *minorFileName)
{

}

bool IFaculty::_saveDataToFile(const char *majorFileName, const char *minorFileName)
{

}

void IFaculty::makeReport(const char *header)
{

}

void IFaculty::saveAStudentScores(long ID, QMap<QString, int> scores)
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
