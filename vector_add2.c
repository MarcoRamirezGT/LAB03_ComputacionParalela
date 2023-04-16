/*
 * Compile:  gcc vector_add2.c -o vector_add2
 * Run:      ./vector_add2
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Read_n(int *n_p);
void Allocate_vectors(double **x_pp, double **y_pp, double **z_pp, int n);
void Generate_random_vector(double a[], int n, int seed, char vec_name[]);
void Print_vector(double b[], int n, char title[]);
void Vector_sum(double x[], double y[], double z[], int n);

/*---------------------------------------------------------------------*/
int main(void)
{
    clock_t start_time, end_time;
    double total_time;
    start_time = clock(); // Iniciar medición del tiempo

    int n;
    double *x, *y, *z;

    Read_n(&n);
    Allocate_vectors(&x, &y, &z, n);

    Generate_random_vector(x, n, time(NULL), "x");
    Generate_random_vector(y, n, time(NULL), "y");

    Print_vector(x, 10, "Primeros 10 elementos de x");
    Print_vector(y, 10, "Primeros 10 elementos de y");
    Print_vector(x + n - 10, 10, "Ultimos 10 elementos de x");
    Print_vector(y + n - 10, 10, "Ultimos 10 elementos de y");

    /*Print_vector(y + n - 10, 10, "Ultimos 10 elementos de y");*/

    Vector_sum(x, y, z, n);

    Print_vector(z, n, "The sum is");

    free(x);
    free(y);
    free(z);
    end_time = clock(); // Finalizar medición del tiempo
    total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecucion: %f segundos\n", total_time);

    return 0;
} /* main */

void Read_n(int *n_p /* out */)
{
    printf("What's the order of the vectors?\n");
    scanf("%d", n_p);
    if (*n_p <= 0)
    {
        fprintf(stderr, "Order should be positive\n");
        exit(-1);
    }
} /* Read_n */

void Allocate_vectors(
    double **x_pp /* out */,
    double **y_pp /* out */,
    double **z_pp /* out */,
    int n /* in  */)
{
    *x_pp = malloc(n * sizeof(double));
    *y_pp = malloc(n * sizeof(double));
    *z_pp = malloc(n * sizeof(double));
    if (*x_pp == NULL || *y_pp == NULL || *z_pp == NULL)
    {
        fprintf(stderr, "Can't allocate vectors\n");
        exit(-1);
    }
} /* Allocate_vectors */

void Generate_random_vector(
    double a[] /* out */,
    int n /* in  */,
    int seed /* in  */,
    char vec_name[] /* in  */)
{
    int i;
    srand(seed);
    for (i = 0; i < n; i++)
        a[i] = (double)rand() / (double)RAND_MAX;
} /* Generate_random_vector */

void Print_vector(
    double b[] /* in */,
    int n /* in */,
    char title[] /* in */)
{
    printf("%s\n", title);
    int i;
    for (i = 0; i < 10 && i < n; i++) // Imprimir los primeros 10 elementos
        printf("%f ", b[i]);

    if (n > 20) // Si el arreglo tiene más de 20 elementos, imprimir puntos suspensivos
        printf("... ");

    for (i = n - 10; i < n; i++) // Imprimir los últimos 10 elementos
        if (i >= 10)
            printf("%f ", b[i]);
    printf("\n");
} /* Print_vector */

void Vector_sum(
    double x[] /* in  */,
    double y[] /* in  */,
    double z[] /* out */,
    int n /* in  */)
{
    int i;

    for (i = 0; i < n; i++)
        z[i] = x[i] + y[i];
} /* Vector_sum */
