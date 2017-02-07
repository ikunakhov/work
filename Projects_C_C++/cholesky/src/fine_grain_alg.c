#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include "complexm.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define NEXTR(a,b) ((a) == (b - 1) ? (0) : (a + 1))
#define PREVR(a,b) ((a) == 0 ? (b - 1) : (a - 1))


typedef struct send_args{
	complexm val;
	int i;
	int j;

}send_args;

typedef enum calc_mode{
	PERFORMANCE,
	RESULT
}calc_mode;


void cholesky (int argc, char* argv[], complexm* mtx, int mtx_size, calc_mode mode)
{
	MPI_Init(&argc, &argv);
	int cores = 0 , rank = 0, tmp_rank = 0;
    int i = 0, j = 0, k = 0, p = 0;
	
	MPI_Comm_size(MPI_COMM_WORLD, &cores);
    
    //==================Init MPI structures types=======================================
	const int nitems_c = 4;
    int blocklengths_c[4] = {1,1,1,1};
    MPI_Datatype types_c[4] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype MPI_COMPLEXM;
    MPI_Aint     offsets_c[4];

    offsets_c[0] = offsetof(complexm, re);
    offsets_c[1] = offsetof(complexm, im);
    offsets_c[2] = offsetof(complexm, r);
    offsets_c[3] = offsetof(complexm, fi);

    MPI_Type_create_struct(nitems_c, blocklengths_c, offsets_c, types_c, &MPI_COMPLEXM);
    MPI_Type_commit(&MPI_COMPLEXM);

    const int nitems = 3;
    int blocklengths[3] = {1,1,1};
    MPI_Datatype types[3] = {MPI_COMPLEXM, MPI_INT, MPI_INT};
    MPI_Datatype MPI_S_ARGS;
    MPI_Aint     offsets[3];

    offsets[0] = offsetof(send_args, val);
    offsets[1] = offsetof(send_args, i);
    offsets[2] = offsetof(send_args, j);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_S_ARGS);
    MPI_Type_commit(&MPI_S_ARGS);
    //==================================================================================


	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	send_args buff;
	init_reim(&buff.val, 0, 0);
	buff.i= 0;
	buff.j = 0;

	if ((rank == 0) && (mode == RESULT))
    {
    	for(i = 0; i<  mtx_size; i++)
    	{
    		for(j = 0; j< mtx_size; j++)
    			printfcm_reim(mtx[j+i*mtx_size]);
    		printf("\n");
    	}
    	printf("     |||||\n");
 		printf("      |||\n");
 		printf("       |\n");
 	}
    
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
            			MPI_Recv(&buff, 1, MPI_S_ARGS, k%cores, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            			//ccopy(&mtx[buff.j + buff.i*mtx_size], &buff.val)
            			mtx[buff.j + buff.i*mtx_size] = buff.val;	
            		}
            		mtx[j + i * mtx_size] = csub(mtx[j + i * mtx_size], cmul(mtx[k + i * mtx_size], mtx[j + k * mtx_size]));
            		//mtx[j + i * mtx_size] = mtx[j + i * mtx_size] - mtx[k + i * mtx_size] * mtx[j + k * mtx_size];
            	}

            	if (i == j)
            	{
            		mtx[i + i * mtx_size] = cmsqrt(mtx[i + i * mtx_size], 2, 0);
                	//mtx[i + i * mtx_size] = sqrt(mtx[i + i * mtx_size]);
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
                		mtx[j + i * mtx_size] = cdiv(mtx[j + i * mtx_size], mtx[i + i * mtx_size]);
                    	//mtx[j + i * mtx_size] = mtx[j + i * mtx_size] / mtx[i + i * mtx_size];

                	}
                	else
                	{
                		mtx[j + i * mtx_size] = cdiv(mtx[j + i * mtx_size], mtx[j + j * mtx_size]);
                    	//mtx[j + i * mtx_size] = mtx[j + i * mtx_size] / mtx[j + j * mtx_size];
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
        			init_reim(&mtx[j+i*mtx_size], 0, 0);
        			//mtx[j+i*mtx_size] = 0;
        		if(mode == RESULT)
            		printfcm_reim(mtx[j + i * mtx_size]);
            		//printf("%lf ", mtx[j + i * mtx_size]);
        	}
        	if (mode == RESULT)
        	printf("\n");
    	}
	}
	MPI_Finalize();

}

void generate_cmtx(complexm* mtx, int size)
{
	int i = 0, j = 0;
	for(i = 0; i < size;i++)
	{
		for(j = 0; j < size; j++)
		{
			if (i == j ){
				init_reim(&mtx[i + size*j], i+j+1, i);
			}
			else{
				init_reim(&mtx[i + j * size], 0, 0);
			}

		}
	}
}


int main(int argc, char* argv[])
{
    int mtx_size = atoi(argv[1]);
    calc_mode mode;
    if (!strcmp(argv[2], "p"))
    {
    	mode = PERFORMANCE;
    }
    else if (!strcmp(argv[2], "r"))
    {
    	mode = RESULT;
    }
    else 
    {
    	printf("wrong mode");
    	return 0;
    }
    complexm* mtx = cmtx_alloc(mtx_size);
    generate_cmtx(mtx, mtx_size);
	cholesky(argc, argv, mtx, mtx_size, mode);
	cmtx_free(mtx);
    return 0;
}

