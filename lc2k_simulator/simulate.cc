#include<iostream>
#include<vector>
#include<string>
#include<cstring>
#include<fstream>
#include<algorithm>
#include<cstdlib>
#include<stdexcept>

#include<stdint.h>

typedef int_least32_t word_t;
typedef uint_least32_t mc_t;

class LC2KMachine
{
    public:
        static const size_t NUMMEMORY = 65536;
        static const size_t NUMREGS = 8;

    private:
        size_t _mem_size, _pc;
        bool _ready;
        word_t _registers[NUMREGS];
        word_t *_memory;

        inline void handleAdd(mc_t mc);
        inline void handleNand(mc_t mc);
        inline void handleLw(mc_t mc);
        inline void handleSw(mc_t mc);
        inline void handleBeq(mc_t mc);
        inline void handleJalr(mc_t mc);
        inline void handleHalt();
        inline void handleNoop();

        int interpretOffset(mc_t mc)
        {
            mc_t offset = mc & 0xffff;
            return (offset & 0x8000) ? offset - 0x10000 : offset;
        }

    public:
        LC2KMachine(): _memory(NULL), _ready(false) {}
        ~LC2KMachine()
        {
            if(_memory)
                delete [] _memory;
        }

        void next();
        void setMachineCode(std::vector<word_t> mc);
        void printState(std::ostream &os = std::cout);
        void printInitMem(std::ostream &os = std::cout);
};

class MachineHalt: public std::exception
{ };

class MachineRuntimeError: public std::runtime_error
{
    public:
        MachineRuntimeError(const char *msg): std::runtime_error(msg) {}
};

void LC2KMachine::handleAdd(mc_t mc)
{
    mc_t regA = (mc >> 19) & 0x7,
         regB = (mc >> 16) & 0x7,
         regC = mc & 0x7;

    _registers[regC] = _registers[regA] + _registers[regB];
    ++_pc;
}

void LC2KMachine::handleNand(mc_t mc)
{
    mc_t regA = (mc >> 19) & 0x7,
         regB = (mc >> 16) & 0x7,
         regC = mc & 0x7;

    _registers[regC] = ~(_registers[regA] & _registers[regB]);
    ++_pc;
}

void LC2KMachine::handleLw(mc_t mc)
{
    mc_t regA = (mc >> 19) & 0x7,
         regB = (mc >> 16) & 0x7;
    int addr = _registers[regA];

    addr += interpretOffset(mc);
    if(addr < 0 || addr >= _mem_size)
        throw MachineRuntimeError("Access invalid memory!");
    _registers[regB] = _memory[addr];
    ++_pc;
}

void LC2KMachine::handleSw(mc_t mc)
{
    mc_t regA = (mc >> 19) & 0x7,
         regB = (mc >> 16) & 0x7;
    int addr = _registers[regA];

    addr += interpretOffset(mc);
    if(addr < 0 || addr >= _mem_size)
        throw MachineRuntimeError("Access invalid memory!");
    _memory[addr] = _registers[regB];
    ++_pc;
}

void LC2KMachine::handleBeq(mc_t mc)
{
    mc_t regA = (mc >> 19) & 0x7,
         regB = (mc >> 16) & 0x7;

    if(_registers[regA] == _registers[regB])
        _pc += interpretOffset(mc);
    ++_pc;
}

void LC2KMachine::handleJalr(mc_t mc)
{
    mc_t regA = (mc >> 19) & 0x7,
         regB = (mc >> 16) & 0x7;

    _registers[regB] = _pc + 1;
    _pc = _registers[regA];
}

void LC2KMachine::handleHalt()
{
    _ready = false;
    throw MachineHalt();
}

void LC2KMachine::handleNoop()
{
    ++_pc;
}

void LC2KMachine::printState(std::ostream &os)
{
    if(!_ready)
        throw std::runtime_error("Machine is not ready!");

    os << "\n@@@\nstate:\n"
        "\tpc " << _pc << "\n"
        "\tmemory:\n";

    for(size_t i = 0; i < _mem_size; ++i)
        os << "\t\tmem[ " << i << " ] " << _memory[i] << "\n";

    os << "\tregisters:\n";
    for(size_t i = 0; i < NUMREGS; ++i)
        os << "\t\treg[ " << i << " ] " << _registers[i] << "\n";

    os << "end state" << std::endl;
}

void LC2KMachine::printInitMem(std::ostream &os)
{
    if(!_ready)
        throw std::runtime_error("Machine is not ready!");

    for(size_t i = 0; i < _mem_size; ++i)
        os << "memory[" << i << "]=" << _memory[i] << "\n";
    os.flush();
}

void LC2KMachine::next()
{
    if(_pc >= _mem_size)
        throw MachineRuntimeError("Access invalid memory!");

    mc_t *mc = reinterpret_cast<mc_t *>(_memory + _pc);
    mc_t opcode = (*mc >> 22) & 0x7;
    switch(opcode)
    {
        case 0:
            handleAdd(*mc); break;
        case 1:
            handleNand(*mc); break;
        case 2:
            handleLw(*mc); break;
        case 3:
            handleSw(*mc); break;
        case 4:
            handleBeq(*mc); break;
        case 5:
            handleJalr(*mc); break;
        case 6:
            handleHalt(); break;
        case 7:
            handleNoop(); break;
    }
}

void LC2KMachine::setMachineCode(std::vector<word_t> mc)
{
    if(_ready)
        throw std::runtime_error("Machine has been ready!");

    if(_memory)
        delete [] _memory;

    memset(_registers, 0, sizeof(_registers));

    _mem_size = mc.size();
    _memory = new word_t[_mem_size];
    std::copy(mc.begin(), mc.end(), _memory);
    _pc = 0;
    _ready = true;
}

std::vector<word_t> readFromFile(const std::string &filename)
{
    std::ifstream ifs(filename.c_str());
    if(!ifs)
        throw std::runtime_error("Open file failed: " + filename);

    std::vector<word_t> words;
    word_t tmp;
    while(ifs >> tmp)
    {
        words.push_back(tmp);
    }

    return words;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cerr << "Usage: simulate file.mc" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        LC2KMachine machine;
        machine.setMachineCode(readFromFile(argv[1]));
        machine.printInitMem();

        while(true)
        {
            machine.printState();
            machine.next();
        }
    }
    catch(MachineHalt)
    {
    }
    catch(std::runtime_error e)
    {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
