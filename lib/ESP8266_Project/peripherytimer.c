

#include "parse.h"
#include "peripherycommon.h"
#include "cpu.h"
#include "peripherygpio.h"
#include "peripherytimer.h"

#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#define DEBUG_TIMER

void timer_registertype(struct CpuState *pstate) {
    
    struct PeripheryType pt;


    strcpy(pt.type_name, "TIMER");
    pt.create = timer_create;
    pt.setup = timer_setup;
    pt.set = timer_set;
    pt.loop = timer_loop;

    addPeripheryType(pstate, &pt);
}




void timer_create(struct CpuState *pstate, struct Line *pline){
    //add a new instance to the periphery list in the cpu state, fill in the fields
    //millis();
    //types of timers in plc: all 3 of them on,off,ondely
    //gpiocreate similar to this, i create new instance
    //add a new instance to the periphery list in the cpu state, fill in the fields

    if (pstate->peripherylistnumber == PERIPHERYLIST_SIZE) {
        printf("Cannot add more than %d peripheries\n", PERIPHERYLIST_SIZE);
        return;
    }

    static char name[]="TIMER"; //declare a global static variable 

    unsigned char found=0;
    for (int i=0;i<pstate->peripherytypelistnumber;i++){
        if (strcmp(name, pstate->peripherytypelist[i].type_name)==0){
            pstate->peripherylist[pstate->peripherylistnumber].pType=&pstate->peripherytypelist[i];
            found=1;
            break;
        }
    }
    if (found==0){
        printf("Periphery type not found %s\n", name);
        return;
    }

    strcpy(pstate->peripherylist[pstate->peripherylistnumber].name,pline->arguments[1]);
    pstate->peripherylistnumber++;

}

void timer_loop(struct CpuState *pstate, struct Periphery* pperiphery, char cyclestart){
    //if not between instructions, update the state (update the actual time in the timerdata struct, update the  output variable
    //here i have to keep track of ms time that i updated each time put ms counter and i have to get acctual time , calculate difference and update the times
    //update the output as well variable of the timer
    //measure time calculate time difference how much time passed since last time update actual time actual time-previous time if its 0 dont do much
    //if its not zero  do whats written bellow, save actual time to the timer data
    // so i know how much time has passed
    #ifdef DEBUG_TIMER
    //printf ("timer loop:\n");
    #endif

    struct TimerData *td = pperiphery->data;

    if (millis() - td->ActualTime > td->Time) {
        #ifdef DEBUG_TIMER
        printf("timer triggerred: %s %d ms\n", pperiphery->name, td->Time);
        #endif
        td->ActualTime = millis();
    }



    // behavior of modes when it's  OnDelay, Offdelay, 
    //internal acctual time counter in the timer, preset time which is given time for the timer thats parameter that comes from set function
    //ondelay whenever i see 0 on the input i set actual time for this timer to 0, whener input is not 0 or 1 or true i set 
    //i increment time by the time that passed since the last update
    //it measures how long the input is high
    //if the actual time is larger or equal preset time i output 1 otherwise i output 0
    //if i reach max time then dont increment, if i reach preset time i dont need to increment, 
    //important is not to allow system to overflow
    //pulse timer:normal and extended, normal one limits length of the time when the output is 1
    // if input is 0 i output 0, i reset actual time to 0, if the input is 1 i increment actual time counter, if actual time is less then preset time i output 1 otherwise i output 0
    //extended pusle timer:this one always outputs preset time length , whenever there is rising edge or 1 on input i output pulse
    //
}


void timer_set(struct CpuState *pstate, struct Periphery* pperiphery, struct Line *pline){
    //check settings keys: Time, Input, Output
    //variable can be input, set type and time value

    if (pperiphery->data == NULL)
        //in the c file memory can be allocated globaly in array and find next one that is empty if you are out of these create error and stop execution
        pperiphery->data = calloc(1, sizeof (struct TimerData));

    struct TimerData *pdata = pperiphery->data;

    if (pline->argsnum < 3) {
        printf("timer set error: not enough parameters\n");
        return;
    }

 //   printf("timer arg: %s\n", pline->arguments[1]);

    if (!strcmpi(pline->arguments[1], "period")) {
        printf("timer set: setting time %d milis\n", atoi(pline->arguments[2]));
        pdata->Time = atoi(pline->arguments[2]); // must be number
        pdata->ActualTime = millis();

    }
    else if (!strcmpi(pline->arguments[1], "mode")) {
        printf("timer set: setting mode to value %d\n", atoi(pline->arguments[2]));
        //pdata->Mode = atoi(pline->arguments[2]);

        if (!strcmpi(pline->arguments[2], "OnDelay")) {
            pdata->Mode = OnDelay;
        } else if (!strcmpi(pline->arguments[2], "OffDelay")) {
            pdata->Mode = OffDelay;
        } else if (!strcmpi(pline->arguments[2], "Pulse")) {
            pdata->Mode = Pulse;
        } else if (!strcmpi(pline->arguments[2], "PulseExt")) {
            pdata->Mode = PulseExt;
        } else {
            printf("timer error: Unsupported mode");
        }

    } else if (!strcmpi(pline->arguments[1], "input") || !strcmpi(pline->arguments[1], "output") || !strcmpi(pline->arguments[1], "reset")) {
        int i = 0;
        struct Variable *v;
        for (i = 0; i < pstate->variablelistnumber; i++) {
            if (!strcmpi(pstate->variablelist[i].name, pline->arguments[1])) {
                printf("timer set: setting existing input variable %s for timer %s\n", pstate->variablelist[i].name,  pperiphery->name);
                //pdata->pVariableInput = &pstate->variablelist[i];
                v = &pstate->variablelist[i];
                break;
            }
        }
        if (i == pstate->variablelistnumber) {
            printf("timer set: variable not found %s\n", pline->arguments[1]);
            if (pstate->variablelistnumber == VARIABLELIST_SIZE) {
                printf("timer set error: cannot introduce more variables\n");
                return;
            }
            printf("introducting new timer variable %s\n", pline->arguments[1]);
            pstate->variablelist[pstate->variablelistnumber].isLiteral = false;
            strcpy(pstate->variablelist[pstate->variablelistnumber].name, pline->arguments[1]);
            pstate->variablelist[pstate->variablelistnumber].type = INT;
            pstate->variablelist[pstate->variablelistnumber].val.i = 0;
            v = &pstate->variablelist[pstate->variablelistnumber];

            if (!strcmpi(pline->arguments[1], "input")) {
                pdata->pVariableInput = v;
                printf("setting input timer variable to %s\n", pline->arguments[1]);
            } else if (!strcmpi(pline->arguments[1], "output")) {
                printf("setting output timer variable to %s\n", pline->arguments[1]);
                pdata->pVariableOutput = v;
            } else {
                printf("setting reset timer variable to %s\n", pline->arguments[1]);
                pdata->pVariableReset = v;
            }

        }
    }
}

void timer_setup(struct CpuState *pstate,struct Periphery* pperiphery){
    //?
}


