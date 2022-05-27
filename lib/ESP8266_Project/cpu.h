#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdbool.h>

#include "parse.h"
#include "peripherycommon.h"
//#include "peripherygpio.h"
//include other  pheriphery structure definition here
//always put sistem includes before incusion of my own header files 

struct Variable;

void initCPU(struct CpuState *cpu, const char* filename);
void registerDefaultInstructions(struct CpuState *pstate);

void parseArgument(struct CpuState *pstate, const char *value, struct Variable **arg);
void printCpuState(struct CpuState *pstate);

int getIndexByName(struct CpuState *pstate, const char *instruction);

enum EnumType
{
    BOOL,
    INT,
    FLOAT,
    UNDEFINED
};

struct Variable
{
    //char type[NAME_TYPE_SIZE];
    bool isLiteral;

    enum EnumType type;
    char name[NAME_TYPE_SIZE];

    union val {
        int i;
        bool b;
        float f;
    } val;
};

struct Instruction
{
    char name[LINE_CHAR_NUM];
    void (*function)(struct CpuState *pstate, struct Line *pline);
};

struct CpuState
{
    int nextinstructionlinenumber;
    char filename[200];
    char RLO;

    struct Instruction instructionlist[30];
    int instructionlistnumber;

    struct Variable variablelist[VARIABLELIST_SIZE];
    int variablelistnumber;

    struct Periphery peripherylist[PERIPHERYLIST_SIZE];
    int peripherylistnumber;

    struct PeripheryType peripherytypelist[PERIPHERYTYPELIST_SIZE];
    int peripherytypelistnumber;

    //instructions should be in an array  but i should have function that adds instructions to the array
    //instrucion list should be empty and
    //add instruction function
};


#endif