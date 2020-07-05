#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

/* \brief A sample program that demonstrates how to use MPI_Probe
 * to dynamically allocate received messages.
 */

#define max_num 1000

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
    	/* Probe for an incoming message from process zero */
    	MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

    	/* When probe returns, the status object has the size and other
    	 * attributes of the incoming message. Get the size of the message. */
    	MPI_Get_count(&status, MPI_INT, &number_amount);

    	/* Allocate a buffer just big enough to hold the incoming numbers */
    	int* number_buffer = (int*)malloc(sizeof(int)*number_amount);

    	/* Now receive the message with the allocated buffer */
		MPI_Recv(number_buffer, number_amount, MPI_INT, 0, 0, MPI_COMM_WORLD,
				 MPI_STATUS_IGNORE);
		printf("1 dynamically received %d numbers from 0.\n", number_amount);
		free(number_buffer);
	}

	MPI_Finalize();
	return 0;
}
