#include <stdio.h>
#include <mpi.h>
#include "functions.h"

static int is_power_of_two(int n) { return n > 0 && (n & (n - 1)) == 0; }

int main(int argc, char **argv) {
  int rank, size;
  double my_value, sum;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (!is_power_of_two(size)) {
    if (rank == 0) {
      fprintf(stderr, "Error: process count %d is not a power of two.\n", size);
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  my_value = (double)rank;

  global_sumA(&sum, rank, size, my_value);
  printf("FINAL after global_sumA: Process %d has Sum = %f\n", rank, sum);

  global_sumB(&sum, rank, size, my_value);
  printf("FINAL after global_sumB: Process %d has Sum = %f\n", rank, sum);

  MPI_Finalize();
  return 0;
}
