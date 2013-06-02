#include<iostream>
#include<limits>
#include<cctype>
#include<cstdlib>
#include<string>
#include<sstream>
#include<iomanip>
using namespace std;

#include "User.h"
#include "MenuClass.h"
#include "OrdersManager.h"

static void get_not_empty_line(string &target)
{
    target = "";
    if(!cin)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    while(target == "" || target == "\n")
    {
        target = "";
        getline(cin, target);
    }
}

//Implementation of User class
User::User()
{
    menu = new MenuClass();
    om = new OrdersManager(menu);
}

User::~User()
{
    delete menu;
    delete om;
}

void User::new_screen()
{
    string line(56, '-');
    string empty_row(54, ' ');
    empty_row = '|' + empty_row + '|';

    cout << string(100, '\n') << endl;
    cout << line << '\n' << empty_row << "\n"
        "|" << string(20, ' ') << "订  餐  系  统"
        << string(20, ' ') << "|\n"
        << empty_row << '\n' << line << endl;
}

void User::print_a_row(const string &content)
{
    size_t content_len = content.size();
    if(content_len > 52)
        cout << content << endl;
    else
    {
        cout << "| " << left << setw(53) << content << "|";
        cout << endl;
    }
}

//Implementation of Seller class
void Seller::main_menu()
{
    string cmd;
    do
    {
        new_screen();
        print_a_row("卖家页面");
        print_a_row("请选择功能：");
        print_a_row("m ) 菜单管理");
        print_a_row("o ) 订单管理");
        print_a_row("q ) 退出");
        cout << string(56, '-') << "\n"
            "请输入相应的字母，按回车确认：";
        cout.flush();
        get_not_empty_line(cmd);
        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cmd[0] = tolower(cmd[0]);
        if(cmd == "m")
            manage_menu();
        else if(cmd == "o")
            manage_orders();
    }while(cmd != "q");
}

bool Seller::modifyOrder(int id)
{
    OrderClass *the_order = om->get_order(id);
    if(the_order)
    {
        the_order->confirm();
        cout << "确认订单 " << id << " 成功！" << endl;
        return true;
    }
    else
    {
        cout << ">该订单不存在！" << endl;
        return false;
    }
}

void Seller::manage_orders()
{
    string cmd;
    do
    {
        new_screen();
        print_a_row("卖家页面");
        print_a_row("╚═订单管理");
        cout << string(56, '-') << endl;
        om->print_order_list();
        cout << string(56, '-') << endl;
        print_a_row("输入订单编号并回车查看订单详情，输入q并回车返回上级菜单");
        cout << "命令：";
        cout.flush();
        get_not_empty_line(cmd);
        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if(isalpha(cmd[0]))
            cmd[0] = tolower(cmd[0]);
        else
        {
            int id = atoi(cmd.c_str());
            order_detail(id);
        }
    }while(cmd != "q");
}

void Seller::order_detail(int id)
{
    OrderClass *the_order = om->get_order(id);
    if(!the_order)
    {
        cout << ">无效订单号！" << endl;
        return;
    }
    string cmd;
    do
    {
        new_screen();
        print_a_row("卖家页面");
        print_a_row("╚═订单管理");
        print_a_row("  ╚═订单详情");
        the_order->pretty_print(cout);
        cout << string(56, '-') << endl;
        print_a_row("输入e并按回车确认订单，输入q并回车返回上级菜单");
        cout << "命令：";
        cout.flush();
        get_not_empty_line(cmd);
        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cmd[0] = tolower(cmd[0]);
        if(cmd == "e")
            the_order->confirm();
    }while(cmd != "q" && cmd != "e");
}

void Seller::manage_menu()
{
    string cmd;
    do
    {
        new_screen();
        print_a_row("卖家页面");
        print_a_row("╚═菜单管理");
        menu->print_menu();
        print_a_row("输入菜式编号并回车删除菜式，输入a并回车添加菜式");
        print_a_row("输入q并回车返回上级菜单");
        cout << "命令：";
        cout.flush();
        get_not_empty_line(cmd);
        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if(isalpha(cmd[0]))
            cmd[0] = tolower(cmd[0]);
        else
        {
            int id = atoi(cmd.c_str());
            Menu menu_item = menu->get_item(id);
            print_a_row("确定要删除 " + menu_item.get_name() + " 吗？");
            cout << "输入Y确认删除，输入n取消：";
            cout.flush();
            string confirm;
            while(confirm != "Y" && confirm != "n" && confirm != "N")
            {
                confirm = "";
                getline(cin, confirm);
            }
            if(confirm == "Y")
                menu->delete_item(id);
            continue;
        }
        if(cmd == "a")
            add_menu_item();
    }while(cmd != "q");
}

void Seller::add_menu_item()
{
    string name, description;
    double price = 0;

    new_screen();

    print_a_row("卖家页面");
    print_a_row("╚═菜单管理");
    print_a_row("  ╚═添加菜式");
    cout << string(56, '-') << endl;
    cout << "请输入菜式名称：";
    get_not_empty_line(name);
    cout << "请输入价格：";
    while(price <= 0 && !cin.eof())
        if(!cin.fail())
            cin >> price;
        else
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    cout << "请输入菜式介绍：";
    get_not_empty_line(description);

    menu->add_item(name, price, description);
}

//Implementation of Buyer class
void Buyer::main_menu()
{
    string cmd;
    do
    {
        new_screen();
        print_a_row("客户页面");
        print_a_row("请选择功能：");
        print_a_row("s ) 订单查询");
        print_a_row("n ) 新建订单");
        print_a_row("q ) 退出");
        cout << string(56, '-') << "\n"
            "请输入相应的字母，按回车确认：";
        cout.flush();
        get_not_empty_line(cmd);
        cmd[0] = tolower(cmd[0]);
        if(cmd == "s")
            query_order();
        else if(cmd == "n")
            add_order();
    }while(cmd != "q");
}

bool Buyer::modifyOrder(int id)
{
    return om->delete_order(id);
}

void Buyer::query_order()
{
    string keyword;
    new_screen();
    print_a_row("客户页面");
    print_a_row("╚═订单查询");
    print_a_row("输入`#订单号`或客户姓名进行查询，输入q返回上级菜单");
    cout << "命令：";
    get_not_empty_line(keyword);
    if(keyword == "q")
        return;
    if(keyword[0] == '#')
        om->query_by_id(atoi(keyword.substr(1).c_str()));
    else
        om->query_by_name(keyword);

    string cmd;
    bool error_occured = false;
    do
    {
        if(!error_occured)
        {
            new_screen();
            print_a_row("客户页面");
            print_a_row("╚═订单查询");
            if(keyword[0] == '#')
                om->query_by_id(atoi(keyword.substr(1).c_str()));
            else
                om->query_by_name(keyword);
        }

        cout << "输入订单号并回车删除订单，输入q返回上级菜单\n";
        cout << "命令：";
        get_not_empty_line(cmd);
        cmd[0] = tolower(cmd[0]);
        if(cmd != "q")
        {
            int id = atoi(cmd.c_str());
            if(id)
            {
                cout << "确实要取消订单 " << id << " 吗？\n"
                    "输入Y确定，输入n取消：";
                string confirm;
                while(confirm != "Y" && confirm != "n" && confirm != "N")
                {
                    confirm = "";
                    getline(cin, confirm);
                }
                if(confirm == "Y")
                    error_occured = !modifyOrder(id);
            }
        }
    }while(cmd != "q");
}

void Buyer::add_order()
{
    string cmd;
    OrderClass the_order;
    do{
        new_screen();
        print_a_row("客户页面");
        print_a_row("╚═新建订单");

        menu->print_menu();

        stringstream buf;
        buf << "已选菜式：" << the_order.get_count()
            << " 种，总价：" << the_order.get_total_price();
        print_a_row(buf.str());

        print_a_row("输入菜式编号选择菜式，输入c查看当前已选择菜式");
        print_a_row("输入s提交订单，输入q返回上级菜单");
        cout << "请输入需要的菜式编号：";
        get_not_empty_line(cmd);
        cmd[0] = tolower(cmd[0]);
        if(cmd != "q" && cmd != "s" && cmd != "c")
        {
            int id = atoi(cmd.c_str());
            if(menu->check_item(id))
            {
                int quantity = 0;
                cout << "请输入需要的数量：";
                while(quantity <= 0 && !cin.eof())
                    if(!cin.fail())
                        cin >> quantity;
                    else
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                Order the_item(menu->get_item(id), quantity);
                the_order.add_item(the_item);
            }
        }
        if(cmd == "c")
            current_order(the_order);
    }while(cmd != "q" && cmd != "s");
    if(cmd == "s")
        save_order(the_order);
}

void Buyer::save_order(OrderClass &order)
{
    string name, address, contact;
    new_screen();
    print_a_row("客户页面");
    print_a_row("╚═新建订单");
    print_a_row("  ╚═提交订单");
    order.pretty_print(cout);

    cout << "请输入您的姓名：";
    get_not_empty_line(name);
    cout << "请输入您的地址：";
    get_not_empty_line(address);
    cout << "请输入您的联系方式：";
    get_not_empty_line(contact);

    order.set_customer(name, address, contact);
    int id = order.save();
    om->add_order(order);
    cout << "订单提交成功！请记住您的订单号：" << id << '\n';
    cout << "按回车键返回";
    cout.flush();
    getline(cin, name);
}

void Buyer::current_order(OrderClass &order)
{
    string cmd;
    do
    {
        new_screen();
        print_a_row("客户页面");
        print_a_row("╚═新建订单");
        print_a_row("  ╚═查看订单");
        order.pretty_print(cout);
        print_a_row("输入编号删除菜式，输入q返回上级菜单");
        get_not_empty_line(cmd);
        cmd[0] = tolower(cmd[0]);
        if(cmd != "q")
            order.delete_item(atoi(cmd.c_str()));
    }while(cmd != "q");
}

