#!/bin/bash
#SBATCH --job-name=gsum
#SBATCH --output=gsum.%j.out
#SBATCH --partition=shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --mem=20GB
#SBATCH --account=ccu108
#SBATCH --time=02:00:00

module purge
module load slurm
module load cpu
module load gcc/10.2.0
module load openmpi/4.1.3

make

# Write CSV header
echo "method,np,time" > results.csv

# Run with 2, 4, 8, 16 processes as required by the assignment.
# --ntasks overrides the job-level ntasks-per-node for each step.
# --mpi=pmix ensures correct MPI bootstrap under SLURM.
for n in 2 4 8 16; do
    echo "--- Running with np=${n} ---"
    srun --nodes=1 --ntasks=${n} --mpi=pmix ./driver >> results.csv
done

echo "Done. Results written to results.csv."