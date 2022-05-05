#pragma once
#include "string.h"

struct progInfo {
    char *callingType;
    char *filename;
    int n;
    char *traceMode;
    int masterNum;
    char *hashType;
    int procID;
};


void parser(int argc, char *argv[], progInfo *pi);
int generate_trace(progInfo &pi);
int serial_test(progInfo &pi);
int parallel_test(progInfo &pi);