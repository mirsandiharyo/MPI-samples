#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* \brief A sample program that demonstrates how to use broadcast
 * using MPI_Send and MPI_Recv
 */

void my_broadcast(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator);

int main(int argc, char** argv)
{
	int world_size, world_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int data = 100;
	if (world_rank == 0) {
		printf("Process 0 broadcasting data %d\n", data);
		my_broadcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}
	else {
		my_broadcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
		printf("Process %d received data %d from root process\n", world_rank, data);
	}
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
