#include <stdio.h>
#include <stdlib.h> // For exit()
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "parse.h"
#include "cpu.h"
#include "peripherygpio.h"
#include "peripherytimer.h"
/*

#include "peripherytimer.h"
*/
#pragma warning(disable : 4996) // for vs

// label:   instruction   arg0 arg1 arg2
//  instruction arg0 arg1 arg2

/*
struct Variable;
struct Instruction;
struct Line;
*/

// global variables

struct CpuState CPU;
struct Line actualline;

char _actualFilename[200];
int _actualFileLineNumber;
FILE *_actualFile;

/*


//this goes to peripherygpio.c(pp)

*/
int GpioDataNumber=0;
struct GpioData gpiodata[10];

//timers...
//www.plcacademy.com/plc-timers
//off delay
//on delay
//pulse + extended pulse
//...

int TimerDataNumber=0;
struct TimerData timerdata[10];

//#include <time.h>
//... use google
//time()
//in arduino: millis()

void cpuLoop();
void test();

int cycleStart = 0;

void cpuLoop()
{
    // load the next line (first if you reached the end)
    // increment the instruction pointer (number)
    // find the instruction
    // execute the instruction

    //later handle peripheries
    struct Line line;

    bool br = readLine(CPU.filename, CPU.nextinstructionlinenumber, &line);

    if (br == false) {
        
        printf("------------------\n");
        printf("CPU halted: press any key to start loop again");
        initCPU(&CPU, "input.txt");
        getchar();
        return;
    }

    CPU.nextinstructionlinenumber++;

    int index = getIndexByName(&CPU, line.instruction);

    if (index != -1) {

        // do not call loop for create & set instructions for gpio only for standard cpu instructions before execution of instruction
        if (strcmp(line.instruction, "create") != 0 && strcmp(line.instruction, "set") != 0) {
            for (int i = 0; i < CPU.peripherylistnumber; i++) {
                printf("calling gpio loop before executing instruction %s\n", CPU.instructionlist[getIndexByName(&CPU, line.instruction)].name);
                CPU.peripherylist[i].pType->loop(&CPU, &CPU.peripherylist[i], cycleStart); 
            }
            cycleStart = !cycleStart;
        }


        CPU.instructionlist[getIndexByName(&CPU, line.instruction)].function(&CPU, &line);

    }

  


}

void test()
{

    initCPU(&CPU, "input.txt");



    while (1)
    {
        cpuLoop();
    }
}


int main()
{
    test();
}
