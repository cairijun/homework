#ifndef MENU_H
#define MENU_H

#include<string>

using namespace std;

class Menu
{
    private:
        static int last_id;//The upper bound of IDs. Used to generate new IDs.

    protected:
        string name, description;
        double price;
        int id;

    public:
        Menu() {};
        //Create a new item.
        Menu(string name, double price, string description)
            :name(name), description(description), price(price), id(++last_id)
        {}

        //Initial from an existing item.
        Menu(string name, double price, string description, int new_id)
            :name(name), description(description), price(price), id(new_id)
        {
            if(new_id > last_id)
                last_id = new_id;
        }

        int get_id() const { return id; };
        string get_name() const { return name; }
        virtual string get_csv_line() const;
        virtual string get_pretty_printed_line() const;

        virtual ~Menu() {};
};


//The definition of Order class. The information of the
//customer is stored in the instance of OrderClass class
//instead of Order class.
class Order: public Menu
{
    private:
        int quantity;

    public:
        Order() {}
        //Generate a new Order item from a given Menu object and quantity.
        Order(const Menu &m, int n)
            :Menu(m), quantity(n) {}

        double get_price() const { return price; }
        int get_quantity() const { return quantity; }
        void set_quantity(int n) { quantity = n; }

        string get_csv_line() const;
        string get_pretty_printed_line() const;
};


#endif

