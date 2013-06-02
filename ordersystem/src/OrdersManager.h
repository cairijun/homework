#ifndef ORDERSMANAGER_H
#define ORDERSMANAGER_H

#include<string>
#include<map>
#include<iostream>

#include "Menu.h"
#include "MenuClass.h"
#include "CSVParser.h"

//The OrderClass is the *read* order, while Order class is the items.
class OrderClass
{
    //friend class OrdersManager;

    private:
        map<int, Order> item_list;
        string name, address, contact;//informations of the customer
        int id;
        bool confirmed;

        //OrderClass(const OrderClass &);//forbid copying (to ensure the uniqueness of ID)

        static int last_id;//upper bound of IDs

    public:
        //id = -1 means that the order hasn't been saved.
        OrderClass():id(-1), confirmed(false) {};

        void set_id(int id)
        {
            this->id = id;
            if(id > last_id)
                last_id = id;
        }
        void set_confirmed(bool s) { this->confirmed = s; }
        void set_customer(string name, string address, string contact)
        {
            this->name = name;
            this->address = address;
            this->contact = contact;
        }

        void add_item(const Order &);
        void delete_item(int);
        //ID is set when saved.
        int save() { return id = ++last_id; }
        void confirm() { confirmed = true; }

        int get_id() const { return id; }
        string get_name() const { return name; }
        bool get_confirmed() const { return confirmed; }
        int get_count() const { return item_list.size(); }
        double get_total_price() const;
        void print_to_csv(ostream &) const;
        void pretty_print(ostream &) const;
        void pretty_print_short(ostream &) const;
};

class OrdersManager
{
    private:
        map<int, OrderClass *> order_list;
        multimap<string, OrderClass *> index_by_name;
        MenuClass *menu;//The OrdersManager should know the menu.

        //function template to print a query result
        //the definition is *not* in OrdersManager.cpp due to the
        //lack of support of `export` keyword in g++
        template<typename I> void print_query(I, I) const;

        bool load_list();
        bool save_list() const;
        OrdersManager(const OrdersManager&);//forbid copying

    public:
        OrdersManager(MenuClass *);
        ~OrdersManager();

        bool add_order(const OrderClass &);
        bool delete_order(int);
        OrderClass* get_order(int);
        const OrderClass* get_order(int) const;
        void print_order_list() const;
        void query_by_id(int) const;
        void query_by_name(const string &) const;
};

const char * const ORDERS_FILE_NAME = "order.txt";

template<typename I> void OrdersManager::print_query(I i, I j) const
{
    int c = 0;//the number of result
    while(i != j)
    {
        ++c;
        (i++)->second->pretty_print(cout);
    }
    cout << string(56, '-');
    cout << "\n共 " << c << " 条结果。" << endl;
}

#endif

