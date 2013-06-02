#include "StudentMain.h"
#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "Student.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    mainWindow(0)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;

    if(mainWindow)
        delete mainWindow;
}

void LoginDialog::on_LoginButtonBox_accepted()
{
    if(ui->AdmissionsOffice->isChecked())
        this->mainWindow = new StudentMain(Student::ADMISSIONS_OFFICE);
    else if(ui->DegreesOffice->isChecked())
        this->mainWindow = new StudentMain(Student::DEGREES_OFFICE);
    else if(ui->FacultyA->isChecked())
        this->mainWindow = new StudentMain(Student::FACULTY_A);
    else if(ui->FacultyB->isChecked())
        this->mainWindow = new StudentMain(Student::FACULTY_B);
    else if(ui->FacultyC->isChecked())
        this->mainWindow = new StudentMain(Student::FACULTY_C);

    this->hide();
    this->mainWindow->show();
}
