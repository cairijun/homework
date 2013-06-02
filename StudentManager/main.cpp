#include "StudentMain.h"
#include "LoginDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //StudentMain w;
    //w.show();

    LoginDialog login;
    login.show();
    
    return a.exec();
}
