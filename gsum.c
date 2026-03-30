#include <stdio.h>
#include <mpi.h>
#include "functions.h"

static int is_power_of_two(int n) { return n > 0 && (n & (n - 1)) == 0; }

int main(int argc, char **argv) {
    int rank, size;
    double my_value;
    double resultA, resultB;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Power-of-two check */
    if (!is_power_of_two(size)) {
        if (rank == 0) {
            fprintf(stderr,
                "Error: process count %d is not a power of two.\n", size);
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* Each process's value is its rank, as required by the assignment */
    my_value = (double)rank;

    /* ---- Method A ---- */
    global_sumA(&resultA, rank, size, my_value);

    /* All ranks print their result for method A */
    printf("[A] Rank %d: global sum = %.1f\n", rank, resultA);
    MPI_Barrier(MPI_COMM_WORLD);   /* keep output blocks together */

    /* ---- Method B ---- */
    global_sumB(&resultB, rank, size, my_value);

    /* All ranks print their result for method B */
    printf("[B] Rank %d: global sum = %.1f\n", rank, resultB);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}