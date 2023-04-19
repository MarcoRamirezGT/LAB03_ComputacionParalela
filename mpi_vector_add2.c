/*
 * Compile:  mpicc mpi-vector-add2.c -o mpi-vector-add2
 * Run:      mpiexec -np N ./mpi-vector-add2
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void Check_for_error(int local_ok, char fname[], char message[],
                     MPI_Comm comm);
void Read_n(int *n_p, int *local_n_p, int my_rank, int comm_sz,
            MPI_Comm comm);
void Allocate_vectors(double **local_x_pp, double **local_y_pp,
                      double **local_z_pp, int local_n, MPI_Comm comm);
void Generate_random_vector(double local_a[], int local_n, int n,
                 int my_rank, MPI_Comm comm);
void Print_vector(double local_b[], int local_n, int n, char title[],
                  int my_rank, MPI_Comm comm);
void Parallel_vector_sum(double local_x[], double local_y[],
                         double local_z[], int local_n);

/*-------------------------------------------------------------------*/
int main(void)
{
    int n, local_n;
    int comm_sz, my_rank;
    double *local_x, *local_y, *local_z;
    MPI_Comm comm;
    double tstart, tend;

    srand(time(NULL));

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    Read_n(&n, &local_n, my_rank, comm_sz, comm);
    srand(time(NULL));
    Allocate_vectors(&local_x, &local_y, &local_z, local_n, comm);


    Generate_random_vector(local_x, local_n, n, my_rank, comm);
    Generate_random_vector(local_y, local_n, n, my_rank, comm);
    
    
    Print_vector(local_x, local_n, n, "Vector x is:", my_rank, comm);
    Print_vector(local_y, local_n, n, "Vector y is:", my_rank, comm);

    tstart = MPI_Wtime();
    Parallel_vector_sum(local_x, local_y, local_z, local_n);
    tend = MPI_Wtime();

    Print_vector(local_z, local_n, n, "The sum is", my_rank, comm);
    if (my_rank == 0)
        printf("\nTook %f seconds to run\n", tend - tstart);

    free(local_x);
    free(local_y);
    free(local_z);

    MPI_Finalize();

    return 0;
} /* main */

void Check_for_error(
    int local_ok /* in */,
    char fname[] /* in */,
    char message[] /* in */,
    MPI_Comm comm /* in */)
{
    int ok;

    MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
    if (ok == 0)
    {
        int my_rank;
        MPI_Comm_rank(comm, &my_rank);
        if (my_rank == 0)
        {
            fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname,
                    message);
            fflush(stderr);
        }
        MPI_Finalize();
        exit(-1);
    }
} /* Check_for_error */

void Read_n(
    int *n_p /* out */,
    int *local_n_p /* out */,
    int my_rank /* in  */,
    int comm_sz /* in  */,
    MPI_Comm comm /* in  */)
{
    int local_ok = 1;
    char *fname = "Read_n";

    if (my_rank == 0)
    {
        printf("What's the order of the vectors?\n");
        scanf("%d", n_p);
    }
    MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
    if (*n_p <= 0 || *n_p % comm_sz != 0)
        local_ok = 0;
    Check_for_error(local_ok, fname,
                    "n should be > 0 and evenly divisible by comm_sz", comm);
    *local_n_p = *n_p / comm_sz;
} /* Read_n */

void Allocate_vectors(
    double **local_x_pp /* out */,
    double **local_y_pp /* out */,
    double **local_z_pp /* out */,
    int local_n /* in  */,
    MPI_Comm comm /* in  */)
{
    int local_ok = 1;
    char *fname = "Allocate_vectors";

    *local_x_pp = malloc(local_n * sizeof(double));
    *local_y_pp = malloc(local_n * sizeof(double));
    *local_z_pp = malloc(local_n * sizeof(double));

    if (*local_x_pp == NULL || *local_y_pp == NULL ||
        *local_z_pp == NULL)
        local_ok = 0;
    Check_for_error(local_ok, fname, "Can't allocate local vector(s)",
                    comm);
} /* Allocate_vectors */

void Generate_random_vector(
    double local_a[] /* out */,
    int local_n /* in  */,
    int n /* in  */,
    int my_rank /* in  */,
    MPI_Comm comm /* in  */)
{

    double *a = NULL;
    int i;
    int local_ok = 1;
    char *fname = "Generate_random_vector";

    if (my_rank == 0)
    {
        a = malloc(n * sizeof(double));
        if (a == NULL)
            local_ok = 0;
        Check_for_error(local_ok, fname, "Can't allocate temporary vector",
                        comm);
        // printf("Enter the vector %s\n", vec_name);
        // fill vec with indez
        for (i = 0; i < n; i++)
            a[i] = (double)rand() / (double)RAND_MAX;
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0,
                    comm);
        free(a);
    }
    else
    {
        Check_for_error(local_ok, fname, "Can't allocate temporary vector",
                        comm);
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0,
                    comm);
    }
} /* Read_vector */

void Print_vector(
    double local_b[] /* in */,
    int local_n /* in */,
    int n /* in */,
    char title[] /* in */,
    int my_rank /* in */,
    MPI_Comm comm /* in */)
{

    double *b = NULL;
    int i;
    int local_ok = 1;
    char *fname = "Print_vector";

    if (my_rank == 0)
    {
        b = malloc(n * sizeof(double));
        if (b == NULL)
            local_ok = 0;
        Check_for_error(local_ok, fname, "Can't allocate temporary vector",
                        comm);
        MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE,
                   0, comm);
        printf("%s\n", title);
        for (i = 0; i < 10 && i < n; i++)
            printf("%.3f ", b[i]);
        
        if (n > 20)
            printf("... ");

        for (i = n - 10; i < n; i++)
            if (i >= 10)
                printf("%.3f ", b[i]);
        printf("\n");
        free(b);
    }
    else
    {
        Check_for_error(local_ok, fname, "Can't allocate temporary vector",
                        comm);
        MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE, 0,
                   comm);
    }
} /* Print_vector */

void Parallel_vector_sum(
    double local_x[] /* in  */,
    double local_y[] /* in  */,
    double local_z[] /* out */,
    int local_n /* in  */)
{
    int local_i;

    for (local_i = 0; local_i < local_n; local_i++)
        local_z[local_i] = local_x[local_i] + local_y[local_i];
} /* Parallel_vector_sum */
