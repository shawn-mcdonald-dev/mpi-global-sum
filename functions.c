#include <stdio.h>
#include <mpi.h>
#include "functions.h"

static int is_power_of_two(int n) { return n > 0 && (n & (n - 1)) == 0; }

void global_sumA(double *result, int rank, int size, double my_value) {
  double t0, t1;
  double sum;
  int r;

  if (!is_power_of_two(size)) {
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  t0 = MPI_Wtime();
  sum = my_value;

  if (rank == 0) {
    double buf;
    for (r = 1; r < size; r++) {
      MPI_Recv(&buf, 1, MPI_DOUBLE, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      sum += buf;
    }
    *result = sum;
    for (r = 1; r < size; r++) {
      MPI_Ssend(result, 1, MPI_DOUBLE, r, 1, MPI_COMM_WORLD);
    }
  } else {
    MPI_Ssend(&my_value, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    MPI_Recv(&sum, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    *result = sum;
  }
  t1 = MPI_Wtime();

  if (rank == 0) {
    printf("global_sumA: elapsed = %e s\n", t1 - t0);
  }
}

void global_sumB(double *result, int rank, int size, double my_value) {
  double t0, t1;
  double sum = my_value;
  int d;

  if (!is_power_of_two(size)) {
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  t0 = MPI_Wtime();
  for (d = 0; (1 << d) < size; d++) {
    int partner = rank ^ (1 << d);
    double recv_val;
    if (rank < partner) {
      MPI_Ssend(&sum, 1, MPI_DOUBLE, partner, d, MPI_COMM_WORLD);
      MPI_Recv(&recv_val, 1, MPI_DOUBLE, partner, d, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
    } else {
      MPI_Recv(&recv_val, 1, MPI_DOUBLE, partner, d, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
      MPI_Ssend(&sum, 1, MPI_DOUBLE, partner, d, MPI_COMM_WORLD);
    }
    sum += recv_val;
  }
  *result = sum;
  t1 = MPI_Wtime();

  if (rank == 0) {
    printf("global_sumB: elapsed = %e s\n", t1 - t0);
  }
}
