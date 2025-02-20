// linear hash
#include "include/rigtorp/HashMap.h"
#include "include/flat_hash_map.hpp"
#include "mpi.h"
#include "compute.h"
#include <cstring>
#include <vector>
#include<iostream>

using namespace rigtorp;
#define ll long long
#define OPGet "GET"
#define OPPut "PUT"
#define OPRmv "RMV"

struct Hash {
    ll Mod;

    Hash() {

    }

    Hash(ll mod) {
        Mod = mod;
    }

    size_t operator()(ll v) {
        std::hash<int> hasher;
        return (size_t) hasher(v) % 1000003;
    }
};

struct Equal {
    bool operator()(ll l, ll r) { return l == r; }
};

/* hashtable worker node, async receive signal from master
 * @param masterNum: the number of the masters
 * @param hashType: type of hashing functions
 * */
void compute_hashWorker(int masterNum, char *hashType) {
    if (strcmp(hashType, "linear") == 0) {
        cout << "call linear" << endl;
        LinearHashWorker(masterNum);
    } else {
        cout << "call flat" << endl;
        FlatHashWorker(masterNum);
    }
}

/* hashtable master node, shared by any kinds of hashing function
 * @param: procID process id
 * @param: nproc number of processes
 * @param: nMaster number of masters
 * @param: traceList list of trace requests
 * */
void compute_hashMaster(int procID, int nproc, int nMaster, std::vector <Request> &traceList) {
    LinearHashMaster(procID, nproc, nMaster, traceList);
}

// parallel algorithm caller
void compute_caller(int procID, int nproc) {

}

/* decode a request
 * @param: req byte array
 * @return: request object decoded
 * */
Request RequestDecoder(char *req) {
    Request r;
    int cursor = 0;
    memcpy(&r.source, req + cursor, sizeof(int));
    //cout<<"decode source "<<r.source<<*(int*)req<<endl;
    cursor += sizeof(int);
    memcpy(&r.comm, req + cursor, 4);
    //cout<<"decode comm "<<r.comm<<endl;
    cursor += 4;
    memcpy(&r.key, req + cursor, sizeof(ll));
    //cout<<"decode key "<<r.key<<endl;
    cursor += sizeof(ll);
    memcpy(&r.value, req + cursor, sizeof(int));
    //cout<<"decode value"<<r.source<<endl;
    return r;
}

/* encode a request
 * @param: buf byte buffer for encoded request
 * @param: source source of the request
 * @param: comm command
 * @param: key key of the hash table
 * @param: value value of the object
 * */
void RequestEncoder(char *buf, int source, int comm, ll key, int value) {
    int cursor = 0;
    memcpy(buf + cursor, &source, sizeof(int));
    cursor += sizeof(int);
    memcpy(buf + cursor, &comm, 4);
    cursor += 4;
    memcpy(buf + cursor, &key, sizeof(ll));
    cursor += sizeof(ll);
    memcpy(buf + cursor, &value, sizeof(int));
}

/* linear hashing serial test
 * @param: traceList request list from the trace file
 * */
void LinearHashSerial(std::vector <Request> &traceList) {
    Hash modHash(1000003);
    Equal LLEqual;
    int tag = 0;
    MPI_Status status;
    HashMap < ll, int, decltype(modHash), decltype(LLEqual) > hm(16, 0);
    // void *r = malloc(20);
    char r[21];
    ll key;
    int value;
    int traceId = 0;
    int maxTraceId = traceList.size();
    while (1) {
        Request req;
        if (traceId == maxTraceId) {
            break;
        }
        req = traceList[traceId];
        traceId++;
        void *resp = malloc(4);
        if (req.comm == 0) {
            auto res = hm.emplace(req.key, req.value);
            int resInt = res.second ? 1 : 0;
            memcpy(resp, &resInt, 4);
        } else if (req.comm == 1) {
            try {
                int res = hm.at(req.key);
                memcpy(resp, &res, 4);
            }
            catch (const std::exception& e) {
            }

        } else if (req.comm == 2) {
            try {
                hm.erase(req.key);
                int resInt = 1;
                memcpy(resp, &resInt, 4);
            }
            catch (const std::exception& e) {
            }
        }
        free(resp);
    }
}

/* flat hash worker function
 * @param: masterNum: number of masters
 * */
void FlatHashWorker(int masterNum) {
    MPI_Status status;
    pp::flat_hash_map<ll, int> *fhm = new pp::flat_hash_map<ll, int>();
    char r[21];
    ll key;
    int value;
    int request_end = 0;
    Request *req = (Request *)malloc(sizeof(int) * 6);
    while (request_end < masterNum) {
        MPI_Recv(req, 6, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Request request;
        if (req->procID == -1) {
            request_end++;
            continue;
        }
        void *resp = malloc(4);
        if (req->comm == 0) {
            auto res = fhm->insert_or_assign(req->key, req->value);
            int resInt = res.second ? 1 : 0;
            memcpy(resp, &resInt, 4);
        } else if (req->comm == 1) {
            try {
                int res = fhm->at(req->key);
                memcpy(resp, &res, 4);
            }
            catch (const std::exception& e) {
            }
        } else if (req->comm == 2) {

        }
    }
}

/* linear hash worker function
 * @param: masterNum: number of masters
 * */
void LinearHashWorker(int masterNum) {
    Hash modHash(1000003);
    Equal LLEqual;
    int tag = 0;
    MPI_Status status;
    HashMap < ll, int, decltype(modHash), decltype(LLEqual) > hm(16, 0);
    // void *r = malloc(20);
    char r[21];
    ll key;
    int value;
    int request_end = 0;
    Request *req = (Request *)malloc(sizeof(int) * 6);
    while (request_end < masterNum) {
        MPI_Recv(req, 6, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
        MPI_Request request;
        if (req->procID == -1) {
            request_end++;
            continue;
        }
        //MPI_Irecv(recvBuf,5,MPI_INT,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD,&request);
        // if(recvBuf[0]>=0 && recvBuf[0]<=4 && id<50)printf("received 1 %d %d %d %d %d\n",recvBuf[0],recvBuf[1],recvBuf[2],recvBuf[3],recvBuf[4]);
        /*if(req.comm !=0){
            continue;
        }*/
        void *resp = malloc(4);
        if (req->comm == 0) {
            auto res = hm.emplace(req->key, req->value);
            int resInt = res.second ? 1 : 0;
            memcpy(resp, &resInt, 4);
        } else if (req->comm == 1) {
            int res = hm.at(req->key);
            memcpy(resp, &res, 4);
        } else if (req->comm == 2) {
            hm.erase(req->key);
            int resInt = 1;
            memcpy(resp, &resInt, 4);
        }
        // send back to source;
        // cout<<"send back to "<<req.source<<endl;
        // MPI_Send(resp,4,MPI_INT,req.source,tag,MPI_COMM_WORLD);
    }
}

/* linear hash master function
 * @param: masterNum: number of masters
 * @param: procID: process id
 * @param: nproc: number of processes
 * @param: traceList: list of requests from the trace file
 * */
void LinearHashMaster(int procID, int nproc, int nMaster, std::vector <Request> &traceList) {
    Hash modHash(1000003);
    Equal LLEqual;
    int sourceID = 0;
    char comm[21];
    int traceId = 0;
    MPI_Status status;
    int maxTraceId = traceList.size();
    MPI_Request request[100];
    int index = 0;
    printf("total master number %d i am %d\n", nMaster, procID);
    while (traceId < maxTraceId) {
        Request &r = traceList[traceId];
        traceId++;
        int workerID = r.key % (nproc - nMaster) + nMaster;
        MPI_Isend(&r, 6, MPI_INT, workerID, 0, MPI_COMM_WORLD, &request[index]);

        index++;
        if (index == 100) {
            index = 0;
            for (int i = 0; i < 100; i++) {
                MPI_Wait(&request[i], &status);
            }
        }
    }
    for (int workerID = nMaster; workerID < nproc; workerID++) {
        Request r;
        r.procID = -1;
        MPI_Send(&r, 6, MPI_INT, workerID, 0, MPI_COMM_WORLD);
    }
    printf("end of master\n");
}

/*
void BenchmarkTraceCaller(char* traceFilePath, int procID,int nproc, int nMaster){
    FILE *input = fopen(traceFilePath, "r");
    if (!input) {
        printf("Unable to open file: %s.\n", traceFilePath);
    }
    int n;
    int tag = 0;
    fscanf(input,"%d\n",&traceFilePath);
    char *comm = malloc(4);
    ll key;
    int value;
    char *req = malloc(20);
    for(int i=0;i<n;i++){
        fscanf(input,"%s %lld %d\n",comm,&key,&value);
        RequestEncoder(req,procID,comm,key,value);
        int masterID = rand()%nMaster+1;
        MPI_Send(req,20,MPI_BYTE,masterID,tag,MPI_COMM_WORLD);
        int res = 0;
        MPI_Recv(&res,1,MPI_INT,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD, &status);
    }
}*/