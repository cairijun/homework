#include<iostream>
#include<string>

#include "User.h"

using namespace std;

int main()
{
    char type = 0;
    User *the_user = 0;

    while(type != 'B' && type != 'S')
    {
        cout << "请选择您的身份（B顾客/S卖家）：";
        cin >> type;
    }

    if(type == 'B')
        the_user = new Buyer();
    else
        the_user = new Seller();

    the_user->main_menu();//main loop

    delete the_user;
    return 0;
}

