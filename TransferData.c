#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* \brief A sample program that demonstrates how to time an MPI program, and how to
 * send (parts of) the rows and columns in a two-dimensional array between
 * two processes using MPI.
 * In the matrix representation used here, columns are simple to copy, as
 * column elements are contiguous in memory. Row copying is more complex as
 * consecutive row elements are not contiguous in memory and require the
 * use of derived MPI_Datatypes to perform the copy.
 */

#define rows 9
#define cols 7

void zero_matrix(double **matrix);
void print_matrix(char *title, double **matrix);
double **double_2D_array(long int m, long int n);
void free_2D_array(void **array);

int main(int argc, char **argv)
{
    double t;
    int i, j, world_size, world_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_size != 2) {
        printf("Must be run on exactly 2 processors.\n");
        MPI_Finalize();
        return 1;
    }

    t = MPI_Wtime();

    /* Allocate an cols * rows array */
    double **matrix = double_2D_array(cols, rows);

    /* Fill processor 1's matrix with numbers */
    for (i = 0; i < cols; i++) {
        for ( j = 0; j < rows; j++) {
            matrix[i][j] = (i * 10) + j;
        }
    }

    /* Define two MPI_Datatypes for rows that we use later */
    MPI_Datatype fullrowtype, partrowtype;
    MPI_Type_vector(rows, 1, rows, MPI_DOUBLE, &fullrowtype);
    MPI_Type_commit(&fullrowtype);
    MPI_Type_vector(3, 1, rows, MPI_DOUBLE, &partrowtype);
    MPI_Type_commit(&partrowtype);

    if (world_rank == 0) {
        MPI_Status s;

        zero_matrix(matrix);
        MPI_Recv(matrix[4], rows, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &s);
        print_matrix("After receiving column 4:", matrix);

        zero_matrix(matrix);
        MPI_Recv(&matrix[6][2], 4, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &s);
        print_matrix("After receiving column 6, rows 3-5:", matrix);

        zero_matrix(matrix);
        MPI_Recv(matrix[3], rows*2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &s);
        print_matrix("After receiving column 3 and 4:", matrix);

        zero_matrix(matrix);
        MPI_Recv(matrix[0], rows*cols, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &s);
        print_matrix("After receiving all columns:", matrix);

        zero_matrix(matrix);
        MPI_Recv(&matrix[0][6], 1, fullrowtype, 1, 0, MPI_COMM_WORLD, &s);
        print_matrix("After receiving row 6:", matrix);

        zero_matrix(matrix);
        MPI_Recv(&matrix[0][1], 1, partrowtype, 1, 0, MPI_COMM_WORLD, &s);
        print_matrix("After receiving row 1 cols 0-2:", matrix);

        zero_matrix(matrix);
        MPI_Recv(&matrix[4][1], 1, partrowtype, 1, 0, MPI_COMM_WORLD, &s);
        print_matrix("After receiving row 1 cols 4-6:", matrix);
    }
    else {
        /* Send all of column 4 to processor 0 */
        MPI_Send(matrix[4], rows, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

        /* Send column 6 rows 2-5 to processor 0 */
        MPI_Send(&matrix[6][2], 4, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

        /* Send columns 3 and 4 to processor 0 */
        MPI_Send(matrix[3], rows*2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

        /* Send the entire matrix (ie all columns) to processor 0 */
        MPI_Send(matrix[0], rows*cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

        /* Send row 6 to processor 0 */
        MPI_Send(&matrix[0][6], 1, fullrowtype, 0, 0, MPI_COMM_WORLD);

        /* Send row 1 cols 0-2 to processor 0 */
        MPI_Send(&matrix[0][1], 1, partrowtype, 0, 0, MPI_COMM_WORLD);

        /* Send row 1 cols 4-6 to processor 0 */
        MPI_Send(&matrix[4][1], 1, partrowtype, 0, 0, MPI_COMM_WORLD);
    }
    if (world_rank == 0) {
        t = MPI_Wtime() - t;
        printf("Program took %f secs to run.\n", t);
    }

    /* Free the matrix we allocated */
	free_2D_array((void **)matrix);

    /* Free the derived MPI_Datatypes */
    MPI_Type_free(&fullrowtype);
    MPI_Type_free(&partrowtype);

    MPI_Finalize();
    return 0;
}

void zero_matrix(double **matrix)
{
    int i, j;
    for ( j = 0; j < rows; j++) {
        for ( i = 0; i < cols; i++) {
            matrix[i][j] = 0.0;
        }
    }
}

void print_matrix(char *title, double **matrix)
{
    int i, j;
    printf("%s\n", title);
    for ( j = 0; j < rows; j++) {
        for ( i = 0; i < cols; i++) {
            printf("%02g ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

double **double_2D_array(long int m, long int n)
{
	double **array;
	int i;
	long int x;
	x = m * n;
	array =  (double**)malloc(m * sizeof(double *));
	array[0] =  (double*)malloc(x * sizeof(double));

	for(i = 0; i < m; ++i)
		array[i]  = array[0] + i * n;

	return array;
}

void free_2D_array(void **array)
{
	free(array[0]);
	free(array);
	array = NULL;
}
