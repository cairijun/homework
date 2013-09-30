#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include<sstream>
#include<vector>
#include<set>
#include<map>

#include<stdint.h>

typedef uint_least32_t mc_t;
typedef int_least32_t mc_t_s;

using std::vector;
using std::string;
using std::map;
using std::set;

class Assembler
{
    struct Instruction
    {
        Instruction() {}
        Instruction(
                const string &_label,
                const string &_instruction,
                const vector<string> _fields):
            label(_label), instruction(_instruction), fields(_fields) {}

        string label;
        string instruction;
        vector<string> fields;
    };

    private:
        vector<string> _as;
        vector<Instruction> _ins;
        map<string, size_t> _label_table;
        vector<mc_t> _mc;

        set<string> _INS;
        set<string> _R_INS;
        set<string> _I_INS;
        set<string> _J_INS;
        set<string> _O_INS;
        set<string> _PSEUDO_INS;

        inline int interpreOffsetField(const string &field, bool *pLabelSign = 0);

        inline mc_t translateAdd(const Instruction &ins);
        inline mc_t translateNand(const Instruction &ins);
        inline mc_t translateLw(const Instruction &ins);
        inline mc_t translateSw(const Instruction &ins);
        inline mc_t translateBeq(const Instruction &ins, size_t pc);
        inline mc_t translateJalr(const Instruction &ins);
        inline mc_t translateHalt(const Instruction &ins);
        inline mc_t translateNoop(const Instruction &ins);
        inline mc_t translateDotFill(const Instruction &ins);

    public:
        Assembler();
        void setAssembly(const vector<string> &as);
        void synAnalyse();
        void passOne();
        void passTwo();
        vector<mc_t> getMechineCode();
};

int Assembler::interpreOffsetField(const string &field, bool *pLabelSign)
{
    bool sign = false;
    mc_t reg = atoi(field.c_str());
    if(!reg && field != "0")
    {
        reg = _label_table[field];
        sign = true;
    }

    if(pLabelSign)
        *pLabelSign = sign;
    return reg;
}

mc_t Assembler::translateAdd(const Instruction &ins)
{
    mc_t code = 0x00000000;
    mc_t regA = atoi(ins.fields[0].c_str());
    mc_t regB = atoi(ins.fields[1].c_str());
    mc_t regC = atoi(ins.fields[2].c_str());

    code |= (regA << 19 & 0x00380000) | (regB << 16 & 0x00070000) | (regC & 0x00000007);
    return code;
}

mc_t Assembler::translateNand(const Instruction &ins)
{
    mc_t code = 0x00400000;
    mc_t regA = atoi(ins.fields[0].c_str());
    mc_t regB = atoi(ins.fields[1].c_str());
    mc_t regC = atoi(ins.fields[2].c_str());

    code |= (regA << 19 & 0x00380000) | (regB << 16 & 0x00070000) | (regC & 0x00000007);
    return code;
}

mc_t Assembler::translateLw(const Instruction &ins)
{
    mc_t code = 0x00800000;
    mc_t regA = atoi(ins.fields[0].c_str());
    mc_t regB = atoi(ins.fields[1].c_str());
    int offset = interpreOffsetField(ins.fields[2]);

    code |= (regA << 19 & 0x00380000) | (regB << 16 & 0x00070000) | (offset & 0x0000ffff);
    return code;
}

mc_t Assembler::translateSw(const Instruction &ins)
{
    mc_t code = 0x00C00000;
    mc_t regA = atoi(ins.fields[0].c_str());
    mc_t regB = atoi(ins.fields[1].c_str());
    int offset = interpreOffsetField(ins.fields[2]);

    code |= (regA << 19 & 0x00380000) | (regB << 16 & 0x00070000) | (offset & 0x0000ffff);
    return code;
}

mc_t Assembler::translateBeq(const Instruction &ins, size_t pc)
{
    mc_t code = 0x01000000;
    mc_t regA = atoi(ins.fields[0].c_str());
    mc_t regB = atoi(ins.fields[1].c_str());

    bool labelSign;
    int offset = interpreOffsetField(ins.fields[2], &labelSign);
    if(labelSign)
        offset = offset - pc - 1;

    code |= (regA << 19 & 0x00380000) | (regB << 16 & 0x00070000) | (offset & 0x0000ffff);
    return code;
}

mc_t Assembler::translateJalr(const Instruction &ins)
{
    mc_t code = 0x01400000;
    mc_t regA = atoi(ins.fields[0].c_str());
    mc_t regB = atoi(ins.fields[1].c_str());

    code |= (regA << 19 & 0x00380000) | (regB << 16 & 0x00070000);
    return code;
}

mc_t Assembler::translateHalt(const Instruction &ins)
{
    return 0x01800000;
}

mc_t Assembler::translateNoop(const Instruction &ins)
{
    return 0x01C00000;
}

mc_t Assembler::translateDotFill(const Instruction &ins)
{
    return interpreOffsetField(ins.fields[0]);
}

Assembler::Assembler()
{
    _R_INS.insert("add");
    _R_INS.insert("nand");

    _I_INS.insert("lw");
    _I_INS.insert("sw");
    _I_INS.insert("beq");

    _J_INS.insert("jalr");

    _O_INS.insert("halt");
    _O_INS.insert("noop");

    _PSEUDO_INS.insert(".fill");

    _INS.insert(_R_INS.begin(), _R_INS.end());
    _INS.insert(_I_INS.begin(), _I_INS.end());
    _INS.insert(_J_INS.begin(), _J_INS.end());
    _INS.insert(_O_INS.begin(), _O_INS.end());
    _INS.insert(_PSEUDO_INS.begin(), _PSEUDO_INS.end());
}

void Assembler::setAssembly(const vector<string> &as)
{
    this->_as = as;
}

void Assembler::synAnalyse()
{
    std::cerr << "Syntax Analyse...\n" << std::endl;
    _ins.empty();

    string tmp1, tmp2, tmp3;
    for(vector<string>::const_iterator i = _as.begin();
            i != _as.end(); ++i)
    {
        std::cerr << "Assembly: " << *i << std::endl;
        Instruction new_ins;
        std::stringstream buf(*i);

        buf >> tmp1;
        if(!_INS.count(tmp1))
        {
            new_ins.label = tmp1;
            buf >> new_ins.instruction;
            std::cerr << "Label: " << new_ins.label << std::endl;
        }
        else
            new_ins.instruction = tmp1;

        std::cerr << "Instruction: " << new_ins.instruction << std::endl;

        if(_R_INS.count(new_ins.instruction) ||
                _I_INS.count(new_ins.instruction))
        {
            buf >> tmp1 >> tmp2 >> tmp3;
            new_ins.fields.push_back(tmp1);
            new_ins.fields.push_back(tmp2);
            new_ins.fields.push_back(tmp3);
        }
        else if(_J_INS.count(new_ins.instruction))
        {
            buf >> tmp1 >> tmp2;
            new_ins.fields.push_back(tmp1);
            new_ins.fields.push_back(tmp2);
        }
        else if(_PSEUDO_INS.count(new_ins.instruction))
        {
            if(new_ins.instruction == ".fill")
            {
                buf >> tmp1;
                new_ins.fields.push_back(tmp1);
            }
        }
        else if(!_O_INS.count(new_ins.instruction))
        {
            //TO-DO: Raise exception for unrecognizable instructions.
        }

        for(vector<string>::const_iterator i = new_ins.fields.begin();
                i != new_ins.fields.end(); ++i)
            std::cerr << "Field: " << *i << std::endl;
        std::cerr << std::endl;

        _ins.push_back(new_ins);
    }
}

void Assembler::passOne()
{
    std::cerr << "Pass 1...\n" << std::endl;
    for(size_t i = 0; i < _ins.size(); ++i)
        if(_ins[i].label.size())
            if(_label_table.count(_ins[i].label))
            {
                //TO-DO: Raise exception for duplicated labels;
                std::cerr << "Duplicated Label: " << _ins[i].label << std::endl;
            }
            else
            {
                _label_table[_ins[i].label] = i;
                std::cerr << "Label " << _ins[i].label << " for address " << i << std::endl;
            }
    std::cerr << std::endl;
}

void Assembler::passTwo()
{
    std::cerr << "Pass 2...\n" << std::endl;
    _mc.empty();

    for(vector<Instruction>::const_iterator i = _ins.begin();
            i != _ins.end(); ++i)
    {
        std::cerr << "Instruction " << i->instruction << " on " << _mc.size() << std::endl;
        if(i->instruction == "add")
            _mc.push_back(translateAdd(*i));
        else if(i->instruction == "nand")
            _mc.push_back(translateNand(*i));
        else if(i->instruction == "lw")
            _mc.push_back(translateLw(*i));
        else if(i->instruction == "sw")
            _mc.push_back(translateSw(*i));
        else if(i->instruction == "beq")
            _mc.push_back(translateBeq(*i, _mc.size()));
        else if(i->instruction == "jalr")
            _mc.push_back(translateJalr(*i));
        else if(i->instruction == "halt")
            _mc.push_back(translateHalt(*i));
        else if(i->instruction == "noop")
            _mc.push_back(translateNoop(*i));
        else if(i->instruction == ".fill")
            _mc.push_back(translateDotFill(*i));
        else
        {
            //TO-DO: Raise exception for unrecognizable instructions.
            std::cerr << "Unrecognizable Instruction: " << i->instruction << std::endl;
        }
        std::cerr << "Mechine code: " << static_cast<mc_t_s>(_mc.back()) << std::endl << std::endl;
    }
}

vector<mc_t> Assembler::getMechineCode()
{
    return _mc;
}

vector<string> readFromFile(const string &filename)
{
    std::ifstream ifs(filename.c_str());
    string tmp;
    vector<string> lines;

    while(ifs)
    {
        getline(ifs, tmp);
        if(tmp.size())
            lines.push_back(tmp);
    }

    ifs.close();
    return lines;
}

void writeToFile(const string &filename, const vector<mc_t> &mc)
{
    std::ofstream ofs(filename.c_str());
    //ofs << std::showbase << std::hex;

    for(vector<mc_t>::const_iterator i = mc.begin();
            i != mc.end(); ++i)
        ofs << static_cast<mc_t_s>(*i) << '\n';
    ofs << std::flush;

    ofs.close();
}

int main(int argc, char **argv)
{
    if(argc < 3)
        return EXIT_FAILURE;

    try
    {
        vector<string> as = readFromFile(argv[1]);
        Assembler assembler;
        assembler.setAssembly(as);

        assembler.synAnalyse();
        assembler.passOne();
        assembler.passTwo();

        writeToFile(argv[2], assembler.getMechineCode());
    }
    catch(...)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

