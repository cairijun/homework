#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<map>
#include<iomanip>
#include<limits>

using namespace std;

#include "OrdersManager.h"
#include "CSVParser.h"

//Implementation of OrderClass
int OrderClass::last_id = 0;

void OrderClass::add_item(const Order &item)
{
    if(item_list.count(item.get_id()))
        //Item exists, just set the quantity.
        item_list[item.get_id()].set_quantity(item.get_quantity());
    else
        item_list[item.get_id()] = item;
}

void OrderClass::delete_item(int id)
{
    item_list.erase(id);
}

void OrderClass::print_to_csv(ostream &os) const
{
    if(id < 0)
    {
        cerr << "订单未保存，无法序列化。";
        return;
    }
    //customer's informations and order status (the first column is -1)
    os << "-1,"
        << id << ","
        "\"" << name << "\","
        "\"" << address << "\","
        "\"" << contact << "\","
        << confirmed << endl;
    
    //items list
    for(map<int, Order>::const_iterator i = item_list.begin();
            i != item_list.end(); ++i)
        os << id << ',' << i->second.get_csv_line() << '\n';
    os.flush();
}

void OrderClass::pretty_print(ostream &os) const
{
    //table title
    cout << string(56, '-') << "\n"
        "|" << string(24, ' ') << "订  单"
        << string(24, ' ')  << "|\n";
    //columns names
    cout << string(56, '-') << '\n'
        <<"|数量  编号  名称                价格    介绍          |" << endl;
    //items
    for(map<int, Order>::const_iterator i = item_list.begin();
            i != item_list.end(); ++i)
        cout << "|" << left << setw(54)
            << i->second.get_pretty_printed_line() << "|\n";
    //bottom line
    cout << string(56, '-') << endl;
    cout << "共计：" << item_list.size() << "种，总价：" << get_total_price() << endl;

    //Order informations
    if(id >= 0)//Print the extra informations only if the order has been saved.
    {
        cout << "订单编号：" << left << setw(6) << id
            << "订单状态：" << (confirmed ? "已确认\n": "未确认\n");
        cout << "客户姓名：" << name << '\n'
            << "收货地址：" << address << '\n'
            << "联系方式：" << contact << endl;
    }
}

void OrderClass::pretty_print_short(ostream &os) const
{
    if(id >= 0)
    {
        cout << "| " << left << setw(6) << id
            << left << setw(10) << name
            << left << setw(12) << fixed << setprecision(2) << get_total_price()
            << (confirmed ? "已确认": "未确认")
            << "                   |" << endl;
    }
}

double OrderClass::get_total_price() const
{
    double sum = 0.0;
    for(map<int, Order>::const_iterator i = item_list.begin();
            i != item_list.end(); ++i)
        sum += i->second.get_price() * i->second.get_quantity();
    return sum;
}


//Implementation of OrdersManager
OrdersManager::OrdersManager(MenuClass *menu)
    :menu(menu)
{
    if(!load_list())
        cerr << "加载订单数据失败！" << endl;
}

OrdersManager::~OrdersManager()
{
    save_list();
    for(map<int, OrderClass *>::const_iterator i = order_list.begin();
            i != order_list.end(); ++i)
        delete i->second;
}

bool OrdersManager::load_list()
{
    ifstream ifs(ORDERS_FILE_NAME);
    if(!ifs.good())
        return false;

    string line;
    int order_id, item_id, quantity;
    bool confirmed;
    string name, address, contact;
    OrderClass *order;
    while(getline(ifs, line))
    {
        CSVParser parser(line);
        parser.get_num(order_id);
        if(order_id < 0)
        {
            //This line is the extra informations of the order.
            parser.
                get_num(order_id).
                get_str(name).
                get_str(address).
                get_str(contact).
                get_num(confirmed);

            if(!order_list.count(order_id))
            {
                order = new OrderClass();
                order_list[order_id] = order;
            }
            else
                order = order_list[order_id];
            order->set_id(order_id);
            order->set_customer(name, address, contact);
            order->set_confirmed(confirmed);

            //update index
            index_by_name.insert(pair<string, OrderClass *>(name, order));
        }
        else
        {
            //This line is an item of an order.
            parser.get_num(item_id).get_num(quantity);
            Order an_item(menu->get_item(item_id), quantity);

            if(!order_list.count(order_id))
                order_list[order_id] = order = new OrderClass();
            else
                order = order_list[order_id];
            order->add_item(an_item);
        }
        //ifs.ignore(numeric_limits<streamsize>::max(), '\n');
        line = "";
    }
    ifs.close();
    return true;
}

bool OrdersManager::save_list() const
{
    ofstream ofs(ORDERS_FILE_NAME);
    if(!ofs.good())
        return false;
    for(map<int, OrderClass *>::const_iterator i = order_list.begin();
            i != order_list.end(); ++i)
        i->second->print_to_csv(ofs);
    bool stream_status = ofs.good();
    ofs.close();
    return stream_status;
}

bool OrdersManager::add_order(const OrderClass &order)
{
    if(order.get_id() < 0)
    {
        cerr << "订单未保存，无法添加！" << endl;
        return false;
    }
    if(order_list.count(order.get_id()))
    {
        cerr << "相同编号订单已存在，无法添加！" << endl;
        return false;
    }
    OrderClass *p = new OrderClass(order);
    order_list[order.get_id()] = p;
    index_by_name.insert(pair<string, OrderClass *>(order.get_name(), p));
    return save_list();
}

bool OrdersManager::delete_order(int id)
{
    map<int, OrderClass *>::iterator iter = order_list.find(id);
    if(iter == order_list.end())
    {
        cout << ">该订单不存在！" << endl;
        return false;
    }
    else if(iter->second->get_confirmed())
    {
        cout << ">该订单已确认，无法删除！" << endl;
        return false;
    }
    //clear index
    pair<map<string, OrderClass *>::iterator, map<string, OrderClass *>::iterator>
        index_iter = index_by_name.equal_range(iter->second->get_name());
    for(map<string, OrderClass *>::iterator i = index_iter.first;
            i != index_iter.second; ++i)
        if(i->second->get_id() == id)
        {
            index_by_name.erase(i);
            break;
        }

    //delete it from order list
    delete iter->second;
    order_list.erase(iter);
    return save_list();
}

OrderClass* OrdersManager::get_order(int id)
{
    map<int, OrderClass *>::iterator iter = order_list.find(id);
    if(iter == order_list.end())
        return 0;
    else
        return iter->second;
}

const OrderClass* OrdersManager::get_order(int id) const
{
    map<int, OrderClass *>::const_iterator iter = order_list.find(id);
    if(iter == order_list.end())
        return 0;
    else
        return iter->second;
}

void OrdersManager::query_by_id(int id) const
{
    pair<map<int, OrderClass *>::const_iterator, map<int, OrderClass *>::const_iterator>
        range = order_list.equal_range(id);
    cout << "按订单号查询，关键词：" << id << endl;
    print_query(range.first, range.second);
}

void OrdersManager::query_by_name(const string &name) const
{
    pair<map<string, OrderClass *>::const_iterator, map<string, OrderClass *>::const_iterator>
        range = index_by_name.equal_range(name);
    cout << "按客户名字查询，关键词：" << name << endl;
    print_query(range.first, range.second);
}

void OrdersManager::print_order_list() const
{
    cout << "| 编号  客户      总价        状态                     |" << endl;
    for(map<int, OrderClass *>::const_iterator i = order_list.begin();
            i != order_list.end(); ++i)
        i->second->pretty_print_short(cout);
}
