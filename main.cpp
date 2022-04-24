#include "mpi.h"
#include <algorithm>
#include "trace/trace_generator.hpp"
using namespace std;

int main() {
    int procID;
    int nproc;
    char *filename = NULL;
    int opt = 0;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    char* callingType = NULL;
    int n = 0;
    char* traceMode = NULL;
    int masterNum = 1;

    // Read command line arguments
    do {
        opt = getopt(argc, argv, "t:n:f:c:m:");
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
        if(traceMode==NULL){
            traceMode = "ModePutGet";
        }
        if(procID<masterNum) {
            GenerateTrace(filename, n, p, traceMode);
        }
        MPI_Finalize();
        return 0;
    }

    // test
    if(strcmp(callingType,"test")==0){
        /* Initailize additional data structures needed in the algorithm */
        time_t t;
        srand((unsigned) time(&t));

        // Get process rank
        MPI_Comm_rank(MPI_COMM_WORLD, &procID);

        // Get total number of processes specificed at start of run
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);

        if(procID<masterNum) {
            char *filename = (char *)malloc(30);
            sprintf(filename,"%d.txt",procID);

            FILE *input = fopen(traceFilePath, "r");
            if (!input) {
                printf("Unable to open file: %s.\n", filename);
            }

            int n;
            int tag = 0;
            fscanf(input,"%d\n",&n);
            vector<Request> requestList;
            ll key;
            int value;
            for(int i=0;i<n;i++){
                fscanf(input,"%s %lld %d\n",comm,&key,&value);
                Request r;
                strcpy(r.comm,comm,4);
                r.key = key;
                r.value = value;
                requestList.push_back(r);
            }

            // Run computation
            startTime = MPI_Wtime();
            if(procID<=masterNum){
                compute_hashMaster(procID, nproc, masterNum, true, char *traceList[])
            }else{
                compute_hashWorker(procID, nproc);
            }
            endTime = MPI_Wtime();
        }

        // Cleanup
        MPI_Finalize();
        return 0;
    }
}