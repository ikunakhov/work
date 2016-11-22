#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define NEXTR(a,b) ((a) == (b - 1) ? (0) : (a + 1))
#define PREVR(a,b) ((a) == 0 ? (b - 1) : (a - 1))


typedef struct send_args{
	double val;
	int i;
	int j;

}send_args;

typedef enum calc_mode{
	PERFORMANCE,
	RESULT
}calc_mode;


void cholesky (int argc, char* argv[], double* mtx, int mtx_size, calc_mode mode)
{
	MPI_Init(&argc, &argv);
	int cores = 0 , rank = 0, tmp_rank = 0;
    int i = 0, j = 0, k = 0, p = 0;
	
	MPI_Comm_size(MPI_COMM_WORLD, &cores);
    const int nitems = 3;
    int blocklengths[3] = {1,1,1};
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_INT, MPI_INT};
    MPI_Datatype MPI_S_ARGS;
    MPI_Aint     offsets[3];

    offsets[0] = offsetof(send_args, val);
    offsets[1] = offsetof(send_args, i);
    offsets[2] = offsetof(send_args, j);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_S_ARGS);
    MPI_Type_commit(&MPI_S_ARGS);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	send_args buff;
	buff.val = 0;
	buff.i= 0;
	buff.j = 0;
    
	double t1 = 0, t2 = 0;
    if (rank == ((mtx_size -1) % cores))
    	t1 = MPI_Wtime();

    for (j = 0; j < mtx_size; j++)
    {	
		if ((j % cores) == rank) 
		{
        	for (i = 0; i < mtx_size; i++)
        	{
            	for (k = 0; k < MIN(i, j); k++)
            	{
            		if (k%cores != rank)
            		{
            			MPI_Recv(&buff, 1, MPI_S_ARGS, k%cores, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            			mtx[buff.j + buff.i*mtx_size] = buff.val;
            			
            		}
            		mtx[j + i * mtx_size] = mtx[j + i * mtx_size] - mtx[k + i * mtx_size] * mtx[j + k * mtx_size];
            	}

            	if (i == j)
            	{
                	mtx[i + i * mtx_size] = sqrt(mtx[i + i * mtx_size]);
                	if (i%cores != rank)
                	{
                		buff.val = mtx[i+i*mtx_size];
                		buff.i = i;
                		buff.j = i;
                		MPI_Send(&buff, 1, MPI_S_ARGS, i%cores, 0, MPI_COMM_WORLD);
                	}
                	for(p = i+1; p < mtx_size; p++)
                	{
                		if (p%cores != rank)
                		{
                			buff.val=  mtx[i+i*mtx_size];
                			buff.i = i;
                			buff.j = i;
                			MPI_Send(&buff, 1, MPI_S_ARGS, p%cores, 0, MPI_COMM_WORLD);	
                		}
                	}
            	}
            	else
            	{
                	if (i < j)
                	{
                		if (i % cores != rank)
                		{
            
                			MPI_Recv(&buff, 1, MPI_S_ARGS, i % cores, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                			mtx[buff.j+buff.i*mtx_size] = buff.val;
                			
                		}
                    	mtx[j + i * mtx_size] = mtx[j + i * mtx_size] / mtx[i + i * mtx_size];

                	}
                	else
                	{
                    	mtx[j + i * mtx_size] = mtx[j + i * mtx_size] / mtx[j + j * mtx_size];
                		for(p = j+1; p < mtx_size; p++)
                		{
                			if (p%cores != rank)
                			{
                				buff.val = mtx[j+i*mtx_size];
                				buff.j = j;
                				buff.i = i; 
                				MPI_Send(&buff, 1, MPI_S_ARGS, p%cores, 0, MPI_COMM_WORLD);
                			}
                		}
                	}
            	}
        	}	
    	}
	}

	if (rank == ((mtx_size -1) % cores))
	{
		t2 = MPI_Wtime();
		if (mode == PERFORMANCE)
			printf("time %lf \n", t2-t1);
    	for (i = 0; i < mtx_size; i++)
    	{
        	for (j = 0; j < mtx_size; j++)
        	{
        		if (j > i)
        			mtx[j+i*mtx_size] = 0;
        		if(mode == RESULT)
            		printf("%lf ", mtx[j + i * mtx_size]);
        	}
        	if (mode == RESULT)
        	printf("\n");
    	}
	}
	MPI_Finalize();

}


void generate_pos_def_mtx(double* mtx, int size)
{
	int i = 0, j = 0;
	int tmp = 1;
	for (i = 0; i< size; i++)
	{
		for(j = 0; j <= i; j++)
		{
			if (i == j)
			{
				mtx[j+i*size] = tmp;
				tmp+=2;
			}
			else
			{
				if ((i+j) % 2 == 0)
				{
					mtx[j+i*size] = 0;
					mtx[i+j*size] = 0;
				}
				else
				{
					mtx[j+i*size] = 1;
					mtx[i+j*size] = 1;
				}
			}
		}
	}
}






int main(int argc, char* argv[])
{
    int mtx_size = atoi(argv[1]);

    double* mtx = (double*)malloc(mtx_size*mtx_size*sizeof(double));
    int i= 0, j = 0;
    generate_pos_def_mtx(mtx, mtx_size);
	cholesky(argc, argv, mtx, mtx_size, PERFORMANCE);
    return 0;
}

