#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "parse.h"
#include <LittleFS.h>




#define MAX_LINE_LEN 100
char actualLine[MAX_LINE_LEN];

void initLine(struct Line *pline) {
    pline->argsnum = 0;
    memset(pline->label, 0, sizeof(pline->label));
    memset(pline->instruction, 0, sizeof(pline->instruction));
    memset(pline->arguments, 0, sizeof(pline->arguments));
}


void tokenizeLine(char *rawLine, struct Line *pline) {
    if (rawLine == NULL)
        return;

    // reset pline
    initLine(pline);

    for (char *p = rawLine; *p != '\0'; p++) {
        *p = tolower(*p);
    }

    // first token break by space or tab
    const char *tokens[LINE_ARGS_NUM + 2]; // label + instr + maxargs
    const char *delims = " \t\r\n";
    char *s = strtok(rawLine, delims);

    // no tokens found
    if (s == NULL || strlen(s) == 0) {
        return;
    }

    // tokenize the rest of the line
    for (int i = 0; s != NULL && i < LINE_ARGS_NUM + 2; i++)  {
        tokens[i] = s;
        s = strtok(NULL, delims);
        pline->argsnum++;
    }

    // check label first
    bool hasLabel = false;
    if (tokens[0][strlen(tokens[0])-1] == ':') {
        strncpy(pline->label, tokens[0], 50);
        pline->argsnum--;
        hasLabel = true;
    }

    strncpy(pline->instruction, tokens[(int)hasLabel], 50);
    pline->argsnum--;

    for (int j = 0; j < pline->argsnum; j++) {
        strcpy(pline->arguments[j], tokens[hasLabel + j + 1]);
    }
}


char *readAtLine_old(char filename[], int number) {
    FILE* f = fopen(filename, "r");

    printf("opening file %s\n", filename);
    
    if (f == NULL) {
        printf("error: cannot open %s file\n", filename);
        return NULL;
    }

    if (number < 0) {
        return NULL;
    }

    const int MAX_LINE = 512;
    char *buff = (char*)malloc(MAX_LINE * sizeof(char));
    int c = 0;
    bool found = false;

    while (fgets(buff, MAX_LINE, f) != NULL) {

        if (c == number) {
            found = true;
            break;
        }

        c++;
    }


    if (found == false) {
        free (buff);
        buff = NULL;
    }

    fclose(f);
    return buff;
}


char *readAtLine(char filename[], int number) {
    
    if(!LittleFS.begin()){
        Serial.println("An Error has occurred while mounting LittleFS");
    return NULL;
  }

    File file = LittleFS.open(filename, "r");
    if(!file){
        Serial.println("readAtLine: Failed to open file for reading");
        return NULL;
    }

    if (number < 0) {
        return NULL;
    }

    int c = 0;
    bool found = false;
    String s;

    while (file.available()) {

        if (c == number) {
            found = true;
            break;
        }

        s = file.readStringUntil('\n');
        //Serial.println("line: " + s);

        c++;

    }

    if (found == true) {
        char *p = new char[s.length()+1];
        strcpy(p, s.c_str());
        return p;
    }
    file.close();
    LittleFS.end();
    return NULL;
}

bool appendLine(const char filename[512], const char line[512]) {

    if(!LittleFS.begin()){
        Serial.println("An Error has occurred while mounting LittleFS");
        return false;
  }

    File file = LittleFS.open(filename, "a");
    if(!file){
        Serial.println("appendLine: Failed to open file for writing");
        return false;
    }

    char data[512];
    strcpy(data, "\r\n");
    strcat(data, line);
    strcat(data, "\r\n");

    file.println(line);
    file.flush();
    file.close();
    LittleFS.end();
    return true;

}

bool readLinex(char filename[], int number, struct Line *pline)
{
    char *rawLine = readAtLine(filename, number);

    if (rawLine == NULL) {
        //rawLine = readAtLine(filename, 0);
        return false;
    }
    //
    tokenizeLine(rawLine, pline);
    return true;
}


char *readFilex(const char *filename) {

    if(!LittleFS.begin()){
        Serial.println("An Error has occurred while mounting LittleFS");
        return NULL;
  }

    File file = LittleFS.open(filename, "r");
    if(!file){
        Serial.println("readAtLine: Failed to open file for reading");
        return NULL;
    }


    String s;

    while (file.available()) {
        s += file.readString() + "\n";
    }


    file.close();
    LittleFS.end();

    char *str;
    str = strdup(s.c_str());
    return str;
}

void printFile(const char *filename) {

    if(!LittleFS.begin()){
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
  }

    File file = LittleFS.open(filename, "r");
    if(!file){
        Serial.println("readAtLine: Failed to open file for reading");
        return;
    }


    String s;

    Serial.println("============START OF FILE===========================");
    while (file.available()) {

        s = file.readStringUntil('\n');
        Serial.println(s);

    }

    Serial.println("===============EOF========================");

    file.close();
    LittleFS.end();
}

void printLine(struct Line *pline) {
    printf("Label: %s\n", pline->label);
    printf("Ins: %s\n", pline->instruction);
    printf("arg num: %d\n", pline->argsnum);
    printf("args: ");
    for (int i = 0; i < pline->argsnum; i++)
        printf("%s ", pline->arguments[i]);
    putchar('\n');
}
