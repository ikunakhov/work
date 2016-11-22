#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

int next_rank(int rank, int cores)
{
	int ret = 0;
	if (rank != cores - 1)
		ret = rank + 1;
	else
		ret = 0;
	return ret;
}

int prev_rank(int rank, int cores)
{
	int ret = 0;
	if (rank != 0)
		ret = rank - 1;
	else 
		ret = cores - 1;
	return ret;
}


int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int k = 0, i = 0, j = 0, p = 0, l = 0;
	const int size_mtx = 2;
	int flag = 0;

	double a[] = {4, 1,		
				  1, 4};
	
	for (i = 0; i < size_mtx; i++)
	{
		if ((i % size) == rank)
		{
			for (j = 0; j <= i; j++)
			{
				for(k = 0; k <= (j - 1); k++)
				{
					if (j != size_mtx - 1)
					{
						MPI_Recv(&a[j + k*size_mtx], 1, MPI_DOUBLE, k % size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					}
					else
						flag = 1;
					a[j + i*size_mtx] = a[j + i*size_mtx] - a[k + i*size_mtx]*a[j + k*size_mtx];
				}
				if (i == j)
				{
					a[i + i*size_mtx] = sqrt(a[i + i*size_mtx]);
					if ( i != (size_mtx - 1))
					{
						l = i;
						p = rank;
						while ((p < size) && (l < size_mtx))
						{
							p = next_rank(rank, size);
							if ( p != rank)
								MPI_Send(&a[i+i*size_mtx], 1, MPI_DOUBLE, p, 0, MPI_COMM_WORLD);
							l++;
						}
					}
					else
						flag = 1;		
				}
				else
				{
					MPI_Recv(&a[j + j*size], 1, MPI_DOUBLE, j % size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					a[j + i*size_mtx] = a[j + i*size_mtx]/a[j + j*size];
					if ( i != size_mtx - 1)
					{
						l = i;
						p = rank;
						while((p < size) && ( l < size_mtx))
						{
							p = next_rank(rank, size);
							if ( p != rank)
								MPI_Send(&a[j+ i*size_mtx], 1, MPI_DOUBLE, p, 0,  MPI_COMM_WORLD);
							l++;
						}
					}
					else
						flag = 1;
				}
			}
		}
	}
	if (flag == 1)
	{
		for(i = 0; i < size_mtx; i++)
		{
			for ( j = 0; j  < size_mtx; j++)
			{
				if ( j > i) a[j + i*size_mtx] = 0;
				printf("%lf ", a[j + i*size_mtx]);
			}
			printf("\n");
		}	
	}
	MPI_Finalize();
	return 0;

}

