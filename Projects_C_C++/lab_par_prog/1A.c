#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 1000
#define JSIZE 1000

#define NEXTR(a,b) ((a) == (b - 1) ? (0) : (a + 1))
#define PREVR(a,b) ((a) == 0 ? (b - 1) : (a - 1))

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	double a[ISIZE][JSIZE];
	int i,j;
	FILE *ff;

	int cores = 0 , rank = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &cores);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	for (i = 0; i < ISIZE; i++)
	{
		if (i%cores == rank)
		{
			for  (j = 0; j < JSIZE; j++)
			{
				a[i][j] = 10*i + j;
			}
		}
	}


	if (rank == 0)
	{
		for (j = 0; j<JSIZE; j++)
		{
				MPI_Send(&a[0][j], 1, MPI_DOUBLE, NEXTR(rank,cores), 0, MPI_COMM_WORLD);
		}
	}

	for (i = 1; i < ISIZE; i++)
	{
		if (i%cores == rank)
		{
			for(j = 0 ;j<JSIZE + 1; j++)
			{
				if (j != JSIZE)
					MPI_Recv(&a[i-1][j], 1, MPI_DOUBLE, PREVR(rank,cores), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if (j != 0)
				{
					if (j != JSIZE)
						a[i][j-1] = sin(0.00001*a[i-1][j]);
					if (i!= ISIZE-1)
						MPI_Send(&a[i][j -1], 1, MPI_DOUBLE, NEXTR(rank,cores), 0, MPI_COMM_WORLD);
					else if(rank != 0)
						MPI_Send(&a[i][j -1], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
				}
			}
		}
	}
	if ((rank != 0) && (NEXTR(rank,cores) != 0))
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
			if ((i%cores != rank) &&(NEXTR(i%cores, cores) != 0))
			{
				for  (j = 0; j < JSIZE; j++)
				{
					MPI_Recv(&a[i][j], 1, MPI_DOUBLE, i%cores, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
				}
			}
			else if ((i%cores != rank) && (ISIZE-1 == i))
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
