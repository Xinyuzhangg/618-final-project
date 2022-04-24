//
// Created by zhi lin on 4/24/22.
//

#ifndef INC_618_FINAL_PROJECT_REQUEST_H
#define INC_618_FINAL_PROJECT_REQUEST_H

#endif //INC_618_FINAL_PROJECT_REQUEST_H

struct Request{
    char *comm;
    int source;
    long long key;
    int value;
    Request(){
        comm = (char *)malloc(4);
    }
    ~Request(){
        free(comm);
    }
};

void compute_hashWorker(int procID, int nproc);

// hashtable master node
void compute_hashMaster(int procID, int nproc,int nMaster, bool isBenchmark, char *traceList[]);