#include "Student.h"
#include<QString>
#include<QStringList>
#include<algorithm>
#include<iterator>
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

QString IFaculty::makeReport(
        bool isHTML,
        QString *pGood, QString *pMajorFailed, QString *pMinorFailed) const
{
    QString goodStudentStr = _findGoodStudents(isHTML).join("\n");
    QString majorFailed = _findFailedStudents(isHTML, _majorStudentScores, true).join("");
    QString minorFailed = _findFailedStudents(isHTML, _minorStudentScores, false).join("");
    if(pGood)
        *pGood = goodStudentStr;
    if(pMajorFailed)
        *pMajorFailed = majorFailed;
    if(pMinorFailed)
        *pMinorFailed = minorFailed;
    if(isHTML)
        return QString("<h1 align=\"center\">%1成绩统计报告</h1>"
                       "<h2>主修优秀学生</h2><ol>%2</ol>"
                       "<h2>补考学生（主修）</h2><ol>%3</ol>"
                       "<h2>补考学生（辅修）</h2><ol>%4</ol>")
                .arg(_facultyName).arg(goodStudentStr).arg(majorFailed).arg(minorFailed);
    else
        return goodStudentStr + majorFailed + minorFailed;
}

void IFaculty::saveAStudentScores(long ID, bool isMajor, QMap<QString, int> scores)
{
    if(scores.size())
    {
        auto pScoreList = isMajor ? &_majorStudentScores : &_minorStudentScores;
        pScoreList->insert(ID, scores);
    }
}

const char * StudentBase::_FILENAME = "BaseFile.dat";
const char * FacultyA::_FILENAME_MAJOR = "AMajor.dat";
const char * FacultyA::_FILENAME_MINOR = "AMinor.dat";
const char * FacultyB::_FILENAME_MAJOR = "BMajor.dat";
const char * FacultyB::_FILENAME_MINOR = "BMinor.dat";
const char * FacultyC::_FILENAME_MAJOR = "CMajor.dat";
const char * FacultyC::_FILENAME_MINOR = "CMinor.dat";

QString StudentMIS::makeReport(bool isHTML) const
{
    if(isHTML)
    {
        return QString("<h1 align=\"center\">成绩统计报告</h1>");
    }
    else
    {
        return "";
    }
}

QStringList IFaculty::_findGoodStudents(bool isHTML) const
{
    QStringList returnBuf;
    for(auto i = _majorStudentScores.constBegin(); i != _majorStudentScores.constEnd(); ++i)
    {
        size_t countOver85 = 0;
        bool isOver70 = true;
        for(auto score: i.value())
        {
            if(score >= 85)
                ++countOver85;
            if(score < 70)
            {
                isOver70 = false;
                break;
            }
        }
        if(countOver85 >= 3 && isOver70)
        {
            auto aStudent = _studentList[i.key()];
            returnBuf << QString(isHTML ? "<li>%1<br />学号：%2</li>" : "%1 %2")
                         .arg(aStudent.getName()).arg(aStudent.getID());
        }
    }
    return returnBuf;
}

QStringList IFaculty::_findFailedStudents(
        bool isHTML, const QMap<long, QMap<QString, int> > &scoreList, bool isMajor) const
{
    QStringList returnBuf;
    for(auto i = scoreList.constBegin(); i != scoreList.constEnd(); ++i)
    {
        bool isFailed = false;
        QStringList aStudentData;
        for(auto j = i.value().constBegin(); j != i.value().constEnd(); ++j)
            if(j.value() < 60)
            {
                if(!isFailed)
                {
                    isFailed = true;
                    auto aStudentObj = _studentList[i.key()];
                    if(isHTML)
                        aStudentData << "<li>" << aStudentObj.getName()
                                     << "<br />学号：" << QString::number(aStudentObj.getID())
                                     << "<br />重修科目：";
                    else
                        aStudentData << aStudentObj.getName()
                                     << QString::number(aStudentObj.getID()) << " ";
                }
                aStudentData << j.key() << " ";
            }
        if(isFailed)
        {
            if(isHTML)
                aStudentData << "</li>";
            returnBuf << aStudentData.join("");
        }
    }
    return returnBuf;
}

}
