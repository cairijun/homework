#include<string>
#include<sstream>
#include<iomanip>

#include "Menu.h"

//Implementation of Menu class
string Menu::get_csv_line() const
{
    stringstream buf;
    buf << fixed << setprecision(2);
    buf << id << ","
        "\"" << name << "\","
        << price << ","
        "\"" << description << "\"";
        buf.flush();
    return buf.str();
}

string Menu::get_pretty_printed_line() const
{
    stringstream buf;
    buf << fixed << setprecision(2);
    buf << left << setw(6) << id
        << left << setw(20) << name
        << left << setw(8) << price
        << description;
    buf.flush();
    return buf.str();
}

int Menu::last_id = 0;


//Implementation of Order class
string Order::get_csv_line() const
{
    stringstream buf;
    buf << get_id()<< ','
        << quantity;
    buf.flush();
    return buf.str();
}

string Order::get_pretty_printed_line() const
{
    stringstream buf;
    buf << left << setw(6) << quantity
        << Menu::get_pretty_printed_line();
    buf.flush();
    return buf.str();
}

