#include "ui_StudentMain.h"
#include "StudentMain.h"

#include<cassert>
#include<QStringList>
#include<QStringBuilder>
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
    if(_controller->saveAStudent())
        QMessageBox::information(this, "保存信息", "保存成功！");
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

void StudentMain::on_AddMajorScore_clicked()
{
    bool isOk = false;
    QString name = QInputDialog::getText(this, "添加科目成绩", "请输入（主修）科目名称：", QLineEdit::Normal, "", &isOk);
    if(!isOk || !name.size())
        return;

    QStringList buf;
    buf << "请输入学生 " << ui->NameBox->text() << " （学号" << ui->IDBox->text() << "）的\n" << name << " （主修）成绩：";
    int score = QInputDialog::getInt(this, "添加科目成绩", buf.join(""), 80, 0, 100, 1, &isOk);
    if(!isOk)
        return;

    int rowCount = ui->MajorScoreList->rowCount();
    ui->MajorScoreList->setRowCount(rowCount + 1);
    ui->MajorScoreList->setItem(rowCount, 0, new QTableWidgetItem(name));
    ui->MajorScoreList->setItem(rowCount, 1, new QTableWidgetItem(QString::number(score)));
    ui->MajorScoreList->setItem(rowCount, 2, new QTableWidgetItem(score >= 60 ? "过" : "挂"));
}

void StudentMain::on_DeleteMajorScore_clicked()
{
    int row = ui->MajorScoreList->currentRow();
    auto item = ui->MajorScoreList->item(row, 0);
    auto button = QMessageBox::warning(this, "删除成绩", "确定要删除" + item->text() + "的成绩吗？",
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(button == QMessageBox::Yes)
        ui->MajorScoreList->removeRow(row);
}

void StudentMain::on_AddMinorScore_clicked()
{
    bool isOk = false;
    QString name = QInputDialog::getText(this, "添加科目成绩", "请输入（辅修）科目名称：", QLineEdit::Normal, "", &isOk);
    if(!isOk || !name.size())
        return;

    QStringList buf;
    buf << "请输入学生 " << ui->NameBox->text() << " （学号" << ui->IDBox->text() << "）的\n" << name << " （辅修）成绩：";
    int score = QInputDialog::getInt(this, "添加科目成绩", buf.join(""), 80, 0, 100, 1, &isOk);
    if(!isOk)
        return;

    int rowCount = ui->MinorScoreList->rowCount();
    ui->MinorScoreList->setRowCount(rowCount + 1);
    ui->MinorScoreList->setItem(rowCount, 0, new QTableWidgetItem(name));
    ui->MinorScoreList->setItem(rowCount, 1, new QTableWidgetItem(QString::number(score)));
    ui->MinorScoreList->setItem(rowCount, 2, new QTableWidgetItem(score >= 60 ? "过" : "挂"));
}

void StudentMain::on_DeleteMinorScore_clicked()
{
    int row = ui->MinorScoreList->currentRow();
    auto item = ui->MinorScoreList->item(row, 0);
    auto button = QMessageBox::warning(this, "删除成绩", "确定要删除" + item->text() + "的成绩吗？",
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(button == QMessageBox::Yes)
        ui->MinorScoreList->removeRow(row);
}
