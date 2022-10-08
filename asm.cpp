#include <cstdio>
#include <string.h>
#include "asm.h"
#ifndef functionList
    #include "functionList.h"
#endif

const char  *SIGNATURE        =        "CP"      ;
const int    ASM_VERSION      =         3        ;

int stackAsmTex(Lines *commandList, FILE *outStream) {
    catchNullptr(commandList);
    catchNullptr(outStream);

    fprintf(outStream, "%s %d %zu\n", SIGNATURE, ASM_VERSION, commandList -> numberOfLines);
    for (size_t currentCommand = 0; currentCommand < commandList -> numberOfLines; ++currentCommand) {
        if (!strcmpi(commandList -> array[currentCommand].line, "push")) {
            int value = 0;
            ++currentCommand;
            sscanf(commandList -> array[currentCommand].line, "%d", &value);
            fprintf(outStream, "%d %d\n", PUSH, value);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "in")) {
            int value = 0;
            ++currentCommand;
            sscanf(commandList -> array[currentCommand].line, "%d", &value);
            fprintf(outStream, "%d %d\n", IN, value);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "pop")) {
            fprintf(outStream, "%d\n", POP);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "halt")) {
            fprintf(outStream, "%d\n", HALT);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "add")) {
            fprintf(outStream, "%d\n", ADD);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "div")) {
            fprintf(outStream, "%d\n", DIV);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "out")) {
            fprintf(outStream, "%d\n", OUT);
        } else if (!strcmpi(commandList -> array[currentCommand].line, "product")) {
            fprintf(outStream, "%d\n", PRODUCT);
        }
    }

    return OK;
}

int argDefenition(Line *args, char **cmdArgs, size_t *dataSize, char command);

int stackAsmBin(Lines *commandList, FILE *outStream) {
    catchNullptr(commandList);
    catchNullptr(outStream);

    char *outputData  = (char *) calloc(commandList->numberOfLines + 3, sizeof(int));
    char *currentElem = outputData;
    size_t dataSize  = 3 * sizeof(int);

     *((int *) currentElem) = 'C' * 256 + 'P';
    currentElem += sizeof(int);
     *currentElem = ASM_VERSION;
    currentElem += sizeof(int);
    char *sizeOfData = currentElem;
    currentElem += sizeof(int);

    for (size_t currentCommand = 0; currentCommand < commandList -> numberOfLines; ++currentCommand) {
        if (!strcmpi(commandList -> array[currentCommand].line, "push")) {
            size_t sizeOfCmd = 0;
            argDefenition(&(commandList -> array[currentCommand]), &currentElem, &sizeOfCmd, PUSH);
              dataSize  += sizeOfCmd;
            currentElem += sizeOfCmd;
        }  else if (!strcmpi(commandList -> array[currentCommand].line, "pop")) {
            size_t sizeOfCmd = 0;
            argDefenition(&(commandList -> array[currentCommand]), &currentElem, &sizeOfCmd, POP);
            currentElem += sizeOfCmd;
              dataSize  += sizeOfCmd;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "halt")) {
            *currentElem = HALT;
            ++currentElem;
            ++dataSize;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "add")) {
            *currentElem = ADD;
            ++currentElem;
            ++dataSize;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "div")) {
            *currentElem = DIV;
            ++currentElem;
            ++dataSize;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "out")) {
            *currentElem = OUT;
            ++currentElem;
            ++dataSize;
        } else if (!strcmpi(commandList -> array[currentCommand].line, "product")) {
            *currentElem = PRODUCT;
            ++currentElem;
            ++dataSize;
        }
    }
    *sizeOfData = dataSize;

    fwrite(outputData, sizeof(char), dataSize, outStream);

    return OK;
}

int argDefenition(Line *args, char **cmdArgs, size_t *sizeData, char command) {
    catchNullptr(args);

    char   cmd   =    command  ;
    char  RegArg =       0     ;
    int   NumArg =       0     ;
    *sizeData = sizeof(char);

    char *currentPart = strtok(args -> line, "]+ ");
    while (currentPart != nullptr) {
        if (*currentPart == '[')
            cmd |= TypeRAM;
        currentPart++;
        if (stricmp("rax", currentPart)) {
            cmd |= TypeReg;
            RegArg = 1;
            *sizeData += sizeof(char);
        } else if (stricmp("rbx", currentPart)) {
            cmd |= TypeReg;
            RegArg = 2;
            *sizeData += sizeof(char);
        } else if (stricmp("rcx", currentPart)) {
            cmd |= TypeReg;
            RegArg = 3;
            *sizeData += sizeof(char);
        } else if (stricmp("rdx", currentPart)) {
            cmd |= TypeReg;
            RegArg = 4;
            *sizeData += sizeof(char);
        } else {
            cmd |= TypeNum;
            NumArg = strtol(currentPart, &currentPart, 10);
            *sizeData += sizeof(int);
        }
        currentPart = strtok(nullptr, "]+ ");
    }

    *cmdArgs = (char *) calloc(*sizeData, sizeof(char));
    catchNullptr(*cmdArgs);

    char *curArg = *cmdArgs;
    *curArg = cmd;
    curArg++;
    if (cmd & TypeReg) {
        *curArg = RegArg;
        curArg++;
    }
    if (cmd & TypeNum)
        *curArg = NumArg;

    return OK;

}