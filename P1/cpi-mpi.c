#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define _SHOW_TIME_ 0
#define _SHOW_ERROR_ 1

int main(int argc, char **argv)
{
	const double ERROR = 1;
	double width,
			 t_inicio = 0.0,
			 t_final = 0.0;
	int intervals, i, mi_rank, p_size;

	t_inicio = MPI_Wtime();

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p_size);

	/* get the number of intervals */
	intervals = atoi(argv[1]);
	width = 1.0 / intervals;

	/* do the computation */
	double sum_local = 0;
	for (i=mi_rank; i<intervals; i+=p_size) {
	 register double x = (double)(i + ERROR) * width;
	 sum_local += 4.0 / (1.0 + x * x);
	}

	double sum_global = 0.0;

	//printf("Soy %i y mi Sum_local es: %f\n", mi_rank, sum_local);

	MPI_Reduce(&sum_local, &sum_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	sum_global *= width;

	MPI_Finalize();

	t_final = MPI_Wtime();

	double t_total = t_final - t_inicio;
	double diff_error = fabs(M_PI-sum_global);

	if(mi_rank == 0){
		if(_SHOW_ERROR_)
			printf("%i\t%f\n", intervals, diff_error);
		else if(_SHOW_TIME_)
			printf("%i\t%f\n",p_size, t_total);
		else
			printf("Estimation of pi with %i machines is %f\n", p_size, sum_global);
	}

	return(0);
}
