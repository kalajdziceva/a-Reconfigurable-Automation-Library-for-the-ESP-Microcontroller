#ifndef PERIPHERYCOMMON_H
#define PERIPHERYCOMMON_H

#include "parse.h"

#define PERIPHERYLIST_SIZE 10
#define PERIPHERYSETTING_SIZE 10
#define PERIPHERYTYPELIST_SIZE 10

struct PeripherySetting;
struct PeripheryType;
struct Periphery;
struct CpuState;

struct PeripheryType {
    char type_name[LINE_CHAR_NUM];
    void (*create)(struct CpuState *pstate, struct Line *pline);
    void (*loop)(struct CpuState *pstate, struct Periphery* pperiphery, char cyclestart);
    void (*set)(struct CpuState *pstate, struct Periphery* pperiphery, struct Line *pline);
    void (*setup)(struct CpuState *pstate,struct Periphery* pperiphery);
};

struct Periphery {
    
    char name[LINE_CHAR_NUM];
    struct PeripheryType* pType;
    void* data;
    //struct PeripherySetting settings[PERIPHERYSETTING_SIZE];
    //int numSetting;
};

#endif