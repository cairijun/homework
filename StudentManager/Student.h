#ifndef STUDENT_H
#define STUDENT_H

#include<QString>
#include<QMap>
/**
 * @file 提供数据模型层的类定义
 * @author Richard Tsai
 */

namespace Student
{

/**
 * @brief The Role enum
 *
 *标识不同登录身份的枚举类型。
 */
enum Role{ADMISSIONS_OFFICE, DEGREES_OFFICE, FACULTY_A, FACULTY_B, FACULTY_C};


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
    Student() = delete;
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
};

/**
 * @brief The FacultyA class
 */
class FacultyA: virtual public StudentBase
{

};

}
#endif // STUDENT_H
