#include "mpi.h"
#include <algorithm>
using namespace std;

int main() {
    int procID;
    int nproc;
    char *inputFilename = NULL;
    int opt = 0;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Read command line arguments
    do {
        opt = getopt(argc, argv, "f:");
        switch (opt) {
            case 'f':
                inputFilename = optarg;
                break;

            case -1:
                break;

            default:
                break;
        }
    } while (opt != -1);

    if (inputFilename == NULL) {
        printf("Usage: %s -f <filename> [-p <P>] [-i <N_iters>]\n", argv[0]);
        MPI_Finalize();
        return -1;
    }
    readInput(inputFilename);

    /* Initailize additional data structures needed in the algorithm */
    time_t t;
    srand((unsigned) time(&t));

    // Get process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &procID);

    // Get total number of processes specificed at start of run
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    // Run computation
    startTime = MPI_Wtime();
    compute(procID, nproc, inputFilename);
    endTime = MPI_Wtime();

    // Cleanup
    MPI_Finalize();
}