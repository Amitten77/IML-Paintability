#!/bin/bash
#
#SBATCH --time=00:05:00
#SBATCH --nodes=40
#SBATCH --ntasks-per-node=16
#SBATCH --job-name=traink35
#SBATCH --account=pb38-ic 
##SBATCH --mail-user=amitlk2@illinois.edu 
##SBATCH --mail-type=BEGIN,END
#
# End of embedded SBATCH options
#
g++ -std=c++20 -O3 -flto -march=native -o main main.cpp src/Board.cpp src/helper.cpp src/graph.cpp src/compare.cpp
./main