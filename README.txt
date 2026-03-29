MPI Global Sum — build and run
==============================

Build
-----
  make

This produces the executable `driver` using mpicc.

Run (interactive / login node)
------------------------------
  mpirun -np 4 ./driver
  # or: mpiexec -n 4 ./driver

Use a power of two for -np (e.g. 2, 4, 8, 16). Otherwise the program aborts.

Each process sets my_value = rank. The correct global sum is:
  sum = 0 + 1 + ... + (np-1) = np*(np-1)/2

The program runs global_sumA (hub gather + spread), then global_sumB (recursive
doubling / hypercube-style phases). Each function prints per-rank elapsed time
for its communication.

Batch experiments (minimum np = 2, 4, 8, 16)
--------------------------------------------
Edit sbatch.bash for your site's partition, account, and `module load` lines, then:

  sbatch sbatch.bash

Collect the `global_sumA` / `global_sumB` timing lines from the job output. For the
write-up, plot elapsed time (e.g. rank 0’s line, or max over ranks) versus np for
each algorithm — line or bar charts usually work well for comparing scaling.

Figures
-------
See assignment for discussion of graphs. Optional local images: figure-3.8.png,
figure-1.1.png in this directory.

Clean
-----
  make clean
