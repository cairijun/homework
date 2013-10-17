#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<algorithm>
#include<cstdlib>
#include<stdexcept>

#include<stdint.h>

typedef uint_least32_t word_t;
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
        inline void handleHalt(mc_t mc);
        inline void handleNoop(mc_t mc);

    public:
        LC2KMachine(): _memory(NULL), _ready(false) {}
        ~LC2KMachine()
        {
            if(_memory)
                delete [] _memory;
        }

        void next();

        void setMachineCode(std::vector<mc_t> mc)
        {
            if(_ready)
                throw std::runtime_error("Machine has been ready!");

            _mem_size = mc.size();
            _memory = new word_t[_mem_size];
            std::copy(mc.begin(), mc.end(), _memory);
            _pc = 0;
            _ready = true;
        }

        void printState(std::ostream &os = std::cout);
        void printInitMem(std::ostream &os = std::cout);
};

class MachineStopped: public std::exception
{ };

void LC2KMachine::handleAdd(mc_t mc)
{
}

void LC2KMachine::handleNand(mc_t mc)
{
}

void LC2KMachine::handleLw(mc_t mc)
{
}

void LC2KMachine::handleSw(mc_t mc)
{
}

void LC2KMachine::handleBeq(mc_t mc)
{
}

void LC2KMachine::handleJalr(mc_t mc)
{
}

void LC2KMachine::handleHalt(mc_t mc)
{
}

void LC2KMachine::handleNoop(mc_t mc)
{
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
}

std::vector<mc_t> readFromFile(const std::string &filename)
{
    std::ifstream ifs(filename.c_str());
    if(!ifs)
        throw std::runtime_error("Open file failed: " + filename);

    std::vector<mc_t> _mc;
    mc_t tmp;
    while(ifs)
    {
        ifs >> tmp;
        _mc.push_back(tmp);
    }

    return _mc;
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
            machine.next();
            machine.printState();
        }
    }
    catch(MachineStopped)
    {
    }
    catch(std::runtime_error e)
    {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
