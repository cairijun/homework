/* FSM for LC */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int memoryAddress;
    int memoryData;
    int instrReg;
    int aluOperand;
    int aluResult;
    int numMemory;
} stateType;

void printState(stateType *, char *);
void run(stateType);
int memoryAccess(stateType *, int);
int convertNum(int);

int main(int argc, char *argv[])
{
    int i;
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    /* initialize memories and registers */
    for (i=0; i<NUMMEMORY; i++) {
        state.mem[i] = 0;
    }
    for (i=0; i<NUMREGS; i++) {
        state.reg[i] = 0;
    }

    state.pc=0;

    /* read machine-code file into instruction/data memory (starting at
     * address 0) */
    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s\n", argv[1]);
        perror("fopen");
        exit(1);
    }

    for (state.numMemory=0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    printf("\n");
    /* run never returns */
    run(state);

    return(0);
}

void printState(stateType *statePtr, char *stateName)
{
    int i;
    static int cycle = 0;
    printf("\n@@@\nstate %s (cycle %d)\n", stateName, cycle++);
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("\tinternal registers:\n");
    printf("\t\tmemoryAddress %d\n", statePtr->memoryAddress);
    printf("\t\tmemoryData %d\n", statePtr->memoryData);
    printf("\t\tinstrReg %d\n", statePtr->instrReg);
    printf("\t\taluOperand %d\n", statePtr->aluOperand);
    printf("\t\taluResult %d\n", statePtr->aluResult);
}

/*
 * * Access memory:
 * * readFlag=1 ==> read from memory
 * * readFlag=0 ==> write to memory
 * * Return 1 if the memory operation was successful, otherwise return 0
 * */
int memoryAccess(stateType *statePtr, int readFlag)
{
    static int lastAddress = -1;
    static int lastReadFlag = 0;
    static int lastData = 0;
    static int delay = 0;

    if (statePtr->memoryAddress < 0 || statePtr->memoryAddress >= NUMMEMORY) {
        printf("memory address out of range\n");
        exit(1);
    }

    /*
     * * If this is a new access, reset the delay clock.
     * */
    if ( (statePtr->memoryAddress != lastAddress) ||
            (readFlag != lastReadFlag) ||
            (readFlag == 0 && lastData != statePtr->memoryData) ) {
        delay = statePtr->memoryAddress % 3;
        lastAddress = statePtr->memoryAddress;
        lastReadFlag = readFlag;
        lastData = statePtr->memoryData;
    }

    if (delay == 0) {
        /* memory is ready */
        if (readFlag) {
            statePtr->memoryData = statePtr->mem[statePtr->memoryAddress];
        } else {
            statePtr->mem[statePtr->memoryAddress] = statePtr->memoryData;
        }
        return(1);
    } else {
        /* memory is not ready */
        delay--;
        return(0);
    }
}

int convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit integer */
    if (num & (1 << 15) ) {
        num -= (1 << 16);
    }
    return(num);
}

#define _S(name) name: printState(&state, #name)
#define _DISPATCH(code, label) if((state.instrReg & 0x1C00000) == (code << 22)) goto label
#define _REG1 state.reg[(state.instrReg >> 19) & 0x7]
#define _REG2 state.reg[(state.instrReg >> 16) & 0x7]
#define _DES_REG state.reg[state.instrReg & 0x7]
#define _OFFSET convertNum(state.instrReg & 0xffff)
void run(stateType state)
{
    int bus, memState;

_S(fetch);
    bus = state.pc++;
    state.memoryAddress = bus;
    memoryAccess(&state, 1);
    goto fetch_wait;

_S(fetch_wait);
    if(memoryAccess(&state, 1))
    {
        bus = state.memoryData;
        state.instrReg = bus;
        goto dispatch;
    }
    goto fetch_wait;

_S(dispatch);
    _DISPATCH(0x0, add);
    _DISPATCH(0x1, nand);
    _DISPATCH(0x2, lw);
    _DISPATCH(0x3, sw);
    _DISPATCH(0x4, beq);
    _DISPATCH(0x5, jalr);
    _DISPATCH(0x6, halt);
    _DISPATCH(0x7, noop);


_S(add);
    bus = _REG1;
    state.aluOperand = bus;
    goto add_cal;

_S(add_cal);
    bus = _REG2;
    state.aluResult = state.aluOperand + bus;
    goto add_finish;

_S(add_finish);
    bus = state.aluResult;
    _DES_REG = bus;
    goto fetch;


_S(nand);
    bus = _REG1;
    state.aluOperand = bus;
    goto nand_cal;

_S(nand_cal);
    bus = _REG2;
    state.aluResult = ~(state.aluOperand & bus);
    goto nand_finish;

_S(nand_finish);
    bus = state.aluResult;
    _DES_REG = bus;
    goto fetch;


_S(lw);
    bus = _REG1;
    state.aluOperand = bus;
    goto lw_cal_addr;

_S(lw_cal_addr);
    bus = _OFFSET;
    state.aluResult = state.aluOperand + bus;
    goto lw_read;

_S(lw_read);
    bus = state.aluResult;
    state.memoryAddress = bus;
    memoryAccess(&state, 1);
    goto lw_read_wait;

_S(lw_read_wait);
    if(!memoryAccess(&state, 1))
        goto lw_read_wait;
    bus = state.memoryData;
    _REG2 = bus;
    goto fetch;


_S(sw);
    bus = _REG1;
    state.aluOperand = bus;
    goto sw_cal_addr;

_S(sw_cal_addr);
    bus = _OFFSET;
    state.aluResult = state.aluOperand + bus;
    goto sw_set_addr;

_S(sw_set_addr);
    bus = state.aluResult;
    state.memoryAddress = bus;
    goto sw_store;

_S(sw_store);
    bus = _REG2;
    state.memoryData = bus;
    memoryAccess(&state, 0);
    goto sw_store_wait;

_S(sw_store_wait);
    if(!memoryAccess(&state, 0))
        goto sw_store_wait;
    goto fetch;


_S(beq);
    bus = _REG1;
    state.aluOperand = bus;
    goto beq_cal_diff;

_S(beq_cal_diff);
    bus = _REG2;
    state.aluResult = state.aluOperand - bus;
    goto beq_check;

_S(beq_check);
    if(state.aluResult)
        goto fetch;
    bus = _OFFSET;
    state.aluOperand = bus;
    goto beq_cal_addr;

_S(beq_cal_addr);
    bus = state.pc;
    state.aluResult = state.aluOperand + bus;
    goto beq_branch;

_S(beq_branch);
    bus = state.aluResult;
    state.pc = bus;
    goto fetch;


_S(jalr);
    bus = state.pc;
    _REG2 = bus;
    goto jalr_branch;

_S(jalr_branch);
    bus = _REG1;
    state.pc = bus;
    goto fetch;


_S(halt);
    return;


_S(noop);
    goto fetch;
}
