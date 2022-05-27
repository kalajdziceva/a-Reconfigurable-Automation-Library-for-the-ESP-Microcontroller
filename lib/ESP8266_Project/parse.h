#ifndef PARSE_H
#define PARSE_H

#include <stdbool.h>



#define LINE_CHAR_NUM 50
#define LINE_ARGS_NUM 10
#define NAME_TYPE_SIZE 10
#define INSTRUCTIONLIST_SIZE 30
#define VARIABLELIST_SIZE 100

struct Line;

// funcs declarations

char *readAtLine(char filename[], int number);

bool appendLine(const char filename[512], const char line[512]);
bool readLinex(char filename[], int number, struct Line *pline);
void printLine(struct Line *pline);
void printFile(const char *filename);
char* readFilex(const char *filename);
void tokenizeLine(char *rawLine, struct Line *pline);

struct Line
{
    char label[LINE_CHAR_NUM];
    char instruction[LINE_CHAR_NUM];
    char arguments[LINE_ARGS_NUM][LINE_CHAR_NUM];
    int argsnum;
};

#endif