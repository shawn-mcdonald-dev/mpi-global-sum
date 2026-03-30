#include <stdio.h>
#include <mpi.h>
#include "functions.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Helper: abort if size is not a power of two.
 * ───────────────────────────────────────────────────────────────────────────*/
static int is_power_of_two(int n) { return n > 0 && (n & (n - 1)) == 0; }

/* ─────────────────────────────────────────────────────────────────────────────
 * global_sumA  —  SPMD gather-to-root then broadcast
 *
 * Phase 1: every non-root rank Ssends its value to rank 0.
 *          Rank 0 Recvs from each rank in order and accumulates.
 * Phase 2: rank 0 Ssends the total back to every non-root rank.
 *
 * Timing is measured around both phases and printed as CSV from rank 0
 * so it can be redirected straight into results.csv.
 *
 * Prototype (assignment-required, no extra time parameter):
 *   void global_sumA(double *result, int rank, int size, double my_value);
 * ───────────────────────────────────────────────────────────────────────────*/
void global_sumA(double *result, int rank, int size, double my_value) {
    double t0, t1;
    double sum;
    int r;

    if (!is_power_of_two(size)) {
        if (rank == 0)
            fprintf(stderr, "global_sumA: size %d is not a power of two.\n", size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    t0 = MPI_Wtime();

    sum = my_value;

    if (rank == 0) {
        /* ── Phase 1: collect from all other ranks ── */
        double buf;
        for (r = 1; r < size; r++) {
            MPI_Recv(&buf, 1, MPI_DOUBLE, r, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            sum += buf;
        }
        *result = sum;

        /* ── Phase 2: broadcast result to all other ranks ── */
        for (r = 1; r < size; r++) {
            MPI_Ssend(result, 1, MPI_DOUBLE, r, 1, MPI_COMM_WORLD);
        }
    } else {
        /* Non-root: send value then wait for global sum */
        MPI_Ssend(&my_value, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&sum, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        *result = sum;
    }

    t1 = MPI_Wtime();

    /* Rank 0 prints CSV row: method, np, time */
    if (rank == 0) {
        printf("A,%d,%e\n", size, t1 - t0);
        fflush(stdout);
    }
}

/* ─────────────────────────────────────────────────────────────────────────────
 * global_sumB  —  butterfly / tree-based all-reduce
 *
 * log2(size) phases.  In phase d, each rank exchanges its running sum with
 * the partner obtained by flipping bit d of its rank.
 *
 * Deadlock avoidance with only MPI_Ssend + MPI_Recv:
 *   In every pair, the rank with the LOWER index Recvs first, then Ssends.
 *   The rank with the HIGHER index Ssends first, then Recvs.
 *   This guarantees the Ssend always finds a posted Recv, so neither side
 *   blocks indefinitely.
 *
 * After log2(size) phases every rank holds the global sum.
 *
 * Prototype (assignment-required, no extra time parameter):
 *   void global_sumB(double *result, int rank, int size, double my_value);
 * ───────────────────────────────────────────────────────────────────────────*/
void global_sumB(double *result, int rank, int size, double my_value) {
    double t0, t1;
    double sum = my_value;
    int d;

    if (!is_power_of_two(size)) {
        if (rank == 0)
            fprintf(stderr, "global_sumB: size %d is not a power of two.\n", size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    t0 = MPI_Wtime();

    /* log2(size) communication phases */
    for (d = 0; (1 << d) < size; d++) {
        int partner = rank ^ (1 << d);   /* flip bit d to find partner */
        double recv_val;

        if (rank < partner) {
            /*
             * Lower rank goes Recv-first.
             * Partner (higher rank) will Ssend first, so our Recv has a
             * matching sender and does not block.
             */
            MPI_Recv(&recv_val, 1, MPI_DOUBLE, partner, d,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Ssend(&sum, 1, MPI_DOUBLE, partner, d, MPI_COMM_WORLD);
        } else {
            /*
             * Higher rank goes Ssend-first.
             * Partner (lower rank) already posted its Recv, so our Ssend
             * completes immediately.
             */
            MPI_Ssend(&sum, 1, MPI_DOUBLE, partner, d, MPI_COMM_WORLD);
            MPI_Recv(&recv_val, 1, MPI_DOUBLE, partner, d,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        sum += recv_val;
    }

    *result = sum;

    t1 = MPI_Wtime();

    /* Rank 0 prints CSV row: method, np, time */
    if (rank == 0) {
        printf("B,%d,%e\n", size, t1 - t0);
        fflush(stdout);
    }
}