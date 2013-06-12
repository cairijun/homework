#include "StudentMain.h"
#include "LoginDialog.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //StudentMain w;
    //w.show();

#if QT_VERSION < 0x050000
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
    LoginDialog login;
    login.show();
    
    return a.exec();
}
