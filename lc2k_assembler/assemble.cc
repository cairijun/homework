#include<iostream>
#include<fstream>
#include<cstdlib>
#include<string>
#include<sstream>
#include<vector>
#include<set>
#include<map>
#include<stdexcept>

#include<stdint.h>

typedef uint_least32_t mc_t;
typedef int_least32_t mc_t_s;

using std::vector;
using std::string;
using std::map;
using std::set;

class SyntaxError: public std::runtime_error
{
    public:
        SyntaxError(const string &msg): runtime_error(msg) {}
};

class IOError: public std::runtime_error
{
    public:
        IOError(const string &msg): runtime_error(msg) {}
};

class Assembler
{
    struct Instruction
    {
        string label;
        string instruction;
        vector<string> fields;
    };

    private:
        vector<string> _as;
        vector<Instruction> _ins;
        map<string, size_t> _label_table;
        vector<mc_t> _mc;

        const set<string> _INS = {"add", "nand", "lw", "sw", "beq", "jalr", "noop", "halt", ".fill"};
        const set<string> _R_INS = {"add", "nand"};
        const set<string> _I_INS = {"lw", "sw", "beq"};
        const set<string> _J_INS = {"jalr"};
        const set<string> _O_INS = {"noop", "halt"};
        const set<string> _PSEUDO_INS = {".fill"};

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

        inline static mc_t convertRegisterField(const string &field);

    public:
        void setAssembly(const vector<string> &as);
        void synAnalyse();
        void passOne();
        void passTwo();
        vector<mc_t> getMechineCode();
};

mc_t Assembler::convertRegisterField(const string &field)
{
    int reg = atoi(field.c_str());
    if((!reg && field != "0") || reg < 0 || reg > 7)
        throw SyntaxError("Invalid register: " + field);
    return reg;
}

int Assembler::interpreOffsetField(const string &field, bool *pLabelSign)
{
    bool sign = false;
    int offset = atoi(field.c_str());
    if(offset > 0x7fff | -offset > 0x8000)
        throw SyntaxError("Invalid offset: " + field);
    if(!offset && field != "0")
    {
        try
        {
            offset = _label_table.at(field);
        }
        catch(std::out_of_range e)
        {
            throw SyntaxError("Unspecified label: " + field);
        }
        sign = true;
    }

    if(pLabelSign)
        *pLabelSign = sign;
    return offset;
}

mc_t Assembler::translateAdd(const Instruction &ins)
{
    mc_t code = 0x00000000;
    mc_t regA = convertRegisterField(ins.fields[0]);
    mc_t regB = convertRegisterField(ins.fields[1]);
    mc_t regC = convertRegisterField(ins.fields[2]);

    code |= regA << 19 | regB << 16 | regC;
    return code;
}

mc_t Assembler::translateNand(const Instruction &ins)
{
    mc_t code = 0x00400000;
    mc_t regA = convertRegisterField(ins.fields[0]);
    mc_t regB = convertRegisterField(ins.fields[1]);
    mc_t regC = convertRegisterField(ins.fields[2]);

    code |= regA << 19 | regB << 16 | regC;
    return code;
}

mc_t Assembler::translateLw(const Instruction &ins)
{
    mc_t code = 0x00800000;
    mc_t regA = convertRegisterField(ins.fields[0]);
    mc_t regB = convertRegisterField(ins.fields[1]);
    int offset = interpreOffsetField(ins.fields[2]);

    code |= regA << 19 | regB << 16 | (offset & 0x0000ffff);
    return code;
}

mc_t Assembler::translateSw(const Instruction &ins)
{
    mc_t code = 0x00C00000;
    mc_t regA = convertRegisterField(ins.fields[0]);
    mc_t regB = convertRegisterField(ins.fields[1]);
    int offset = interpreOffsetField(ins.fields[2]);

    code |= regA << 19 | regB << 16 | (offset & 0x0000ffff);
    return code;
}

mc_t Assembler::translateBeq(const Instruction &ins, size_t pc)
{
    mc_t code = 0x01000000;
    mc_t regA = convertRegisterField(ins.fields[0]);
    mc_t regB = convertRegisterField(ins.fields[1]);

    bool labelSign;
    int offset = interpreOffsetField(ins.fields[2], &labelSign);
    if(labelSign)
        offset = offset - pc - 1;

    code |= regA << 19 | regB << 16 | (offset & 0x0000ffff);
    return code;
}

mc_t Assembler::translateJalr(const Instruction &ins)
{
    mc_t code = 0x01400000;
    mc_t regA = convertRegisterField(ins.fields[0]);
    mc_t regB = convertRegisterField(ins.fields[1]);

    code |= regA << 19 | (regB << 16 & 0x00070000);
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

void Assembler::setAssembly(const vector<string> &as)
{
    this->_as = as;
}

void Assembler::synAnalyse()
{
    _ins.empty();

    string tmp1, tmp2, tmp3;
    for(auto &i: _as)
    {
        if(!i.size())
            continue;

        Instruction new_ins;
        std::stringstream buf(i);
        buf.exceptions(std::stringstream::failbit);

        try
        {
            buf >> tmp1;
            if(!_INS.count(tmp1))
            {
                new_ins.label = tmp1;
                buf >> new_ins.instruction;
            }
            else
                new_ins.instruction = tmp1;

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
                std::stringstream expbuf;
                expbuf << "Unrecognizable assembly: " << i
                    << "\non line " << _ins.size()
                    << " (occured in syntax analysis).";
                throw SyntaxError(expbuf.str());
            }
        }
        catch(std::stringstream::failure e)
        {
            std::stringstream expbuf;
            expbuf << "Unrecognizable assembly: " << i
                << "\non line " << _ins.size()
                << " (occured in syntax analysis).";
            throw SyntaxError(expbuf.str());
        }

        _ins.push_back(new_ins);
    }
}

void Assembler::passOne()
{
    for(size_t i = 0; i < _ins.size(); ++i)
        if(_ins[i].label.size())
        {
            if(_label_table.count(_ins[i].label))
            {
                std::stringstream expbuf;
                expbuf << "Duplicated label: " << _ins[i].label << "\non address " << i
                    << ". Peviously defined on address " << _label_table[_ins[i].label]
                    << ".\n(occured in Pass 1)";
                throw SyntaxError(expbuf.str());
            }
            else
                _label_table[_ins[i].label] = i;
        }
}

void Assembler::passTwo()
{
    _mc.empty();

    for(auto &i: _ins)
    {
        try
        {
            if(i.instruction == "add")
                _mc.push_back(translateAdd(i));
            else if(i.instruction == "nand")
                _mc.push_back(translateNand(i));
            else if(i.instruction == "lw")
                _mc.push_back(translateLw(i));
            else if(i.instruction == "sw")
                _mc.push_back(translateSw(i));
            else if(i.instruction == "beq")
                _mc.push_back(translateBeq(i, _mc.size()));
            else if(i.instruction == "jalr")
                _mc.push_back(translateJalr(i));
            else if(i.instruction == "halt")
                _mc.push_back(translateHalt(i));
            else if(i.instruction == "noop")
                _mc.push_back(translateNoop(i));
            else if(i.instruction == ".fill")
                _mc.push_back(translateDotFill(i));
            else
                throw std::logic_error("Unhandle case: " + i.instruction);
        }
        catch(SyntaxError e)
        {
            std::stringstream expbuf;
            expbuf << e.what() << "\non address " << _mc.size() << " (occured in Pass 2).";
            throw SyntaxError(expbuf.str());
        }
    }
}

vector<mc_t> Assembler::getMechineCode()
{
    return _mc;
}

vector<string> readFromFile(const string &filename)
{
    std::ifstream ifs(filename.c_str());
    if(!ifs)
        throw IOError("Open input file failed: " + filename);

    string tmp;
    vector<string> lines;

    while(ifs)
    {
        getline(ifs, tmp);
        lines.push_back(tmp);
    }

    ifs.close();
    return lines;
}

void writeToFile(const string &filename, const vector<mc_t> &mc)
{
    std::ofstream ofs;
    ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    try
    {
        ofs.open(filename.c_str());
        //ofs << std::showbase << std::hex;

        for(auto &code: mc)
            ofs << static_cast<mc_t_s>(code) << '\n';
        ofs << std::flush;

        ofs.close();
    }
    catch(std::ofstream::failure)
    {
        throw IOError("Write to file failed: " + filename);
    }
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
    catch(SyntaxError e)
    {
        std::cerr << "Error occured when assembling.\n" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(IOError e)
    {
        std::cerr << "IOError occured: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

