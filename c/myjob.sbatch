#!/bin/bash
#
#SBATCH --time=24:00:00
#SBATCH --nodes=40
#SBATCH --job-name=traink37
#SBATCH --account=pb38-ic 
##SBATCH --mail-user=amitlk2@illinois.edu 
#SBATCH --partition=secondary
#SBATCH --output=traink37test.o%j           # Name of batch job output file
##SBATCH --error=traink37test.e%j           # Name of batch job error file
##SBATCH --mail-type=BEGIN,END
#
# End of embedded SBATCH options
#
module load gcc/12.2.0

g++ -std=c++20 -O3 -o main main.cpp src/Board.cpp src/helper.cpp src/compare.cpp

./main