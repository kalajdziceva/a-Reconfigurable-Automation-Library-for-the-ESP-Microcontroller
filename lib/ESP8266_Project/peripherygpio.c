
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "peripherycommon.h"
#include "cpu.h"
#include "peripherygpio.h"
#include "peripherytimer.h"

void gpio_registertype(struct CpuState *pstate) {
    //TODO add gpio periphery to the cpu periphery types
    struct PeripheryType pt;


    strcpy(pt.type_name, "GPIO");
    pt.create = gpio_create;
    pt.setup = gpio_setup;
    pt.set = gpio_set;
    pt.loop = gpio_loop;

    addPeripheryType(pstate, &pt);
}

void gpio_create(struct CpuState *pstate, struct Line *pline) {
    //add a new instance to the periphery list in the cpu state, fill in the fields

    if (pstate->peripherylistnumber == PERIPHERYLIST_SIZE) {
        printf("Cannot add more than %d peripheries\n", PERIPHERYLIST_SIZE);
        return;
    }

    static char name[]="GPIO"; //declare a global static variable in gpio.cpp

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

void gpio_loop(struct CpuState *pstate, struct Periphery* pperiphery, char cyclestart) {

    //depending on the time of pheriphery we need different type of enviroment, update outputs and read inputs every cycle, if some communication and multiple devices communicating we need to let pheripery do what is needed ven  more often than one loop
    //on reading and updating only do it at cycle start , cycle start i have if some other pheripheries need service
    //boundaries , put information about pheripheries, pheriphery specific stuff put in structure
    //here we  send high or low output
    //arduino library will be still available,use printf for the start to check
    //if not between instructions, update the state (read in new value from te port / write to the port)

            //if cyclestart is 0 just return
            //if its just 1 only update inputs and outputs
            //making virtual periphery and virtual in sync
            //from the main loop call this function but for now only interested in the begging of the cycle
            //add one more parameter -default value- when start first cycle this is going to be value that i output first
            // go through all the peripheries in loop in cpuloop, type contains the pointers of the functions that are needed
            //from cpuloop call this loop
            //some print do for now
            //if pin is output i need to look at variable value , if the type is bool its 0 or 1
            //if its integer if its 0 low,anything else is high, same for float
            //if gpio is input check what type of variable is, range for input is also 0 or 1 and do the same checks as for output

    //gdje se podesava cyclestart - call different pheryphery loops in cpu loop after or before each instruction i cann call this
    //if i am going to execute first instruction then i am going to set cyclestart =true
    //gdje citamo ako je mode input i sta citamo, koju vrijednost
    //if its input digitalread 0 if it
    //commentout line with digitalread and write number instead of digital read
    //take into account variable type  remeber if variable is float and i write 1 it is not going to be same write float 1 if it is float
    //
    //https://www.aranacorp.com/en/using-the-millis-function-of-the-arduino-ide/
        #ifdef DEBUG_GIO
       printf("gpio cyclestart: %d\n", cyclestart);
        #endif
        
       if (cyclestart == 0) {
           return;
       }
       else if (cyclestart == 1) {
           struct GpioData *pdata = pperiphery->data;

           if (pdata->Mode == 0) { // read? 
           #ifdef DEBUG_GPIO
                printf("gpio loop: reading data from pin %d\n", pdata->Pin);
            #endif
           }

           else if (pdata->Mode == 1) {
               if (pdata->pVariable != NULL) {
                   #ifdef DEBUG_GPIO
                    printf("gpio loop: writing variable %s with value %d to pin %d\n", pdata->pVariable->name, pdata->pVariable->val, pdata->Pin);
                    #endif
               }

           }
       }

}


void gpio_set(struct CpuState *pstate, struct Periphery* pperiphery, struct Line *pline) {
    //check settings keys: Mode, Pin, apply settings to real GPIO peripheries
    //line pointer line should consist of instruction and arguments and compare wether the first or second argument = string mode
    //if true parse next argument and set that value to mode
    //periphery pointer describing set gpio  and then argument  1 can be mode, pin or variabele so compare to these and then arfgument 2 
    //is again a string and i need to compare or parse the value wether i am setting mode or pin
    //if argument 1 is pin create number from argument 2 and set this to pin value
    //if argument is variable then i have to lookup in varables name if its not available create variable  and set adress to gpio data

                //first line is not correct i dont have to lookup bcs i already have periperiphery pointer
            //itterate overall peripheries not periphery types and look for specific name
            //after i found it i check what second parameter is mode or pin, or variable
            //accoding to this i set information in instance descriptor in gpio data structure and fill out parameters
            //fill out param in struct GpioData{
                // configuring real periphery

                //there will be general set instruction and that general set function should lookup types and accoding to the type it should call specific pheriphery function which is in the periphery type structure
                //and there i could also lookup periphery instance

    if (pperiphery->data == NULL)
        //in the c file memory can be allocated globaly in array and find next one that is empty if you are out of these create error and stop execution
        pperiphery->data = calloc(1, sizeof (struct GpioData));

    struct GpioData *pdata = pperiphery->data;

    if (!strcmpi(pline->arguments[1], "pin") && pline->argsnum >= 3) {
        #ifdef DEBUG_GPIO
        printf("gpio set: setting pin to value %d\n", atoi(pline->arguments[2]));
        #endif
        pdata->Pin = atoi(pline->arguments[2]);

    }
    else if (!strcmpi(pline->arguments[1], "mode") && pline->argsnum >= 3) {
        #ifdef DEBUG_GPIO
        printf("gpio set: setting mode to value %d\n", atoi(pline->arguments[2]));
        #endif
        pdata->Mode = atoi(pline->arguments[2]);

    }
    else {
        int i = 0;
        for (i = 0; i < pstate->variablelistnumber; i++) {
            if (!strcmpi(pstate->variablelist[i].name, pline->arguments[1])) {
                #ifdef DEBUG_GPIO
                printf("gpio set: setting existing variable %s for gpio %s\n", pstate->variablelist[i].name,  pperiphery->name);
                #endif
                pdata->pVariable = &pstate->variablelist[i];
                break;
            }
        }

        if (i == pstate->variablelistnumber) {
            printf("gpio set: variable not found %s\n", pline->arguments[1]);
            if (pstate->variablelistnumber == VARIABLELIST_SIZE) {
                printf("gpio set error: cannot introduce more variables\n");
                return;
            }
            #ifdef DEBUG_GPIO
            printf("introducting new gpio variable %s\n", pline->arguments[1]);
            #endif
            pstate->variablelist[pstate->variablelistnumber].isLiteral = false;
            strcpy(pstate->variablelist[pstate->variablelistnumber].name, pline->arguments[1]);
            pstate->variablelist[pstate->variablelistnumber].type = INT;
            pstate->variablelist[pstate->variablelistnumber].val.i = 0;

        }

    }


}


void gpio_setup(struct CpuState *pstate,struct Periphery* pperiphery) {
    // for now we dont need it some initialisation for communication, leave it for now
}



void addPeripheryType(struct CpuState *pstate, struct PeripheryType *pptype){
    //register the p type to the cpu state p type list (copy)
    if (pstate->peripherytypelistnumber < PERIPHERYTYPELIST_SIZE) {
        pstate->peripherytypelist[pstate->peripherytypelistnumber++] = *pptype;

       // printf("Periphery type %s added\n", pptype->type_name);

    } else {
        printf("error: cannot add more periphery types\n");
    }
}






void printPeripheryState(struct CpuState *pstate) {
    for (int i = 0; i <  pstate->peripherylistnumber; i++) {
        printf("++++++++++++++++++++++\n", pstate->peripherylist[i].name);
        printf("periphery name: %s\n", pstate->peripherylist[i].name);
        printf("periphery type: %s\n", pstate->peripherylist[i].pType->type_name);
        printf("settings for this periphery are: \n");
        
        if (!strcmpi(pstate->peripherylist[i].pType->type_name, "GPIO")) {
            struct GpioData *gd =  pstate->peripherylist[i].data;
            printf("mode: %d\n", gd->Mode);
            printf("pin: %d\n", gd->Pin);
            printf("var: %s\n", gd->pVariable->name);
            
        } else if (!strcmpi(pstate->peripherylist[i].pType->type_name, "TIMER")) {
            struct TimerData *td =  pstate->peripherylist[i].data;
            printf("mode: %d\n", td->Mode);
            printf("time: %d\n", td->Time);
            printf("time: %d\n", td->ActualTime);
            if (td->pVariableInput != NULL) {
                printf("input var: %s\n", td->pVariableInput->name);
            }
            if(td->pVariableOutput != NULL) {
                printf("input var: %s\n", td->pVariableOutput->name);
            }
            if(td->pVariableReset != NULL) {
                printf("input var: %s\n", td->pVariableReset->name);
            }
        }

        
    }

}
