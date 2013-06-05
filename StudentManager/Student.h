#ifndef STUDENT_H
#define STUDENT_H

/**
 * @file 提供数据模型层的类定义
 * @author Richard Tsai
 */

#include<QString>
#include<QMap>
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
 *友元：@class StudentBase "Student.h"
 */
class Student
{
    friend class StudentBase;

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
    QMap<long, Student> _studentList;
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
    bool _saveDataToFile();
public:
    StudentBase()
    {
        _loadDataFromFile();
    }

    virtual ~StudentBase()
    {
        _saveDataToFile();
    }

    /**
     * @brief loadStudentList 通过传入的操作把学生姓名学号加载到列表中
     * @param f 一个类型为void (*)(long, QString)的函数指针或有兼容原型
     *的函数、函数对象、lambda表达式。loadStudentList函数会对每个学生
     *调用f，并传入学号和学生姓名。
     */
    template<typename FunType>
    void loadStudentList(FunType f)
    {
        for(auto i : _const(_studentList))
            f(i.getID(), i.getName());
    }

    /**
     * @brief loadAStudentDetails 通过传入的操作加载指定学号学生的详细信息
     * @param ID 学生学号
     * @param f 一个类型为void (*)(const Student &)的函数指针或有兼容原型
     *的函数、函数对象、lambda表达式。提供加载学生详情的具体操作。如果没有
     *找到对应学号的学生，则不会调用此函数。
     */
    template<typename FunType>
    void loadAStudentDetails(long ID, FunType f)
    {
        auto i = _studentList.constFind(ID);
        if(i != _studentList.constEnd())
            f(*i);
    }

    /**
     * @brief saveAStudentDetails 保存一个学生的详细信息
     * @param s @class Student "Student.h" 对象，待保存的学生信息。若该
     *学号在学生列表中不存在，则添加到学生列表中。
     */
    void saveAStudentDetails(Student &s);

    /**
     * @brief testStudentID 检查指定的学号是否存在当前的学生列表中
     * @param ID 指定的学号
     * @return 存在返回true，不存在返回false
     */
    bool testStudentID(long ID);
};


/**
 * @brief The IFaculty class
 *
 *专业类(Faculty*)的基类。包含成绩列表以及相关的各种操作。
 *由于不同专业的数据不共享，专业类**不**应该对本类进行虚继承。
 */
class IFaculty: virtual protected StudentBase
{
protected:
    QMap<long, QMap<QString, int> > _majorStudentScores, _minorStudentScores;
    virtual bool _loadDataFromFile(const char *majorFileName, const char *minorFileName);
    virtual bool _saveDataToFile(const char *majorFileName, const char *minorFileName);
    int _s, _k;

public:
    /**
     * @brief IFaculty 通过课程设置初始化IFaculty类
     * @param s 主修课程数
     * @param k 辅修课程数
     */
    IFaculty(int s, int k): _s(s), _k(k) {}
    virtual ~IFaculty() {}
    virtual void makeReport(const char *header);
    virtual void saveAStudentScores(long ID, QMap<QString, int> scores);

    using StudentBase::loadStudentList;
    using StudentBase::loadAStudentDetails;
    using StudentBase::testStudentID;
};


/**
 * @brief The FacultyA class
 */
class FacultyA: public IFaculty
{
private:
    static const char *_FILENAME_MAJOR;
    static const char *_FILENAME_MINOR;

public:
    FacultyA(int s = 6, int k = 5)
        :IFaculty(s, k)
    {
        IFaculty::_loadDataFromFile(_FILENAME_MAJOR, _FILENAME_MINOR);
    }

    ~FacultyA()
    {
        IFaculty::_saveDataToFile(_FILENAME_MAJOR, _FILENAME_MINOR);
    }

};


class FacultyB: public IFaculty
{
private:
    static const char *_FILENAME_MAJOR;
    static const char *_FILENAME_MINOR;

public:
    FacultyB(int s = 7, int k = 4)
        :IFaculty(s, k)
    {
        IFaculty::_loadDataFromFile(_FILENAME_MAJOR, _FILENAME_MINOR);
    }

    ~FacultyB()
    {
        IFaculty::_saveDataToFile(_FILENAME_MAJOR, _FILENAME_MINOR);
    }

};

class FacultyC: public IFaculty
{
private:
    static const char *_FILENAME_MAJOR;
    static const char *_FILENAME_MINOR;

public:
    FacultyC(int s = 5, int k = 3)
        :IFaculty(s, k)
    {
        IFaculty::_loadDataFromFile(_FILENAME_MAJOR, _FILENAME_MINOR);
    }

    ~FacultyC()
    {
        IFaculty::_saveDataToFile(_FILENAME_MAJOR, _FILENAME_MINOR);
    }

};


class StudentMIS: public FacultyA, public FacultyB, public FacultyC
{
public:
    using StudentBase::loadStudentList;
    using StudentBase::loadAStudentDetails;
    using StudentBase::testStudentID;
};

}
#endif // STUDENT_H
