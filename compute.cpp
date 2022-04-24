// linear hash
#include "include/rigtorp/HashMap.h"
#include "mpi.h"
#include "compute.h"
#inclue<vector>
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

// hashtable worker node
// async receive signal from master
void compute_hashWorker(int procID, int nproc){
    LinearHashWorker(procID,nproc);
}

// hashtable master node
void compute_hashMaster(int procID, int nproc,int nMaster, bool isBenchmark, std::vector<Request> traceList){
    LinearHashMaster(procID,int nproc,int nMaster,bool isBenchmark, traceList);
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
        void *resp = malloc(4);
        if(strcmp(r.comm,OPPut)==0){
            std::pair<rigtorp::iterator,bool> res = hm.emplace(r.key, r.value);
            int resInt = res.second?1:0;
            memcpy(resp,&resInt,4);
        }else if(strcmp(r.comm,OPGet)==0){
            int res = hm.at(r.key);
            memcpy(resp,&res,4);
        }else if(strcmp(r.comm,OPRmv)==0){
            hm.erase(r.key);
            int resInt = 1;
            memcpy(resp,&resInt,4);
        }
        // send back to source;
        MPI_Send(resp,4,MPI_INT,r.source,tag,MPI_COMM_WORLD);
    }
}

void LinearHashMaster(int procID, int nproc,int nMaster,bool isBenchmark, std::vector<Request> traceList){
    Hash modHash(1000003);
    Equal LLEqual;
    int tag = 0;
    int sourceID = 0;
    char *comm = malloc(20);
    int traceId = 0;
    int maxTraceId = traceList.size();
    while(1){
        Request r;
        if(isBenchmark){
            if(traceId == maxTraceId){
                break;
            }
            r = traceList[traceId];
            traceId++;
        }else{
            MPI_Recv(comm, 20, MPI_BYTE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            r = RequestDecoder(comm);
        }
        int callSource = r.source;
        // don't change the source, worker send back directly
        void *buf = malloc(20);
        RequestEncoder(buf,r.source,r.comm,r.key,r.value);
        int workerID = r.key%(nproc-nMaster)+nMaster;
        MPI_Request request;
        // MPI_Send(&workerID,1,MPI_INT,callSource,tag,MPI_COMM_WORLD); // return
        MPI_Send(buf,20,MPI_BYTE,workerID,tag,MPI_COMM_WORLD);
        free(buf);
    }
}

void BenchmarkTraceCaller(char* traceFilePath, int procID,int nproc, int nMaster){
    FILE *input = fopen(traceFilePath, "r");
    if (!input) {
        printf("Unable to open file: %s.\n", inputFilename);
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
}