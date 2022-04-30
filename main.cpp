#include "mpi.h"
#include <algorithm>
#include "unistd.h"
#include "trace_generator.hpp"
#include "string.h"
#include "compute.h"
#include<iostream>
#include<cstdio>
#define ll long long
#include<vector>

using namespace std;

int main(int argc, char *argv[]) {
    int procID = 1;
    int nproc;
    char *filename = NULL;
    int opt = 0;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    char* callingType = NULL;
    int n = 0;
    char* traceMode = NULL;
    int masterNum = 1;
    char* hashType = NULL;

    double startTime;
    double endTime;

    // Read command line arguments
    do {
        opt = getopt(argc, argv, "t:n:f:c:m:h:");
        switch (opt) {
            case 't':
                callingType = optarg;
                break;

            case 'f':
                filename = optarg;
                break;

            case 'n':
                n = atoi(optarg);
                break;

            case 'c':
                traceMode = optarg;
                break;

            case 'm':
                masterNum = atoi(optarg);
                break;

            case 'h':
                hashType = optarg;
                break;

            case -1:
                break;

            default:
                break;
        }
    } while (opt != -1);

    if(callingType==NULL){
        printf("Should have calling type");
        MPI_Finalize();
        return -1;
    }

    // generate trace
    if(strcmp(callingType,"trace")==0){
        if(filename==NULL||n==0){
            printf("Should have file name and n");
            MPI_Finalize();
            return -1;
        }
        ll p = 1000003;
        printf("generate trace\n");
        if(traceMode==NULL){
            traceMode = "ModePutGet";
        }
        if(procID==1 || procID<masterNum) {
            printf("valid\n");
            GenerateTrace(filename, n, p, traceMode);
        }
        MPI_Finalize();
        return 0;
    }

    if(strcmp(callingType,"serial")==0) {
        time_t t;
        srand((unsigned) time(&t));

        printf("call %d\n", procID);
        char *filename = (char *) malloc(30);
        sprintf(filename, "%d.txt", procID);

        FILE *input = fopen(filename, "r");
        if (!input) {
            printf("Unable to open file: %s.\n", filename);
        }

        int n;
        int tag = 0;
        fscanf(input, "%d\n", &n);
        vector <Request> requestList(n);
        char comm[5];
        ll key;
        int value;
        for (int i = 0; i < n; i++) {
            fscanf(input, "%s %lld %d", comm, &key, &value);
            Request *r = new Request;
            if (strcmp(comm, "PUT") == 0) {
                r->comm = 0;
            } else if (strcmp(comm, "GET") == 0) {
                r->comm = 1;
            } else {
                r->comm = 2;
            }
            r->key = key;
            r->value = value;
            requestList[i] = *r;
        }
        startTime = MPI_Wtime();
        LinearHashSerial(requestList);
        endTime = MPI_Wtime();
        printf("running time: %lf\n",endTime-startTime);
        MPI_Finalize();
        return 0;
    }

    // test
    if(strcmp(callingType,"test")==0) {
        /* Initailize additional data structures needed in the algorithm */
        time_t t;
        srand((unsigned) time(&t));

        // Get process rank
        MPI_Comm_rank(MPI_COMM_WORLD, &procID);

        // Get total number of processes specificed at start of run
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);
        vector <Request> requestList;

        if (procID < masterNum) {
            printf("call %d\n",procID);
            char *filename = (char *) malloc(30);
            sprintf(filename, "trace/%d.txt", procID);

            FILE *input = fopen(filename, "r");
            if (!input) {
                printf("Unable to open file: %s.\n", filename);
            }

            int n;
            int tag = 0;
            fscanf(input, "%d\n", &n);
            char comm[5];
            ll key;
            int value;
            requestList.resize(n);
            for (int i = 0; i < n; i++) {
                fscanf(input, "%s %lld %d", comm, &key, &value);
                Request *r = new Request;
                //printf("tag1\n");
                if(strcmp(comm,"PUT")==0){
                    r->comm = 0;
                }else if (strcmp(comm,"GET")==0){
                    r->comm = 1;
                }else{
                    r->comm = 2;
                }
                r->key = key;
                //printf("tag3\n");
                r->value = value;
                //printf("%lld %d %s\n",r->key,r->value,r->comm);
                requestList[i] = *r;
                //printf("pushed\n");
            }
        }

        printf("this is %d\n",procID);
        // Run computation
        startTime = MPI_Wtime();
        if (procID < masterNum) {
            compute_hashMaster(procID, nproc, masterNum, true, requestList);
        } else {
            compute_hashWorker(procID, nproc,hashType);
        }
        endTime = MPI_Wtime();
        printf("running time: %lf\n",endTime-startTime);

        // Cleanup
        MPI_Finalize();
        return 0;
    }
}