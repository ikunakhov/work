#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 5
#define JSIZE 5

#define NEXTR(a,b) ((a) == (b - 1) ? (0) : (a + 1))
#define PREVR(a,b) ((a) == 0 ? (b - 1) : (a - 1))


typedef struct pair
{
	int first;
	int second;
}pair;


int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	double a[ISIZE][JSIZE];
	int i,j;
	FILE *ff;

	int cores = 0 , rank = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &cores);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	pair vector_depend;
	vector_depend.first = 3;
	vector_depend.second = -4;


	for (i = 0; i < ISIZE; i++)
	{
		{
			for  (j = 0; j < JSIZE; j++)
			{
				a[i][j] = 10*i + j;
			}
		}
	}

	for (i = 0; i < ISIZE - vector_depend.first; i++)
	{
		if (i%cores == rank)
		{
			for(j = fabs(vector_depend.second); j < JSIZE; j++)
			{
				a[i][j] = sin(0.00001*a[i+vector_depend.first][j + vector_depend.second]);
			}
		}
	}

	if (rank != 0)
	{
		for (i = 0; i < ISIZE; i++)
		{
			if (i%cores == rank)
			{
				for  (j = 0; j < JSIZE; j++)
				{
					MPI_Send(&a[i][j], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);	
				}
			}
		}
	}

	if (rank == 0)
	{
		for (i = 0; i < ISIZE; i++)
		{
			if (i%cores != rank)
			{
				for  (j = 0; j < JSIZE; j++)
				{
					MPI_Recv(&a[i][j], 1, MPI_DOUBLE, i%cores, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
				}
			}	
		}

		for(i = 0; i<ISIZE; i++)
		{
			for(j = 0; j<JSIZE; j++)
			{
				printf("%lf ", a[i][j]);
			}
			printf("\n");
		}
	}
	MPI_Finalize();

}

