#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* \brief A sample program that demonstrates how to use
 * send and receive in MPI.
*/

#define PING 1
#define PONG 2

int main(int argc, char **argv)
{
	int iters = 1000;
    int i, world_size, world_rank, data_size;
    int min_size = 1, max_size = 32768;
    double start;
    char *progname;
    progname = argv[0];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_size != 2) {
        fprintf(stderr, "%s must be run on exactly 2 processors.\n", progname);
        MPI_Finalize();
        return 1;
    }

    char *dummy;
    dummy = malloc(max_size);
    for (data_size = min_size; data_size <= max_size; data_size *= 2) {
        start = MPI_Wtime();
        for (i = 0; i < iters; i++) {
            MPI_Status s;

            /* PING */
            if (world_rank == 0) {
                MPI_Send(dummy, data_size, MPI_CHAR, 1, PING, MPI_COMM_WORLD);
            }
            else {
                MPI_Recv(dummy, data_size, MPI_CHAR, 0, PING, MPI_COMM_WORLD, &s);
            }

            /* PONG */
            if (world_rank == 1) {
                MPI_Send(dummy, data_size, MPI_CHAR, 0, PONG, MPI_COMM_WORLD);
            }
            else {
                MPI_Recv(dummy, data_size, MPI_CHAR, 1, PONG, MPI_COMM_WORLD, &s);
            }
        }
        if (world_rank == 0) {
            printf("%d %d byte pingpongs took %f seconds.\n", iters,
                data_size, MPI_Wtime()-start);
        }
    }
    MPI_Finalize();
    return 0;
}

