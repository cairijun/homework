#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "StudentMain.h"
#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();
    
private slots:

    void on_LoginButtonBox_accepted();

private:
    Ui::LoginDialog *ui;
    StudentMain *mainWindow;
};

#endif // LOGINDIALOG_H
