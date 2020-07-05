#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <time.h>

/* \brief A sample program that computes the average of an array of elements
 * in parallel using MPI_Reduce
 */

float *create_rand_nums(int num_elements);

int main(int argc, char** argv) {
	int num_elements_per_proc = 100;
	int world_size, world_rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	/* Create a random array of elements on all processes. */
	/* Seed the random number generator to get different results
	 * each time for each processor  */
	srand(time(NULL)*world_rank);
	float *rand_nums = NULL;
	rand_nums = create_rand_nums(num_elements_per_proc);

	/* Sum the numbers locally */
	float local_sum = 0;
	int i;
	for (i = 0; i < num_elements_per_proc; i++) {
		local_sum += rand_nums[i];
	}

	/* Print the random numbers on each process */
	printf("Process %d: Local sum %f, local average = %f\n",
			world_rank, local_sum, local_sum / num_elements_per_proc);

	/* Reduce all of the local sums into the global sum */
	float global_sum;
	MPI_Reduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM, 0,
			 MPI_COMM_WORLD);

	/* Print the result */
	if (world_rank == 0) {
		printf("Total sum = %f, average = %f\n", global_sum,
				global_sum / (world_size * num_elements_per_proc));
	}

	free(rand_nums);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}

/* Creates an array of random numbers.
 * Each number has a value from 0 - 1 */
float *create_rand_nums(int num_elements) {
	float *rand_nums = (float *)malloc(sizeof(float)*num_elements);
	assert(rand_nums != NULL);
	int i;
	for (i = 0; i < num_elements; i++) {
		rand_nums[i] = (rand()/(float)RAND_MAX);
	}
	return rand_nums;
}
