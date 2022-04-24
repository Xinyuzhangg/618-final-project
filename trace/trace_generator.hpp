//
// Created by zhi lin on 4/11/22.
//

#ifndef INC_618_FINAL_PROJECT_TRACE_GENERATOR_H
#define INC_618_FINAL_PROJECT_TRACE_GENERATOR_H

#endif //INC_618_FINAL_PROJECT_TRACE_GENERATOR_H

void GenerateTrace(char *filename, int n, long long p, char* mode);

struct Trace{
    char op[4];
    long long key;
    int value;
};