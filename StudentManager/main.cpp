#include "StudentMain.h"
#include "LoginDialog.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //StudentMain w;
    //w.show();

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    LoginDialog login;
    login.show();
    
    return a.exec();
}
