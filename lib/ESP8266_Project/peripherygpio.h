#ifndef PERIPHERYHPIO_H
#define PERIPHERYHPIO_H

//go into cpu.h some stuff but pheriphery.h i should have
//have 1 more .h file and put it all header files

#include "peripherycommon.h"
#include "parse.h"



void gpio_registertype(struct CpuState *pstate);
void gpio_create(struct CpuState *pstate, struct Line *pline);
void gpio_loop(struct CpuState *pstate, struct Periphery* pperiphery, char cyclestart);
void gpio_set(struct CpuState *pstate, struct Periphery* pperiphery, struct Line *pline);
void gpio_setup(struct CpuState *pstate,struct Periphery* pperiphery);


void addPeripheryType(struct CpuState *pstate, struct PeripheryType *pptype);
void AddPeripheryTypeTimer(struct CpuState *pstate);
void printPeripheryState(struct CpuState *pstate);

struct GpioData{
    char Pin;
    //is it output or input
    char Mode;
    struct Variable* pVariable;
    //gpio specific information
    //add which pin it is, input, output, variable representing state of real gpio pin
};





#endif