//
// Created by zhi lin on 4/24/22.
//

#include<vector>
using namespace std;

#ifndef INC_618_FINAL_PROJECT_REQUEST_H
#define INC_618_FINAL_PROJECT_REQUEST_H

#endif //INC_618_FINAL_PROJECT_REQUEST_H

struct Request{
    int comm;
    int source;
    long long key;
    int value;
    Request(){
    }
    ~Request(){
    }
};

void compute_hashWorker(int procID, int nproc,char* hashType);

// hashtable master node
void compute_hashMaster(int procID, int nproc,int nMaster, bool isBenchmark, std::vector<Request> &traceList);

void LinearHashSerial(std::vector<Request> &traceList);

void LinearHashWorker(int procID, int nproc);

void LinearHashMaster(int procID, int nproc,int nMaster,bool isBenchmark, std::vector<Request> &traceList);

Request RequestDecoder(void *req);

void RequestEncoder(void *buf, int source, char* comm, long long key, int value);

void FlatHashWorker(int procID, int nproc);