#include "StudentMain.h"
#include "LoginDialog.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if QT_VERSION < 0x050000
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));//Qt5默认使用UTF8，不需要
#endif

    LoginDialog login;
    login.show();
    
    return a.exec();
}
