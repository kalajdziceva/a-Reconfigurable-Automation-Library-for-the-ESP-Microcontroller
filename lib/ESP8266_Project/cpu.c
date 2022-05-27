#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "peripherygpio.h"
#include "peripherytimer.h"
#include "cpu.h"


// internally used functions

void functionCreatePeriphery(struct CpuState *pstate, struct Line *pline);
// helper


void initCPU(struct CpuState *cpu, const char* filename) {

    memset(cpu, 0, sizeof *cpu);
    strncpy(cpu->filename, filename, 200);
    registerDefaultInstructions(cpu);
    gpio_registertype(cpu);
    timer_registertype(cpu);


    int i;
    for (i = 0; i < cpu->peripherytypelistnumber; i++) {
        printf("debug: %s\n", cpu->peripherytypelist[i].type_name);
    }

}


void addInstruction(struct CpuState *pstate, struct Instruction *pinstruction)
{
    if (pstate->instructionlistnumber >= VARIABLELIST_SIZE)
    {
        printf("Cannot add more instructions than %d\n", VARIABLELIST_SIZE);
        return;
    }
    pstate->instructionlist[pstate->instructionlistnumber] = *pinstruction;
    pstate->instructionlistnumber++;
}



int getIndexByName(struct CpuState *pstate, const char *instruction) {
    for (int i = 0; i < pstate->instructionlistnumber; i++) {
        if (strcmpi(instruction, pstate->instructionlist[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

int strcmpi(char const *a, char const *b)
{
    for (;; a++, b++) {

        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

int getVariableByName(struct CpuState *pstate, const char *varname) {
    for (int i = 0; i < pstate->variablelistnumber; i++) {
        if (strcmpi(pstate->variablelist[i].name, varname) == 0)
            return i;
    }
    return -1;
}



enum EnumType getType(const char *str) {
    int i;
    float f;
    char *end;

   if (strtol(str, &end, 10) && *end == '\0')
        return INT;

    if (strtod(str, &end) && *end == '\0')
        return FLOAT;

    if (strcmpi(str, "true") == 0 || strcmpi(str,"false") == 0)
        return BOOL;

    return UNDEFINED;
}


void parseArgument(struct CpuState *pstate, const char *value, struct Variable **arg) {

    enum EnumType t = getType(value);

    switch (t) {
        case UNDEFINED:
        {
            struct Variable *p;
            // 
            for (p = pstate->variablelist; p < &pstate->variablelist[pstate->variablelistnumber]; p++) {
                if (strcmpi(p->name, value) == 0) {
                    *arg = p;
                    break;
                }
            }

            if (p ==  &pstate->variablelist[pstate->variablelistnumber]) {
                pstate->variablelist[pstate->variablelistnumber].isLiteral = false;
                strcpy(pstate->variablelist[pstate->variablelistnumber].name, value);
                *arg = &pstate->variablelist[pstate->variablelistnumber];
                pstate->variablelistnumber++;
            }

        }
        break;

        case INT:
            (*arg)->isLiteral = true;
            (*arg)->val.i = atoi(value);
            (*arg)->type = INT;
        break;

        case FLOAT:
            (*arg)->isLiteral = true;
            (*arg)->val.f = atof(value);
            (*arg)->type = FLOAT;
        break;

        case BOOL:
            (*arg)->isLiteral = true;
            (*arg)->val.b = *value == 't' ? true: false;
            (*arg)->type = BOOL;
        break;

        default:
        break;

    }

}

//WGPIO alarm 1
//MAGIC 1 2 3 something 5 6 7 8 9 10

void functionMov(struct CpuState *pstate, struct Line *pline) {

    struct Variable args[LINE_ARGS_NUM] = {0};
    struct Variable *pargs[LINE_ARGS_NUM] = {0};

    // validation
    if (pline->argsnum != 2) {
     //   printf("Error - skipping instruction: mov instruction must have only two operands!\n");
       // printf("This one has: %s %d %s\n", pline->instruction, pline->argsnum, pline->arguments[0]);
        return;
    }

    for (int i = 0; i < pline->argsnum; i++)
    {
        pargs[i] = &args[i];
        parseArgument(pstate, pline->arguments[i], &pargs[i]);
    }

    if (args[0].isLiteral == true) {
        printf("Error - first operand must not be literal\n");
        return;
    }

    pargs[0]->val = pargs[1]->val;
    pargs[0]->type = pargs[1]->type;

}

void functionAdd(struct CpuState *pstate, struct Line *pline)
{
    struct Variable args[LINE_ARGS_NUM] = {0};
    struct Variable *pargs[LINE_ARGS_NUM] = {0};

    // validation
    if (pline->argsnum < 1) {
        printf("Error - skipping instruction: add instruction must more than 1 operand!\n");
        return;
    }

    for (int i = 0; i < pline->argsnum; i++)
    {
        pargs[i] = &args[i];
        parseArgument(pstate, pline->arguments[i], &pargs[i]);
    }

    if (args[0].isLiteral == true) {
        printf("Error - first operand must not be literal\n");
        return;
    }

    float sum = 0;
    for (int i = 0; i < pline->argsnum; i++) {
        if (pargs[i]->type == INT) 
            sum += pargs[i]->val.i;
        else if(pargs[i]->type == FLOAT)
            sum += pargs[i]->val.f;
        else {
            printf("Error - wrong value type - must be int or float\n");
            return;
        }
    }

    if (pargs[0]->type == INT)
        pargs[0]->val.i = (int)sum;
    else
        pargs[0]->val.f = sum;
}


void functionSub(struct CpuState *pstate, struct Line *pline)
{
    struct Variable args[LINE_ARGS_NUM] = {0};
    struct Variable *pargs[LINE_ARGS_NUM] = {0};

    // validation
    if (pline->argsnum < 1) {
        printf("Error - skipping instruction: add instruction must more than 1 operand!\n");
        return;
    }

    for (int i = 0; i < pline->argsnum; i++)
    {
        pargs[i] = &args[i];
        parseArgument(pstate, pline->arguments[i], &pargs[i]);
    }

    if (args[0].isLiteral == true) {
        printf("Error - first operand must not be literal\n");
        return;
    }

    float sum = 0;
    for (int i = 0; i < pline->argsnum; i++) {
        if (pargs[i]->type == INT) 
            sum -= pargs[i]->val.i;
        else if(pargs[i]->type == FLOAT)
            sum -= pargs[i]->val.f;
        else {
            printf("Error - wrong value type - must be int or float\n");
            return;
        }
    }

    if (pargs[0]->type == INT)
        pargs[0]->val.i = (int)sum;
    else
        pargs[0]->val.f = sum;
}

void functionPrint(struct CpuState *pstate, struct Line *pline) {

    struct Variable args[LINE_ARGS_NUM] = {0};
    struct Variable *pargs[LINE_ARGS_NUM] = {0};

    // validation
    if (pline->argsnum < 1) {
        printf("Error - skipping instruction: print instruction must have at least 1 operand!\n");
        return;
    }

    for (int i = 0; i < pline->argsnum; i++) {
        pargs[i] = &args[i];
        parseArgument(pstate, pline->arguments[i], &pargs[i]);
    }


    for (int i = 0; i < pline->argsnum; i++) {

        if (pargs[i]->isLiteral == true)
            printf(" literal = ");
        else
            printf(" %s = ", pargs[i]->name);

        switch (pargs[i]->type) {
            case INT:
                printf("%d", pargs[i]->val.i);
            break;

            case FLOAT:
                printf("%f", pargs[i]->val.f);
            break;

            case BOOL:
                printf("%s", pargs[i]->val.b ? "true" : "false");
            break;

            default:
                printf("undefined");
            break;
        }
    }

    putchar('\n');

}


void functionXor(struct CpuState *pstate, struct Line *pline){
   struct Variable args[LINE_ARGS_NUM] = {0};
    struct Variable *pargs[LINE_ARGS_NUM] = {0};

    // validation
    if (pline->argsnum != 2) {
        printf("Error - xor instruction requires two operands\n");
        return;
    }

    for (int i = 0; i < pline->argsnum; i++)
    {
        pargs[i] = &args[i];
        parseArgument(pstate, pline->arguments[i], &pargs[i]);
    }


    if (pargs[0]->isLiteral == true) {
        printf("Error - first operand must not be literal\n");
        return;
    }

    if (pargs[0]->type != INT) {
        printf("Error - first operand must be type of INT\n");
        return;
    }

    if (pargs[1]->type != INT) {
        printf("Error - second operand must be int\n");
        return;
    }

    pargs[0]->val.i = pargs[0]->val.i ^ pargs[1]->val.i;

}


void functionAnd(struct CpuState *pstate, struct Line *pline){
   struct Variable args[LINE_ARGS_NUM] = {0};
    struct Variable *pargs[LINE_ARGS_NUM] = {0};

    // validation
    if (pline->argsnum != 2) {
        printf("Error - and instruction requires two operands\n");
        return;
    }

    for (int i = 0; i < pline->argsnum; i++)
    {
        pargs[i] = &args[i];
        parseArgument(pstate, pline->arguments[i], &pargs[i]);
    }


    if (pargs[0]->isLiteral == true) {
        printf("Error - first operand must not be literal\n");
        return;
    }

    if (pargs[0]->type != INT) {
        printf("Error - first operand must be type of INT\n");
        return;
    }

    if (pargs[1]->type != INT) {
        printf("Error - second operand must be int\n");
        return;
    }

    pargs[0]->val.i = pargs[0]->val.i & pargs[1]->val.i;

}


void functionOr(struct CpuState *pstate, struct Line *pline){
   struct Variable args[LINE_ARGS_NUM] = {0};
    struct Variable *pargs[LINE_ARGS_NUM] = {0};

    // validation
    if (pline->argsnum != 2) {
        printf("Error - or instruction requires two operands\n");
        return;
    }

    for (int i = 0; i < pline->argsnum; i++)
    {
        pargs[i] = &args[i];
        parseArgument(pstate, pline->arguments[i], &pargs[i]);
    }


    if (pargs[0]->isLiteral == true) {
        printf("Error - first operand must not be literal\n");
        return;
    }

    if (pargs[0]->type != INT) {
        printf("Error - first operand must be type of INT\n");
        return;
    }

    if (pargs[1]->type != INT) {
        printf("Error - second operand must be int\n");
        return;
    }

    pargs[0]->val.i = pargs[0]->val.i | pargs[1]->val.i;

}

void functionNot(struct CpuState *pstate, struct Line *pline){
   struct Variable args[LINE_ARGS_NUM] = {0};
    struct Variable *pargs[LINE_ARGS_NUM] = {0};

    // validation
    if (pline->argsnum != 1) {
        printf("Error - not instruction requires single operand\n");
        return;
    }

    for (int i = 0; i < pline->argsnum; i++)
    {
        pargs[i] = &args[i];
        parseArgument(pstate, pline->arguments[i], &pargs[i]);
    }


    if (pargs[0]->isLiteral == true) {
        printf("Error - first operand must not be literal\n");
        return;
    }

    if (pargs[0]->type != INT) {
        printf("Error - first operand must be type of INT\n");
        return;
    }


    pargs[0]->val.i = ~pargs[0]->val.i;

}


void functionCreatePeriphery(struct CpuState *pstate, struct Line *pline) {

    // validation
    if (pline->argsnum != 2) {
        printf("Error - create instruction requires two parameters\n");
        return;
    }

    int i = 0;

    for (i = 0; i < pstate->peripherytypelistnumber; i++) {
        if (!strcmpi(pline->arguments[0], pstate->peripherytypelist[i].type_name)) {

                break;
        }
    }

    if (pstate->peripherytypelistnumber == i) {
        printf("Error - unrecognised periphery type %s\n", pline->arguments[0]);
        return;
    }


    pstate->peripherylist[pstate->peripherylistnumber].pType = &pstate->peripherytypelist[i];
    pstate->peripherylist[pstate->peripherylistnumber].pType->create(pstate, pline);
}

void functionSet(struct CpuState *pstate, struct Line *pline)
{

    // validation
    if (pline->argsnum < 2) {
        printf("Error - set instruction requires at least 2 parameters\n");
        return;
    }

    int i = 0;

    // find periphery name
    for (i = 0; i < pstate->peripherylistnumber; i++) {
        if (!strcmpi(pline->arguments[0], pstate->peripherylist[i].name)) {
                break;
        }
    }

    // periphery must exist (created with create instr) and then have options set
    if (pstate->peripherylistnumber == i) {
        printf("Error - unrecognised periphery name %s\n", pline->arguments[0]);
        return;
    }

    // based on periphery type (gpio) call set function
    pstate->peripherylist[i].pType->set(pstate, &pstate->peripherylist[i], pline);

    //printf("cpu set %s\n", pline->arguments[2]);

    //gpio_set(pstate, &pstate->peripherylist[i], pline);
/*
    if (pstate->peripherylist[i].numSetting == PERIPHERYSETTING_SIZE) {
        printf("Cannot add more than %d settings for a periphery\n", PERIPHERYSETTING_SIZE);
        return;
    }

    int j = 0;

    for (j = 0; j < PERIPHERYSETTING_SIZE; j++) {
        if (!strcmpi(pstate->peripherylist[i].settings->name, pline->arguments[1])) {
            break;
        }
    }

    if (j == PERIPHERYSETTING_SIZE) {
        j = pstate->peripherylist[i].numSetting;
        strcpy(pstate->peripherylist[i].settings[j].name, pline->arguments[1]);
    }

    strcpy(pstate->peripherylist[i].settings[j].data, pline->arguments[2]);

    pstate->peripherylist[i].numSetting++;
*/
    //pstate->peripherylist[pstate->peripherylistnumber].pType = &pstate->peripherytypelist[i];
    //pstate->peripherylist[pstate->peripherylistnumber].pType->create(pstate, pline);
}

void registerDefaultInstructions(struct CpuState *pstate)
{
    struct Instruction instr;

    instr.function = &functionMov;
    strcpy(instr.name, "mov");
    addInstruction(pstate, &instr);

    instr.function = &functionAdd;
    strcpy(instr.name, "add");
    addInstruction(pstate, &instr);

    instr.function = &functionSub;
    strcpy(instr.name, "sub");
    addInstruction(pstate, &instr);

    instr.function = &functionXor;
    strcpy(instr.name, "xor");
    addInstruction(pstate, &instr);

    instr.function = &functionAnd;
    strcpy(instr.name, "and");
    addInstruction(pstate, &instr);

    instr.function = &functionOr;
    strcpy(instr.name, "or");
    addInstruction(pstate, &instr);

    instr.function = &functionNot;
    strcpy(instr.name, "not");
    addInstruction(pstate, &instr);

    instr.function = &functionPrint;
    strcpy(instr.name, "print");
    addInstruction(pstate, &instr);

    // gpio

    instr.function = &functionCreatePeriphery;
    strcpy(instr.name, "create");
    addInstruction(pstate, &instr);

    instr.function = &functionSet;
    strcpy(instr.name, "set");
    addInstruction(pstate, &instr);

}


const char* getTypeName(enum EnumType t) {
    switch (t) {
        case INT:
            return "INT";
        case FLOAT:
            return "FLOAT";
        case BOOL:
            return "BOOL";
        default:
            return "UNDEFINED";
    }
}


void printVariable(struct Variable *v) {
    printf("variable name: %s\n", v->name);
    printf("var type: %s\n", getTypeName(v->type));
    printf("var value: ");
    switch (v->type) {
        case INT:
            printf("%d\n", v->val.i);
            break;
        case FLOAT:
            printf("%f\n", v->val.f);
            break;
        case BOOL:
            printf("%s\n", v->val.b ? "true" : "false");
            break;
        default:
            printf("UNDEFINED\n");
    }
}


void printCpuState(struct CpuState *pstate) {
    for (int i = 0; i < pstate->variablelistnumber; i++) {
        printVariable(&pstate->variablelist[i]);
    }

}

