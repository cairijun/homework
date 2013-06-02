#include<string>
#include<sstream>
#include "CSVParser.h"
using namespace std;

CSVParser& CSVParser::get_str(string &target)
{
    size_t double_quote_pos_1 = line.find('"');
    size_t double_quote_pos_2 = line.find('"', double_quote_pos_1 + 1);
    if(double_quote_pos_1 < line.size())
        //found
        target = line.substr(double_quote_pos_1 + 1,
                double_quote_pos_2 - double_quote_pos_1 - 1);
    else
        //not found
        target = "";

    size_t comma_pos = line.find(',', double_quote_pos_2 + 1);
    if(comma_pos + 1 < line.size())
        line = line.substr(comma_pos + 1);
    else
        line = "";
    return *this;
}

