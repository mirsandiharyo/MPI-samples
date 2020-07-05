#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define max_num 100

/* \brief A sample program that demonstrates how to check
 *  the MPI_Status object from an MPI_Recv call.
 */

int main(int argc, char** argv)
{
	int world_size, world_rank;
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

    int numbers[max_num], number_amount;
    if (world_rank == 0) {
    	/* Pick a random amount of integers to send to process one */
    	srand(time(NULL));
    	number_amount = (rand()/(float)RAND_MAX)*max_num;

    	/* Send the amount of integers to process one */
    	MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("Process 0 sent %d numbers to 1\n", number_amount);
    }
    else if (world_rank == 1) {
    	MPI_Status status;
    	/* Receive at most max_num from process zero */
    	MPI_Recv(numbers, max_num, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    	/* After receiving the message, check the status to determine how
    	 * many numbers were actually received */
    	MPI_Get_count(&status, MPI_INT, &number_amount);

    	/* Print the amount of numbers, and also print additional
    	 * information in the status object */
    	printf("Process 1 received %d numbers from 0. Message source = %d, tag = %d\n",
    			number_amount, status.MPI_SOURCE, status.MPI_TAG);
    }
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
