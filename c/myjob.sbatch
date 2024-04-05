#!/bin/bash
#
#SBATCH --time=00:05:00
#SBATCH --nodes=16
#SBATCH --job-name=traink35again
#SBATCH --account=pb38-ic 
##SBATCH --mail-user=amitlk2@illinois.edu 
#SBATCH --partition=secondary
#SBATCH --output=traink35test.o%j           # Name of batch job output file
##SBATCH --error=traink35test.e%j           # Name of batch job error file
##SBATCH --mail-type=BEGIN,END
#
# End of embedded SBATCH options
#
module load gcc/12.2.0

g++ -std=c++20 -O3 -flto -march=native -o main main.cpp src/Board.cpp src/helper.cpp src/graph.cpp src/compare.cpp

./main