#include<iostream>
#include <algorithm>
#include<cstdio>
#include<vector>

#include "mpi.h"
#include "compute.h"
#include "utils.h"


using namespace std;


int main(int argc, char *argv[]) {
    int nproc;
    char *filename = NULL;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    progInfo pi;
    parser(argc, argv, &pi);

    if (pi.callingType == NULL) {
        printf("Should have calling type");
        MPI_Finalize();
        return -1;
    }

    if (strcmp(pi.callingType, "trace") == 0) {
        // Generate trace
        return generate_trace(pi);
    } else if (strcmp(pi.callingType, "serial") == 0) {
        return serial_test(pi);
    } else if (strcmp(pi.callingType, "parallel") == 0) {
        return parallel_test(pi);
    }
}