// linear hash
#include "include/rigtorp/HashMap.h"
#include "mpi.h"
#define ll long long
#define OPGet = "GET"
#define OPPut = "PUT"
#define OPRmv = "RMV"

struct Hash {
    ll Mod;
    Hash(ll mod){
        Mod = mod;
    }
    size_t operator()(ll v) {
        return (size_t)hasher(v)%Mod;
    }
};

struct Equal {
    bool operator()(ll l, ll r) { return l == r; }
};

struct Request{
    char *comm;
    int source;
    ll key;
    int value;
    Request(){
        comm = (char *)malloc(4);
    }
    ~Request(){
        free(comm);
    }
};

// hashtable worker node
// async receive signal from master
void compute_hashWorker(int procID, int nproc){

}

// hashtable master node
void compute_hashMaster(int procID, int nproc){

}

// parallel algorithm caller
void compute_caller(int procID, int nproc){

}

Request RequestDecoder(void *req){
    Request r;
    int cursor = 0;
    memcpy(&r.source,req+cursor,sizeof(int));
    cursor+=sizeof(int);
    memcpy(r.comm,req+cursor,4);
    cursor+=4;
    memcpy(&r.key,req+cursor,sizeof(ll));
    cursor+=sizeof(ll);
    memcpy(&r.value,req+cursor,sizeof(int));
    return r;
}

void RequestEncoder(void *buf, int source, char* comm, ll key, int value){
    void *buf = malloc(20);
    int cursor = 0;
    memcpy(buf+cursor,&source,sizeof(int));
    cursor+=sizeof(int);
    memcpy(buf+cursor,comm,4);
    cursor+=4;
    memcpy(buf+cursor,&key,sizeof(ll));
    cursor+=sizeof(ll);
    memcpy(buf+cursor, &value, sizeof(int));
}

void LinearHashWorker(int procID, int nproc){
    Hash modHash(1000003);
    Equal LLEqual;
    int tag = 0;
    MPI_Status status;
    HashMap<ll, int, decltype(modHash), decltype(LLEqual)> hm(16,0);
    void *r = malloc(20);
    ll key;
    int value;
    while(1){
        MPI_Recv(r,20,MPI_BYTE,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD,&status);
        Request r = RequestDecoder(r);
        if(strcmp(r.comm,OPPut)==0){
            std::pair<rigtorp::iterator,bool> res = hm.emplace(r.key, r.value);
        }else if(strcmp(r.comm,OPGet)==0){
            long long res = hm.at(r.key);
        }else if(strcmp(r.comm,OPRmv)==0){
            hm.erase(r.key);
        }
    }
}

void LinearHashMaster(int procID, int nproc,int nMaster){
    Hash modHash(1000003);
    Equal LLEqual;
    int tag = 0;
    int sourceID = 0;
    char *comm = malloc(20);
    while(1){
        MPI_Recv(comm,20,MPI_BYTE,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD,&status);
        Request r = RequestDecoder(comm);
        int callSource = r.source;
        // don't change the source;
        void *buf = malloc(20);
        RequestEncoder(buf,r.source,r.comm,r.key,r.value);
        int workerID = r.key%(nproc-nMaster)+nMaster;
        MPI_Request request;
        MPI_Send(&workerID,1,MPI_INT,callSource,tag,MPI_COMM_WORLD); // return
        MPI_Send(buf,20,MPI_BYTE,workerID,tag,MPI_COMM_WORLD);
        free(buf);
    }
}

