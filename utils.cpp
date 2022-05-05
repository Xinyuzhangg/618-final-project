#include<cstdio>
#include<cstdlib>
#include <time.h>
#include <ostream>
#include <fstream>
#include <sstream>

#include "utils.h"
#include "mpi.h"
#include "compute.h"
#include "unistd.h"
#include "trace_generator.hpp"

using namespace std;
void parser(int argc, char *argv[], progInfo *pi) {
    // Get process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &pi->procID);

    // Read command line arguments
    int opt = 0;
    do {
        opt = getopt(argc, argv, "t:n:f:c:m:h:d:");
        switch (opt) {
            case 'd':
                pi->dir = optarg;
                break;

            case 't':
                pi->callingType = optarg;
                break;

            case 'f':
                pi->filename = optarg;
                break;

            case 'n':
                pi->n = atoi(optarg);
                break;

            case 'c':
                pi->traceMode = optarg;
                break;

            case 'm':
                pi->masterNum = atoi(optarg);
                break;

            case 'h':
                pi->hashType = optarg;
                break;

            case -1:
                break;

            default:
                break;
        }
    } while (opt != -1);
}

int generate_trace(progInfo &pi) {
    int n = pi.n;
    if (pi.filename == NULL || n == 0) {
        printf("Should have file name and n");
        MPI_Finalize();
        return -1;
    }
    ll p = 1000003;
    printf("generate trace\n");
    if (pi.traceMode == NULL) {
        pi.traceMode = "ModePutGet";
    }
    if (pi.procID < pi.masterNum) {
        printf("valid\n");
        GenerateTrace(pi.filename, n, p, pi.traceMode);
    }
    MPI_Finalize();
    return 0;
}


int serial_test(progInfo &pi) {
    int procID = 1;
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
    double startTime = MPI_Wtime();
    LinearHashSerial(requestList);
    double endTime = MPI_Wtime();
    printf("running time: %lf\n", endTime - startTime);
    MPI_Finalize();
    return 0;
}

static void get_request(vector <Request> &requestList, FILE *input, int n, int procID) {
    Request *r;
    char comm[5];
    ll key;
    int value;
    for (int i = 0; i < n; i++) {
        fscanf(input, "%s %lld %d", comm, &key, &value);
        r = new Request;
        //printf("tag1\n");
        if (strcmp(comm, "PUT") == 0) {
            r->comm = 0;
        } else if (strcmp(comm, "GET") == 0) {
            r->comm = 1;
        } else {
            r->comm = 2;
        }
        r->procID = procID;
        r->key = key;
        r->value = value;
        requestList[i] = *r;
    }
}

static void write_result(bool worker, int id, double runtime, char *file_name) {
    fstream result_file;
    string result;

    std::stringstream ss;
    result_file.open (file_name, ios_base::app | ios_base::in);

    if (worker)
        ss << "worker: " << id << "\n" << "run time: " << runtime << "\n\n";
    else
        ss << "master: " << id << "\n" << "run time: " << runtime << "\n\n";
    result_file << ss.str();
    result_file.close();
}


int parallel_test(progInfo &pi) {
    /* Initailize additional data structures needed in the algorithm */
    int procID, nproc;

    time_t t;
    srand((unsigned) time(&t));

    // Get process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);

    // Get total number of processes specificed at start of run
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    vector <Request> requestList;
    if (procID < pi.masterNum) {
        printf("call %d\n", procID);
        char *filename = (char *) malloc(30);
        sprintf(filename, "trace/%s/%d.txt", pi.dir, procID);

        FILE *input = fopen(filename, "r");
        if (!input) {
            printf("Unable to open file: %s.\n", filename);
        }

        int n;
        fscanf(input, "%d\n", &n);
        requestList.resize(n);

        get_request(requestList, input, n, procID);
    }

    printf("this is process: %d\n", procID);
    // Run computation
    double startTime = MPI_Wtime();
    if (procID < pi.masterNum) {
        compute_hashMaster(procID, nproc, pi.masterNum, requestList);
    } else {
        compute_hashWorker(pi.masterNum, pi.hashType);
    }
    double endTime = MPI_Wtime();
    double runtime = endTime - startTime;
    printf("running time: %lf\n", endTime - startTime);

    char *result_file = (char *)malloc(30);
    sprintf(result_file, "result/%s/%s&%dm&%dw.txt", pi.dir, pi.callingType, pi.masterNum, nproc - pi.masterNum);


    if (procID < pi.masterNum) {
        write_result(false, procID, runtime, result_file);
    } else {
        write_result(true, procID - pi.masterNum, runtime, result_file);
    }
    free(result_file);

    // Cleanup
    MPI_Finalize();
    return 0;
}
