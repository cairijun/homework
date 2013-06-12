#include "Student.h"
#include<QString>
#include<QStringList>
#include<QFile>
#include<QTextStream>
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
    QFile fileObj(_FILENAME);
    if(!fileObj.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream fStream(&fileObj);
    while(!fStream.atEnd())
    {
        long ID, age;
        QString name, address;
        int sex;//QTextStream的流提取运算符不支持bool类型，这里使用int代替
        QString line = fStream.readLine();
        QTextStream lineStream(&line);
        lineStream >> ID >> name >> sex >> age >> address;
        _studentList[ID] = Student(name, ID, sex, age, address);
    }
    fileObj.close();
    return true;
}

/**
 * @brief _saveDataToFile 把数据保存到本地文件中
 * @return 保存成功返回true，失败返回false
 */
bool StudentBase::_saveDataToFile() const
{
    QFile fileObj(_FILENAME);
    if(!fileObj.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;

    QTextStream fStream(&fileObj);
    for(Student aStudent: _studentList)
        fStream << aStudent.getID() << " "
                << aStudent.getName() << " "
                << aStudent.getSex() << " "
                << aStudent.getAge() << " "
                << aStudent.getAddress() << "\n";

    fileObj.close();
    return true;
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
    auto pMajorFaculty = _facultyNameToFacultyObject(majorFaculty);
    auto pMinorFaculty = _facultyNameToFacultyObject(minorFaculty);
    pMajorFaculty->saveAStudentScores(s.getID(), true, QMap<QString, int>());
    if(pMinorFaculty != nullptr)
        pMinorFaculty->saveAStudentScores(s.getID(), false, QMap<QString, int>());
    delete pMajorFaculty;
    delete pMinorFaculty;
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


bool IFaculty::_loadDataFromFile(bool isMajor)
{
    QFile fileObj(isMajor ? _FILENAME_MAJOR : _FILENAME_MINOR);
    auto pScoreList = isMajor ? &_majorStudentScores : &_minorStudentScores;

    if(!fileObj.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream fStream(&fileObj);
    while(!fStream.atEnd())
    {
        QString line = fStream.readLine();
        long id, score;
        QString subjectName;
        QTextStream lineStream(&line);
        lineStream >> id;
        if(!StudentBase::checkIDExists(id))//排除已被删除的学生
            continue;
        QMap<QString, int> scoresOfAStudent;
        while(!lineStream.atEnd())
        {
            lineStream >> subjectName >> score;
            if(subjectName.size())//防止行尾空格干扰
                scoresOfAStudent[subjectName] = score;
        }
        (*pScoreList)[id] = scoresOfAStudent;
    }
    fileObj.close();
    return true;
}

bool IFaculty::_saveDataToFile(bool isMajor) const
{
    QFile fileObj(isMajor ? _FILENAME_MAJOR : _FILENAME_MINOR);
    if(!fileObj.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;

    QTextStream fStream(&fileObj);
    auto pScoreList = isMajor ? &_majorStudentScores : &_minorStudentScores;
    for(auto i = pScoreList->constBegin(); i != pScoreList->constEnd(); ++i)
    {
        fStream << i.key() << " ";
        for(auto j = i.value().constBegin(); j != i.value().constEnd(); ++j)
            fStream << j.key() << " " << j.value() << " ";
        fStream << "\n";
    }
    fileObj.close();
    return true;
}

QSet<long> IFaculty::_achieveDegree(bool isMajor) const
{
    QSet<long> returnIDs;
    auto scoreList = isMajor ? &_majorStudentScores : &_minorStudentScores;
    for(auto i = scoreList->constBegin(); i != scoreList->constEnd(); ++i)
        if(i.value().size() >= (isMajor ? _s : _k))//检查修读课程数
        {
            bool isAchieve = true;
            for(auto aScore: i.value())
                if(aScore < 60)
                {
                    isAchieve = false;
                    break;
                }
            if(isAchieve)
                returnIDs.insert(i.key());
        }
    return returnIDs;
}

QString IFaculty::makeReport(
        bool isHTML,
        QString *pGood, QString *pMajorFailed, QString *pMinorFailed) const
{
    QString goodStudentStr = _findGoodStudents(isHTML).join("\n");
    QString majorFailed = _findFailedStudents(isHTML, _majorStudentScores).join("\n");
    QString minorFailed = _findFailedStudents(isHTML, _minorStudentScores).join("\n");
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

bool IFaculty::saveReport() const
{
    QString goodStr, majorFailedStr, minorFailedStr;
    makeReport(false, &goodStr, &majorFailedStr, &minorFailedStr);
    QFile fObjGood(_FILENAME_GOOD);
    if(!fObjGood.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;
    fObjGood.write(goodStr.toLocal8Bit());
    fObjGood.write("\n");
    fObjGood.close();

    QFile fObjMajorFailed(_FILENAME_MAJOR_FAIL);
    if(!fObjMajorFailed.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;
    fObjMajorFailed.write(majorFailedStr.toLocal8Bit());
    fObjMajorFailed.write("\n");
    fObjMajorFailed.close();

    QFile fObjMinorFailed(_FILENAME_MINOR_FAIL);
    if(!fObjMinorFailed.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;
    fObjMinorFailed.write(minorFailedStr.toLocal8Bit());
    fObjMinorFailed.write("\n");
    fObjMinorFailed.close();

    return true;
}

void IFaculty::saveAStudentScores(long ID, bool isMajor, QMap<QString, int> scores)
{
    auto pScoreList = isMajor ? &_majorStudentScores : &_minorStudentScores;
    pScoreList->insert(ID, scores);
}

const char * StudentBase::_FILENAME = "Basefile.DAT";

QString StudentMIS::makeReport(bool isHTML) const
{
    auto majorA = FacultyA::_achieveDegree(true);
    auto majorB = FacultyB::_achieveDegree(true);
    auto majorC = FacultyC::_achieveDegree(true);

    //计算所有获得主修学位的学生集合，用于计算获得辅修学位学生的集合
    auto allMajor = majorA;
    allMajor.unite(majorB).unite(majorC);
    auto minorA = FacultyA::_achieveDegree(false).intersect(allMajor);
    auto minorB = FacultyB::_achieveDegree(false).intersect(allMajor);
    auto minorC = FacultyC::_achieveDegree(false).intersect(allMajor);

    //计算未获得学位的学生集合。注意只需要减去获得主修学位学生即可，因为辅修学生是主修学生的子集。
    auto noDegree = _studentList.keys().toSet().subtract(allMajor);

    auto majorDegreeLines = _getReportLine(majorA, FacultyA::getFacultyName(), isHTML, true);
    majorDegreeLines.append(_getReportLine(majorB, FacultyB::getFacultyName(), isHTML, true));
    majorDegreeLines.append(_getReportLine(majorC, FacultyC::getFacultyName(), isHTML, true));

    auto minorDegreeLines = _getReportLine(minorA, FacultyA::getFacultyName(), isHTML, false);
    minorDegreeLines.append(_getReportLine(minorB, FacultyB::getFacultyName(), isHTML, false));
    minorDegreeLines.append(_getReportLine(minorC, FacultyC::getFacultyName(), isHTML, false));

    auto noDegreeLines = _getReportLine(noDegree, "无", isHTML, true);

    if(isHTML)
        return QString("<h1 align=\"center\">成绩统计报告</h1>"
                       "<h2>获得主修学位学生</h2><ol>%1</ol>"
                       "<h2>获得辅修学位学生</h2><ol>%2</ol>"
                       "<h2>未获得学位学生</h2><ol>%3</ol>")
                .arg(majorDegreeLines.join("\n"))
                .arg(minorDegreeLines.join("\n"))
                .arg(noDegreeLines.join("\n"));
    else
        return (majorDegreeLines + minorDegreeLines + noDegreeLines).join("\n");
}

bool StudentMIS::saveReport() const
{
    QFile fObj(_FILENAME_REPORT);
    if(!fObj.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;
    fObj.write(makeReport(false).toLocal8Bit());
    fObj.write("\n");
    return true;
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
            if(_studentList.contains(i.key()))
            {
                auto aStudent = _studentList[i.key()];
                returnBuf << QString(isHTML ? "<li><b>%1</b><br />学号：%2</li>" : "%1 %2")
                             .arg(aStudent.getName()).arg(aStudent.getID());
            }
    }
    return returnBuf;
}

QStringList IFaculty::_findFailedStudents(
        bool isHTML, const QMap<long, QMap<QString, int> > &scoreList) const
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
                    if(!_studentList.contains(i.key()))
                        continue;
                    auto aStudentObj = _studentList[i.key()];
                    if(isHTML)
                        aStudentData << "<li><b>" << aStudentObj.getName()
                                     << "</b><br />学号：" << QString::number(aStudentObj.getID())
                                     << "<br />补考科目：";
                    else
                        aStudentData << aStudentObj.getName() << " "
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

QStringList StudentMIS::_getReportLine(
        const QSet<long> &idList, const QString FacultyName, bool isHTML, bool isMajor) const
{
    QStringList returnBuf;
    for(long id: idList)
    {
        if(!_studentList.contains(id))
            continue;
        auto studentObj = _studentList[id];
        returnBuf << QString(isHTML ? "<li><b>%1</b><br />学号：%2<br />学位专业：%3</li>"
                                    : isMajor ? "%1 %2 %3 Major" : "%1 %2 %3 Minor")
                         .arg(studentObj.getName())
                         .arg(QString::number(studentObj.getID()))
                         .arg(FacultyName);
    }
    return returnBuf;
}

}
