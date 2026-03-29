#!/bin/bash
#SBATCH --job-name=omp_p32
#SBATCH --output=omp_p32.%j.out
#SBATCH --partition=shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --mem=20GB
#SBATCH --account=ccu108
#SBATCH --time=02:00:00

module purge
module load cpu
module load gcc/10.2.0
module load openmpi/4.1.3
module load mpip/3.5

make clean all

echo "method,np,time" > results.csv

for n in 2 4 8 16; do
  srun -n ${n} ./driver >> results.csv
done
