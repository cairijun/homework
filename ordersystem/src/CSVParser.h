#ifndef CSVPARSER_H
#define CSVPARSER_H

#include<string>
using namespace std;

//A class used to parse a *line* of string in CSV format.
class CSVParser
{
    private:
        string line;

    public:
        explicit CSVParser(string s):line(s) { }

        //Get a number from the first remaining column.
        template<typename T> CSVParser& get_num(T &);
        //Get a string from the first remaining column.
        CSVParser& get_str(string &);
};

template<typename T> CSVParser& CSVParser::get_num(T &target)
{
    size_t comma_pos = line.find(',');
    stringstream buf(line.substr(0, comma_pos));
    buf >> target;

    if(comma_pos + 1 < line.size())
        line = line.substr(comma_pos + 1);
    else
        line = "";
    return *this;
}

#endif

