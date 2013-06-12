#ifndef STUDENT_H
#define STUDENT_H

/**
 * @file 提供数据模型层的类定义
 * @author Richard Tsai
 */

#include<QString>
#include<QMap>
#include<QSet>
#include<QPair>
#include<QVector>

#define _CHK_DEL(v) if(v)delete v

namespace Student
{

/**
 * @brief The Role enum
 *
 *标识不同登录身份的枚举类型。
 */
enum Role{ADMISSIONS_OFFICE, DEGREES_OFFICE, FACULTY_A, FACULTY_B, FACULTY_C};
enum FacultyName{FACULTY_NAME_A, FACULTY_NAME_B, FACULTY_NAME_C, FACULTY_NAME_NONE};

class Student;
class StudentBase;
class IFaculty;
class FacultyA;
class FacultyB;
class FacultyC;
class StudentMIS;

/**
 * @brief _stringToFacultyName 把字符串表示的专业名称转换成@enum FacultyName "Student.h" 类型
 * @param strName 字符串表示的专业名称，接受Faculty X、Faculty_X、Faculty_name_X的形式，不分大小写
 * @return strName表示的专业类型。
 */
inline FacultyName _stringToFacultyName(const QString &strName)
{
#define _CHK_FAC_NAM(x) \
if(strNameUpper == "FACULTY " #x\
|| strNameUpper == "FACULTY_" #x\
|| strNameUpper == "FACULTY_NAME_" #x)\
return FACULTY_NAME_##x

    QString strNameUpper = strName.toUpper();
    _CHK_FAC_NAM(A);
    _CHK_FAC_NAM(B);
    _CHK_FAC_NAM(C);
    //_CHK_FAC_NAM(NONE);

    return FACULTY_NAME_NONE;

#undef _CHK_FAC_NAM
}

/**
 * @brief _const 返回一个变量的常量引用，协助进行自动类型推断。
 * @param o 容器
 * @return o的常量引用
 */
template<typename T>
inline const T& _const(const T& o)
{
    return o;
}

/**
 * @brief The Student class
 *
 *表示一个学生，保存学生的基本个人信息。
 *仅提供读取接口。
 */
class Student
{
private:
    long _ID;
    QString _name, _address;
    bool _sex;
    int _age;

public:
    Student() = default;
    /**
     * @brief Student 使用学生基本信息初始化学生对象
     * @param name 姓名
     * @param ID 学号
     * @param sex 性别(false为男，true为女)
     * @param age 年龄
     * @param address 地址
     */
    Student(const QString &name, long ID, bool sex, int age, const QString &address)
        :_ID(ID), _name(name), _address(address), _sex(sex), _age(age) {}

    QString getName() const
    {
        return _name;
    }

    long getID() const
    {
        return _ID;
    }

    bool getSex() const
    {
        return _sex;
    }

    QString getAddress() const
    {
        return _address;
    }

    int getAge() const
    {
        return _age;
    }

    bool operator==(const Student &other) const
    {
        return _ID == other._ID;
    }

    bool operator<(const Student &other) const
    {
        return _ID < other._ID;
    }
};


/**
 * @brief The StudentBase class
 *
 *提供招生办工作人员操作的接口，同时也是@class FacultyA "Student.h",
 *@class FacultyB "Student.h", @class FacultyC "Student.h", @class StudentMIS "Student.h"
 *的虚基类。
 */
class StudentBase
{
private:
    static const char *_FILENAME;

    /**
     * @brief _loadDataFromFile 从@var StudentBase::_FILENAME 指定的文件名读取学生基本信息。
     * @return 读取成功返回true，失败返回false。
     */
    bool _loadDataFromFile();

    /**
     * @brief _saveDataToFile 把数据保存到本地文件中
     * @return 保存成功返回true，失败返回false
     */
    bool _saveDataToFile() const;

protected:
    QMap<long, Student> _studentList;

public:
    StudentBase()
    {
        _loadDataFromFile();
    }

    StudentBase(const StudentBase &) = delete;

    virtual ~StudentBase()
    {
        _saveDataToFile();
    }

    /**
     * @brief loadStudentList 通过传入的操作把学生姓名学号加载到列表中
     * @param f 一个类型为void (*)(long, QString)的函数指针或有兼容原型
     *的函数、函数对象、lambda表达式。loadStudentList函数会对每个学生
     *调用f，并传入学号和学生姓名。
     * @param count size_t类型的指针，返回学生总数。
     */
    template<typename FunType>
    void loadStudentList(FunType f, size_t *count = 0) const
    {
        for(auto i : _const(_studentList))
            f(i.getID(), i.getName());
        if(count)
            *count = _studentList.size();
    }

    /**
     * @brief loadAStudentDetails 通过传入的操作加载指定学号学生的详细信息
     * @param ID 学生学号
     * @param f 一个类型为void (*)(const Student &)的函数指针或有兼容原型
     *的函数、函数对象、lambda表达式。提供加载学生详情的具体操作。如果没有
     *找到对应学号的学生，则不会调用此函数。
     */
    template<typename FunType>
    void loadAStudentDetails(long ID, FunType f) const
    {
        auto i = _studentList.constFind(ID);
        if(i != _studentList.constEnd())
            f(*i);
    }

    /**
     * @brief saveAStudentDetails 保存一个学生的详细信息
     * @param s @class Student "Student.h" 对象，待保存的学生信息。
     */
    void saveAStudentDetails(Student &s);

    void addAStudent(Student &s, FacultyName majorFaculty, FacultyName minorFaculty);

    void deleteAStudent(long ID);

    /**
     * @brief testStudentID 检查指定的学号是否存在当前的学生列表中
     * @param ID 指定的学号
     * @return 存在返回true，不存在返回false
     */
    bool checkIDExists(long ID) const;

    size_t getStudentCount() const
    {
        return _studentList.size();
    }
};


/**
 * @brief The IFaculty class
 *
 *专业类(Faculty*)的基类。包含成绩列表以及相关的各种操作。
 *由于不同专业的数据不共享，专业类**不**应该对本类进行虚继承。
 */
class IFaculty: virtual protected StudentBase
{
private:
    QStringList _findGoodStudents(bool isHTML) const;
    QStringList _findFailedStudents(
            bool isHTML, const QMap<long, QMap<QString, int> > &scoreList) const;

protected:
    QMap<long, QMap<QString, int> > _majorStudentScores, _minorStudentScores;
    virtual bool _loadDataFromFile(bool isMajor);
    virtual bool _saveDataToFile(bool isMajor) const;

    /**
     * @brief _achieveDegree 获取获得学位的学生集合
     * @param isMajor 是否主修学生
     * @return 获得学位的学生的学号集合
     * @warning 查询辅修学位时，不会排除未获得主修学位的学生，
     *因此需要计算与所有专业获得主修学位学生集合的交集。
     */
    virtual QSet<long> _achieveDegree(bool isMajor) const;
    int _s, _k;
    QString _facultyName;
    const QString _FILENAME_MAJOR;
    const QString _FILENAME_MINOR;
    const QString _FILENAME_GOOD;
    const QString _FILENAME_MAJOR_FAIL;
    const QString _FILENAME_MINOR_FAIL;

public:
    /**
     * @brief IFaculty 通过课程设置初始化IFaculty类
     * @param s 主修课程数
     * @param k 辅修课程数
     */
    IFaculty(int s, int k, const QString &facultyName,
             const QString &fileNameMajor, const QString &fileNameMinor,
             const QString &fileNameGood,
             const QString &fileNameMajorFail, const QString &fileNameMinorFail)
        : _s(s), _k(k), _facultyName(facultyName),
          _FILENAME_MAJOR(fileNameMajor), _FILENAME_MINOR(fileNameMinor),
          _FILENAME_GOOD(fileNameGood),
          _FILENAME_MAJOR_FAIL(fileNameMajorFail), _FILENAME_MINOR_FAIL(fileNameMinorFail) {}

    virtual ~IFaculty() {}

    virtual QString makeReport(bool isHTML = true,
            QString *pGood = 0, QString *pMajorFailed = 0, QString *pMinorFailed = 0) const;
    virtual bool saveReport() const;
    void saveAStudentScores(long ID, bool isMajor, QMap<QString, int> scores);
    void deleteAStudentScores(long ID, bool isMajor)
    {
        (isMajor ? _majorStudentScores : _minorStudentScores).remove(ID);
    }

    bool isMajor(long ID) const
    {
        return _majorStudentScores.contains(ID);
    }
    bool isMinor(long ID) const
    {
        return _minorStudentScores.contains(ID);
    }

    template<typename FunType>
    QString loadAStudentScores(long ID, bool isMajor, FunType f, size_t *count = 0) const
    {
        auto pStudentScores = isMajor ? &_majorStudentScores : &_minorStudentScores;
        if(pStudentScores->contains(ID))
        {
            for(auto i = pStudentScores->value(ID).constBegin();
                i != pStudentScores->value(ID).constEnd(); ++i)
                f(i.key(), i.value());

            if(count)
                *count = pStudentScores->value(ID).size();

            return _facultyName;
        }
        else
        {
            return "";
        }
    }

    size_t getScoreListCount(long ID, bool isMajor) const
    {
        auto p = isMajor ? &_majorStudentScores : &_minorStudentScores;
        if(p->contains(ID))
            return (*p)[ID].size();
        else
            return 0;
    }

    QString getFacultyName() const
    {
        return _facultyName;
    }

    using StudentBase::loadStudentList;
    using StudentBase::loadAStudentDetails;
    using StudentBase::checkIDExists;
    using StudentBase::getStudentCount;
};


/**
 * @brief The FacultyA class
 */
class FacultyA: public IFaculty
{
public:
    FacultyA(int s = 6, int k = 5)
        :IFaculty(s, k, "专业A",
                  "AMajor.DAT", "AMinor.DAT", "AGood.DAT", "AMajorFail.DAT", "AMinorFail.DAT")
    {
        IFaculty::_loadDataFromFile(true);
        IFaculty::_loadDataFromFile(false);
    }

    ~FacultyA()
    {
        IFaculty::_saveDataToFile(true);
        IFaculty::_saveDataToFile(false);
    }

};


class FacultyB: public IFaculty
{
public:
    FacultyB(int s = 7, int k = 4)
        :IFaculty(s, k, "专业B",
                  "BMajor.DAT", "BMinor.DAT", "BGood.DAT", "BMajorFail.DAT", "BMinorFail.DAT")
    {
        IFaculty::_loadDataFromFile(true);
        IFaculty::_loadDataFromFile(false);
    }

    ~FacultyB()
    {
        IFaculty::_saveDataToFile(true);
        IFaculty::_saveDataToFile(false);
    }

};

class FacultyC: public IFaculty
{
public:
    FacultyC(int s = 5, int k = 3)
        :IFaculty(s, k, "专业C",
                  "CMajor.DAT", "CMinor.DAT", "CGood.DAT", "CMajorFail.DAT", "CMinorFail.DAT")
    {
        IFaculty::_loadDataFromFile(true);
        IFaculty::_loadDataFromFile(false);
    }

    ~FacultyC()
    {
        IFaculty::_saveDataToFile(true);
        IFaculty::_saveDataToFile(false);
    }

};


inline IFaculty * _facultyNameToFacultyObject(FacultyName name)
{
    switch(name)
    {
    case FACULTY_NAME_A:
        return new FacultyA;
    case FACULTY_NAME_B:
        return new FacultyB;
    case FACULTY_NAME_C:
        return new FacultyC;
    case FACULTY_NAME_NONE:
        return nullptr;
    }
    return nullptr;
}


class StudentMIS: protected FacultyA, protected FacultyB, protected FacultyC
{
private:
    void saveAStudentScores(long ID, bool isMajor, QMap<QString, int> scores);
    QStringList _getReportLine(
            const QSet<long> &idList, const QString FacultyName, bool isHTML, bool isMajor) const;
    const QString _FILENAME_REPORT = "Degree.dat";

public:
    using StudentBase::loadStudentList;
    using StudentBase::loadAStudentDetails;
    using StudentBase::checkIDExists;
    using StudentBase::getStudentCount;

    template<typename FunType>
    QString loadAStudentScores(long ID, bool isMajor, FunType f, size_t *count = 0) const
    {
        QString facultyName = FacultyA::loadAStudentScores(ID, isMajor, f, count);
        if(!facultyName.size())
            facultyName = FacultyB::loadAStudentScores(ID, isMajor, f, count);
        if(!facultyName.size())
            facultyName = FacultyC::loadAStudentScores(ID, isMajor, f, count);
        return facultyName;
    }

    QString makeReport(bool isHTML) const;
    bool saveReport() const;
};

}
#endif // STUDENT_H
