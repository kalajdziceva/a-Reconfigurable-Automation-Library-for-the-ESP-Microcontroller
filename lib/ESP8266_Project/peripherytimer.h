#ifndef PERIPHERYTIMER_H
#define PERIPHERYTIMER_H

enum TimerMode{
    OnDelay,
    OffDelay,
    Pulse,
    PulseExt
};

struct TimerData{
//    char Output;
    enum TimerMode Mode;
    int Time;
    int ActualTime;
    struct Variable* pVariableInput;
    struct Variable* pVariableOutput;
    struct Variable* pVariableReset;
};


void timer_registertype(struct CpuState *pstate);
void timer_create(struct CpuState *pstate, struct Line *pline);
void timer_loop(struct CpuState *pstate, struct Periphery* pperiphery, char cyclestart);
void timer_set(struct CpuState *pstate, struct Periphery* pperiphery, struct Line *pline);
void timer_setup(struct CpuState *pstate,struct Periphery* pperiphery);


#endif