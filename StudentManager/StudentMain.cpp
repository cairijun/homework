#include "ui_StudentMain.h"
#include "StudentMain.h"

#include<cassert>
#include<QStringList>
#include<QMessageBox>
#include<QInputDialog>
#include<map>

StudentMain::StudentMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StudentMain)
{
    ui->setupUi(this);
}

StudentMain::~StudentMain()
{
    delete ui;

    _CHK_DEL(_controller);
}

StudentMain::StudentMain(Student::Role role, QWidget *parent)
    :StudentMain(parent)
{
    switch(role)
    {
    case Student::ADMISSIONS_OFFICE:
        _controller = new Student::AdmissionsOfficeController(ui);
        break;
    case Student::DEGREES_OFFICE:
        _controller = new Student::DegreesOfficeController(ui);
        break;
    case Student::FACULTY_A:
        _controller = new Student::FacultyController(ui, Student::FACULTY_NAME_A);
        break;
    case Student::FACULTY_B:
        _controller = new Student::FacultyController(ui, Student::FACULTY_NAME_B);
        break;
    case Student::FACULTY_C:
        _controller = new Student::FacultyController(ui, Student::FACULTY_NAME_C);
        break;
    }

    assert(_controller);

    _controller->loadStudentList();
    ui->StudentList->selectRow(0);
}

void StudentMain::on_DegreeStat_clicked()
{
    statDialog = new StatisticsDialog;
    statDialog->show();
    statDialog->exec();
    delete statDialog;
    statDialog = 0;
}

void StudentMain::on_StudentList_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if(currentRow != previousRow)
    {
        QTableWidgetItem *item = ui->StudentList->item(currentRow, 0);
        if(item)
        {
            (*_controller)["adding"] = "0";//取消控制器的添加状态，防止控制器把保存当成添加
            long ID = item->text().toLong();
            _controller->aStudentSelected(ID);
        }
    }
}

void StudentMain::on_SaveData_clicked()
{
    _controller->saveAStudent();
    (*_controller)["adding"] = "0";//取消控制器的添加状态，防止控制器把保存当成添加
}

void StudentMain::on_ResetData_clicked()
{
    QTableWidgetItem *item = ui->StudentList->item(ui->StudentList->currentRow(), 0);
    if(item)//只有在选择了已存在的学生才重置
        _controller->aStudentSelected(item->text().toLong());
}

void StudentMain::on_AddStudent_clicked()
{
    bool isOk = false;

    long id = QInputDialog::getInt(this, "添加学生", "请输入学号：", 0, 0, 99999999, 1, &isOk);
    if(!isOk)
        return;
    if(_controller->checkIDExists(id))
    {
        QMessageBox::critical(this, "错误", "学号已存在！");
        return;
    }

    QStringList facultyList = {"专业A", "专业B", "专业C"};
    QString major = QInputDialog::getItem(
                this, "添加学生", "请选择主修专业：", facultyList, 0, false, &isOk);
    if(!isOk)
        return;

    facultyList.removeOne(major);
    facultyList.append("(无)");
    QString minor = QInputDialog::getItem(
                this, "添加学生", "请选择辅修专业：", facultyList, 0, false, &isOk);
    if(!isOk)
        return;

    ui->NameBox->setText("");
    ui->IDBox->setText(QString::number(id));
    ui->AddressBox->setText("");
    ui->StudentList->setCurrentCell(-1, 0);//取消学生列表的选择

    //1. 用来把用于显示的专业名称转换成用于参数传递的名称
    //2. 由于Qt一个迟迟不肯修复的BUG（QTBUG-25679），QMap无法使用初始化列表，
    //但为了保持简洁高效，这里使用支持初始化列表的std::map代替
    std::map<QString, QString> facultyNameDict = {
        {"专业A", "Faculty A"},
        {"专业B", "Faculty B"},
        {"专业C", "Faculty C"},
        {"(无)", "Faculty NONE"}
    };

    //设置控制器状态为添加学生，并传入相关参数
    (*_controller)["adding"] = "1";
    (*_controller)["major"] = facultyNameDict[major];
    (*_controller)["minor"] = facultyNameDict[minor];
}
