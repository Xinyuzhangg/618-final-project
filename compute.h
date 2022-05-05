#pragma once
#include<vector>
#include "utils.h"
#define ll long long
using namespace std;

struct Request {
    int comm;
    int source;
    int procID;
    long long key;
    int value;
    Request(){
    }
    ~Request(){
    }
};

void compute_hashWorker(int masterNum, char *hashType);

// hashtable master node
void compute_hashMaster(int procID, int nproc, int nMaster, std::vector <Request> &traceList);

void LinearHashSerial(std::vector <Request> &traceList);

void LinearHashWorker(int masterNum);

void LinearHashMaster(int procID, int nproc, int nMaster, std::vector <Request> &traceList);

Request RequestDecoder(void *req);

void RequestEncoder(void *buf, int source, char *comm, long long key, int value);

void FlatHashWorker(int masterNum);