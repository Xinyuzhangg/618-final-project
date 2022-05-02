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
    Hash(){

    }
    Hash(ll mod){
        Mod = mod;
    }
    size_t operator()(ll v) {
        std::hash<int> hasher;
        return (size_t)hasher(v)%1000003;
    }
};

struct Equal {
    bool operator()(ll l, ll r) { return l == r; }
};

// hashtable worker node
// async receive signal from master
void compute_hashWorker(int procID, int nproc,char* hashType){
    if(strcmp(hashType,"linear")==0) {
        cout<<"call linear"<<endl;
        LinearHashWorker(procID, nproc);
    }else{
        cout<<"call flat"<<endl;
        FlatHashWorker(procID,nproc);
    }
}

// hashtable master node
void compute_hashMaster(int procID, int nproc,int nMaster, bool isBenchmark, std::vector<Request> &traceList){
    LinearHashMaster(procID,nproc,nMaster,isBenchmark,traceList);
}

// parallel algorithm caller
void compute_caller(int procID, int nproc){

}

Request RequestDecoder(char *req){
    Request r;
    int cursor = 0;
    memcpy(&r.source,req+cursor,sizeof(int));
    //cout<<"decode source "<<r.source<<*(int*)req<<endl;
    cursor+=sizeof(int);
    memcpy(&r.comm,req+cursor,4);
    //cout<<"decode comm "<<r.comm<<endl;
    cursor+=4;
    memcpy(&r.key,req+cursor,sizeof(ll));
    //cout<<"decode key "<<r.key<<endl;
    cursor+=sizeof(ll);
    memcpy(&r.value,req+cursor,sizeof(int));
    //cout<<"decode value"<<r.source<<endl;
    return r;
}

void RequestEncoder(char *buf, int source, int comm, ll key, int value){
    int cursor = 0;
    memcpy(buf+cursor,&source,sizeof(int));
    cursor+=sizeof(int);
    memcpy(buf+cursor,&comm,4);
    cursor+=4;
    memcpy(buf+cursor,&key,sizeof(ll));
    cursor+=sizeof(ll);
    memcpy(buf+cursor, &value, sizeof(int));
}

void LinearHashSerial(std::vector<Request> &traceList){
    Hash modHash(1000003);
    Equal LLEqual;
    int tag = 0;
    MPI_Status status;
    HashMap<ll, int, decltype(modHash), decltype(LLEqual)> hm(16,0);
    // void *r = malloc(20);
    char r[21];
    ll key;
    int value;
    int traceId = 0;
    int maxTraceId = traceList.size();
    while(1) {
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
            int res = hm.at(req.key);
            memcpy(resp, &res, 4);
        } else if (req.comm == 2) {
            hm.erase(req.key);
            int resInt = 1;
            memcpy(resp, &resInt, 4);
        }
        free(resp);
    }
}

void FlatHashWorker(int procID, int nproc) {
    int tag = 0;
    MPI_Status status;
    pp::flat_hash_map<ll, int> *fhm = new pp::flat_hash_map<ll, int>();
    // void *r = malloc(20);
    char r[21];
    ll key;
    int value;
    while (1) {
        Request req;
        int recvBuf[5];
        MPI_Recv(recvBuf,5,MPI_INT,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD,&status);
        MPI_Request request;
        if(recvBuf[0]==0 && recvBuf[1]==0)continue;
        req.source = recvBuf[0];
        req.key = ((ll)(recvBuf[1])<<32) + (ll)recvBuf[2];
        req.value = recvBuf[3];
        req.comm = recvBuf[4];
        void *resp = malloc(4);
        /*if(req.comm !=0){
            continue;
        }*/
        if (req.comm == 0) {
            auto res = fhm->insert_or_assign(req.key, req.value);
            int resInt = res.second ? 1 : 0;
            memcpy(resp, &resInt, 4);
        } else if (req.comm == 1) {
            int res = fhm->at(req.key);
            memcpy(resp, &res, 4);
        } else if (req.comm == 2) {

        }
        // MPI_Send(resp, 4, MPI_INT, req.source, tag, MPI_COMM_WORLD);
    }
}


void LinearHashWorker(int procID, int nproc){
    Hash modHash(1000003);
    Equal LLEqual;
    int tag = 0;
    MPI_Status status;
    HashMap<ll, int, decltype(modHash), decltype(LLEqual)> hm(16,0);
    // void *r = malloc(20);
    char r[21];
    ll key;
    int value;
    int id = 0;
    while(1){
        // MPI_Recv(r,20,MPI_BYTE,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD,&status);
        // MPI_Recv(r,20,MPI_BYTE,0,tag,MPI_COMM_WORLD,&status);
        // Request req = RequestDecoder(r);
        Request req;
        int recvBuf[5];
        MPI_Recv(recvBuf,5,MPI_INT,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD,&status);
        MPI_Request request;
        //MPI_Irecv(recvBuf,5,MPI_INT,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD,&request);
        // if(recvBuf[0]>=0 && recvBuf[0]<=4 && id<50)printf("received 1 %d %d %d %d %d\n",recvBuf[0],recvBuf[1],recvBuf[2],recvBuf[3],recvBuf[4]);
        id++;
        if(recvBuf[0]==0 && recvBuf[1]==0)continue;
        // printf("received 2 %d %d %d %d %d\n",recvBuf[0],recvBuf[1],recvBuf[2],recvBuf[3],recvBuf[4]);
        req.source = recvBuf[0];
        req.key = ((ll)(recvBuf[1])<<32) + (ll)recvBuf[2];
        req.value = recvBuf[3];
        req.comm = recvBuf[4];
        //printf("received a message %lld %d %d\n",req.key,req.value,req.comm);
        /*if(req.comm !=0){
            continue;
        }*/
        void *resp = malloc(4);
        if(req.comm==0){
            auto res = hm.emplace(req.key, req.value);
            int resInt = res.second?1:0;
            memcpy(resp,&resInt,4);
        }else if(req.comm==1){
            int res = hm.at(req.key);
            memcpy(resp,&res,4);
        }else if(req.comm==2){
            hm.erase(req.key);
            int resInt = 1;
            memcpy(resp,&resInt,4);
        }
        // send back to source;
        // cout<<"send back to "<<req.source<<endl;
        // MPI_Send(resp,4,MPI_INT,req.source,tag,MPI_COMM_WORLD);
    }
}

void LinearHashMaster(int procID, int nproc,int nMaster,bool isBenchmark, std::vector<Request> &traceList){
    Hash modHash(1000003);
    Equal LLEqual;
    int tag = 0;
    int sourceID = 0;
    char comm[21];
    int traceId = 0;
    MPI_Status status;
    int maxTraceId = traceList.size();
    MPI_Request request[100];
    int index = 0;
    int ctn = 0;
    printf("total master number %d i am %d\n",nMaster,procID);
    while(1){
        Request r;
        if(isBenchmark){
            if(traceId == maxTraceId){
                break;
            }
            r = traceList[traceId];
            traceId++;
        }else{
            // MPI_Irecv(comm, 20, MPI_BYTE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            // r = RequestDecoder(comm);
        }
        int callSource = r.source;
        // don't change the source, worker send back directly
        char buf[20];
        RequestEncoder(buf,r.source,r.comm,r.key,r.value);
        int workerID = r.key%(nproc-nMaster)+nMaster;
        // MPI_Send(&workerID,1,MPI_INT,callSource,tag,MPI_COMM_WORLD); // return
        // MPI_Send(buf,20,MPI_BYTE,workerID,tag,MPI_COMM_WORLD);
        // printf("this is %d %lld %d %d\n",procID,r.key,r.value,r.comm);
        int sendBuf[5];
        sendBuf[0] = procID;
        sendBuf[1] = int(r.key>>32);
        sendBuf[2] = int(r.key&0xffffffff);
        sendBuf[3] = r.value;
        sendBuf[4] = r.comm;
        // if(ctn<20) printf("sent %d %d %d %d %d\n",sendBuf[0],sendBuf[1],sendBuf[2],sendBuf[3],sendBuf[4]);
        ctn++;
        //MPI_Send(sendBuf,5,MPI_INT,workerID,tag,MPI_COMM_WORLD);
        MPI_Isend(sendBuf,5,MPI_INT,workerID,tag,MPI_COMM_WORLD,&request[index]);
        index++;
        if(index==100){
            index = 0;
            for(int i=0;i<100;i++){
                MPI_Wait(&request[i],&status);
            }
        }
        //free(buf);
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