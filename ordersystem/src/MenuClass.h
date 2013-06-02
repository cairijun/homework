#ifndef MENUCLASS_H
#define MENUCLASS_H

#include<string>
#include<map>
using namespace std;

#include "Menu.h"

//MenuClass is the *real* menu, while Menu class is the item of MenuClass.
//This class provides some methods to deal with the *real* menu.
//The user should operate on MenuClass rather than Menu class.
class MenuClass
{
    private:
        map<int, Menu> item_list;
        bool ready;//Whether the object is ready for providing data.

        bool save_list() const;
        bool load_list();

        MenuClass(const MenuClass &);//forbid copying

    public:
        MenuClass();
        ~MenuClass();

        int add_item(string, double, string description = "暂无介绍");
        bool delete_item(int);
        void print_menu() const;
        Menu get_item(int) const;
        bool check_item(int id) const { return item_list.count(id); }
};

const char * const MENU_FILE_NAME = "menu.txt";

#endif

