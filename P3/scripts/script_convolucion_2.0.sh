#!/bin/bash
#Se asigna al trabajo el nombre suavizado_mpi
#PBS -N convolucion_2.0
#Se asigna al trabajo la cola acap
#PBS -q acap
#Se imprime información del trabajo usando variables de entorno de PBS

printf "Con 1 proceso: \n"

$PBS_O_WORKDIR/convolucion_2.0 $PBS_O_WORKDIR/1_in.png $PBS_O_WORKDIR/1_out.png
$PBS_O_WORKDIR/convolucion_2.0 $PBS_O_WORKDIR/2_in.png $PBS_O_WORKDIR/2_out.png
$PBS_O_WORKDIR/convolucion_2.0 $PBS_O_WORKDIR/3_in.png $PBS_O_WORKDIR/3_out.png


for ((N=2;N<=8;N=N*2))
do
   printf "\n\nCon $N procesos: \n"

   mpirun -np $N $PBS_O_WORKDIR/convolucion-mpi_2.0 $PBS_O_WORKDIR/1_in.png $PBS_O_WORKDIR/1_out.png
   mpirun -np $N $PBS_O_WORKDIR/convolucion-mpi_2.0 $PBS_O_WORKDIR/2_in.png $PBS_O_WORKDIR/2_out.png
   mpirun -np $N $PBS_O_WORKDIR/convolucion-mpi_2.0 $PBS_O_WORKDIR/3_in.png $PBS_O_WORKDIR/3_out.png
done
