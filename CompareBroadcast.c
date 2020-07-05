#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

/* \brief A sample program that demonstrates the comparison of
 * MPI_Bcast with the my_bcast function
 */

void my_broadcast(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator);

int main(int argc, char** argv)
{
	int num_elements = 100;
	int num_trials = 1000;
	int world_size, world_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	double total_my_bcast_time = 0.0;
	double total_mpi_bcast_time = 0.0;
	int i;
	int* data = (int*)malloc(sizeof(int)*num_elements);
	assert(data != NULL);

	for (i = 0; i < num_trials; i++) {
		/* Time my_bcast */
		/* Synchronize before starting timing */
		MPI_Barrier(MPI_COMM_WORLD);
		total_my_bcast_time -= MPI_Wtime();
		my_broadcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
		/* Synchronize again before obtaining final time */
		MPI_Barrier(MPI_COMM_WORLD);
		total_my_bcast_time += MPI_Wtime();

		/* Time MPI_Bcast */
		MPI_Barrier(MPI_COMM_WORLD);
		total_mpi_bcast_time -= MPI_Wtime();
		MPI_Bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		total_mpi_bcast_time += MPI_Wtime();
	}

	/* Print the timing information */
	if (world_rank == 0) {
		printf("Data size = %d, number of trials = %d\n", num_elements * (int)sizeof(int),
			   num_trials);
		printf("Average my_bcast time = %.3e s\n", total_my_bcast_time/num_trials);
		printf("Average MPI_Bcast time = %.3e s\n", total_mpi_bcast_time/num_trials);
	}
	free(data);
	MPI_Finalize();
	return 0;
}

void my_broadcast(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator)
{
	int world_size, world_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if (world_rank == root) {
		/* If root process, send the data to everyone */
		int i;
		for (i = 0; i < world_size; i++) {
			if (i != world_rank) {
				MPI_Send(data, count, datatype, i, 0, communicator);
			}
		}
	}
	else {
		/* If receiver process, receive the data from the root */
		MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
	}
}
