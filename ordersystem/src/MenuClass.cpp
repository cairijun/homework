#include<string>
#include<map>
#include<fstream>
#include<sstream>
#include<limits>
#include<iostream>
#include<iomanip>
using namespace std;

#include "MenuClass.h"
#include "CSVParser.h"
#include "Menu.h"

bool MenuClass::save_list() const
{
    ofstream ofs(MENU_FILE_NAME);
    if(!ofs)
        return false;

    for(map<int, Menu>::const_iterator i = item_list.begin();
            i != item_list.end(); ++i)
        ofs << i->second.get_csv_line() << '\n';
    
    bool io_status = ofs.good();
    ofs.close();
    return io_status;
}

bool MenuClass::load_list()
{
    ifstream ifs(MENU_FILE_NAME);
    if(!ifs)
        return false;

    string a_line;

    int id;
    string name, description;
    double price;
    while(getline(ifs, a_line))
    {
        CSVParser parser(a_line);
        parser.
            get_num(id).
            get_str(name).
            get_num(price).
            get_str(description);

        item_list[id] = Menu(name, price, description, id);

        a_line = "";
    }
    ifs.close();

    return true;
}

MenuClass::MenuClass()
{
    ready = load_list();
    if(!ready)
        cerr << "加载菜单失败！" << endl;
}

MenuClass::~MenuClass()
{
    save_list();
    item_list.clear();
}

int MenuClass::add_item(string name, double price, string description)
{
    Menu new_item(name, price, description);
    item_list[new_item.get_id()] = new_item;
    save_list();
    return new_item.get_id();
}

bool MenuClass::delete_item(int id)
{
    return item_list.erase(id) && save_list();
}

void MenuClass::print_menu() const
{
    //table title
    cout << string(56, '-') << "\n"
        "|" << string(24, ' ') << "菜  单"
        << string(24, ' ')  << "|\n";
    //columns names
    cout << string(56, '-') << '\n'
        <<"|编号  名称                价格    介绍                |" << endl;
    //items
    for(map<int, Menu>::const_iterator i = item_list.begin();
            i != item_list.end(); ++i)
        cout << "|" << left << setw(54)
            << i->second.get_pretty_printed_line() << "|\n";
    //bottom line
    cout << string(56, '-') << endl;
}

Menu MenuClass::get_item(int id) const
{
    map<int, Menu>::const_iterator iter = item_list.find(id);
    if(iter != item_list.end())
        return iter->second;
    else
        return Menu("（无）", 0.0, "该菜式不存在或已下架。", id);
}

