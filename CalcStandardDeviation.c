#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <assert.h>

/* \brief A sample program that computes the standard deviation of an
 * array of elements in parallel using MPI_Reduce.
 */

float *create_rand_nums(int num_elements);

int main(int argc, char** argv) {
	int num_elements_per_proc = 100;
	int world_size, world_rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	/* Seed the random number generator to get different results in each process */
	srand(time(NULL)*world_rank);

	/* Create a random array of elements on all processes */
	float *rand_nums = NULL;
	rand_nums = create_rand_nums(num_elements_per_proc);

	/* Sum the numbers locally */
	float local_sum = 0;
	int i;
	for (i = 0; i < num_elements_per_proc; i++) {
		local_sum += rand_nums[i];
	}

	/* Reduce all of the local sums into the global sum in order to
	 * calculate the mean */
	float global_sum;
	MPI_Allreduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM,
			MPI_COMM_WORLD);
	float mean = global_sum/(num_elements_per_proc*world_size);

	/* Compute the local sum of the squared differences from the mean */
	float local_sq_diff = 0;
	for (i = 0; i < num_elements_per_proc; i++) {
		local_sq_diff += (rand_nums[i]-mean)*(rand_nums[i]-mean);
	}

	/* Reduce the global sum of the squared differences to the root process
	 * and print the answer */
	float global_sq_diff;
	MPI_Reduce(&local_sq_diff, &global_sq_diff, 1, MPI_FLOAT, MPI_SUM, 0,
			MPI_COMM_WORLD);

    /* The standard deviation is the square root of the mean of the
     * squared differences. */
	if (world_rank == 0) {
		float stddev = sqrt(global_sq_diff/(num_elements_per_proc*world_size));
		printf("Mean = %f, Standard deviation = %f\n", mean, stddev);
	}

  	/* Cleaning */
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
