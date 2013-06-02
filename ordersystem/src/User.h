#ifndef USER_H
#define USER_H

#include "OrdersManager.h"
#include "MenuClass.h"

class User
{
    private:
        User(const User &);//forbid copying

    protected:
        MenuClass *menu;
        OrdersManager *om;
        bool ready;//whether the object is initialized properly

        void new_screen();
        void print_a_row(const string &);

    public:
        User();
        virtual ~User();

        virtual void main_menu() = 0;
        virtual bool modifyOrder(int) = 0;
};

class Seller: public User
{
    private:
        void manage_orders();
        void order_detail(int);

        void manage_menu();
        void add_menu_item();

    public:
        void main_menu();
        bool modifyOrder(int);
};

class Buyer: public User
{
    private:
        void query_order();
        void add_order();
        void save_order(OrderClass &);
        void current_order(OrderClass &);

    public:
        void main_menu();
        bool modifyOrder(int);
};

#endif

