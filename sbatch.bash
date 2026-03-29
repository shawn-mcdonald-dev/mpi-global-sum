#!/bin/bash
#SBATCH --job-name=omp_p32
#SBATCH --output=omp_p32.%j.out
#SBATCH --partition=shared
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=32
#SBATCH --mem=20GB
#SBATCH --account=ccu108
#SBATCH --time=02:00:00

module purge
module load cpu
module load gcc/10.2.0
module load openmpi/4.1.3
module load mpip/3.5

make clean all

for n in 2 4 8 16; do
  echo "========================================"
  echo "MPI tasks (np) = ${n}"
  echo "========================================"
  srun -n "${n}" ./driver > "omp_p${n}.out"
done
