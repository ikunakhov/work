#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "complexm.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define NEXTR(a,b) ((a) == (b - 1) ? (0) : (a + 1))
#define PREVR(a,b) ((a) == 0 ? (b - 1) : (a - 1))


inline int min(int a, int b) { return (a<b) ? a : b; }



typedef struct send_args{
    complexm val;
    int i;
    int j;

}send_args;

typedef enum calc_mode{
    PERFORMANCE,
    RESULT,
    NONE
}calc_mode;




void cholesky (double* mtx, int roi, int mtx_size,  calc_mode mode)
{
    int i = 0, j = 0, k = 0, p = 0;

    for (j = 0; j < roi; j++)
    {   
            for (i = 0; i < roi; i++)
            {
                for (k = 0; k < MIN(i, j); k++)
                {
                    mtx[j + i * mtx_size] = mtx[j + i * mtx_size] - mtx[k + i * mtx_size] * mtx[j + k * mtx_size];
                   
                }

                if (i == j)
                {
                    mtx[i + i * mtx_size] = sqrt(mtx[i + i * mtx_size]);
                   
                }
                else
                {
                    if (i < j)
                    {
                        mtx[j + i * mtx_size] = mtx[j + i * mtx_size] / mtx[i + i * mtx_size];
                       

                    }
                    else
                    {
                       mtx[j + i * mtx_size] = mtx[j + i * mtx_size] / mtx[j + j * mtx_size];
                   
                    }
                }
            }   
        
    }

}
/*
void cholesky_double(double* mtx, int roi, int mtx_size, calc_mode mode)
{
    complexm* tmp = cmtx_alloc(roi);
    double_mtx2complexm_mtx(mtx, tmp, mtx_size, mtx_size);
    cholesky(tmp, roi, mtx_size, mode);
    complexm_mtx2double_mtx(tmp, mtx, mtx_size, mtx_size);
    cmtx_free(tmp);
}*/

void chol_blocked(double *A, int n, int b){

  int i,j,k,m;
  int info;
  const double one = 1.0;
  const double minusone = -1.0;
  double t1, t2;

  for(k = 0; k < n; k += b){
    m=min( n-k, b );

    cholesky(&A[k*n+k], m, n, NONE);
    //dpotrf_("L", &m, &A[k*n+k], &n, &info);
    for(i = k+b; i < n; i += b){
      m=min( n-i, b );  
      dtrsm_("R","L","T","N", &m, &b, &one, &A[k*n+k], &n, &A[i+k*n], &n);
    }
    
    for(i = k + b; i < n; i += b){
      m=min( n-i, b ); 
      for(j = k + b; j <= i - 1; j += b){
       dgemm_("N","T", &m, &b, &b, &minusone, &A[i+k*n], &n, &A[j+k*n], &n, &one, &A[j*n+i], &n);
    }
    dsyrk_("L","N",&m, &b, &minusone, &A[i+k*n], &n, &one, &A[i*n+i], &n);
    }
  }
}


void generate_cmtx(double* mtx, int size)
{
    int i = 0, j = 0;
    for(i = 0; i < size;i++)
    {
        for(j = 0; j < size; j++)
        {
            if (i == j ){
                mtx[i + size*j]= i+j+1;
            }
            else{
                mtx[i + j * size] = 0;
            }
            

        }
    }
}


int main(int argc, char* argv[])
{
    int mtx_size = atoi(argv[1]);
    int block_size = atoi(argv[2]);
    calc_mode mode;
    int i= 0 , j = 0;
    if (!strcmp(argv[3], "p"))
    {
        mode = PERFORMANCE;
    }
    else if (!strcmp(argv[3], "r"))
    {
        mode = RESULT;
    }
    else if (!strcmp(argv[3], "n"))
    {
        mode = NONE;
    }
    else 
    {

        printf("wrong mode");
        return 0;
    }
    double* mtx = (double*)malloc(mtx_size*mtx_size*sizeof(double));
    generate_cmtx(mtx, mtx_size);
        for(i = 0; i < mtx_size;i++)
        {
            for(j = 0; j < mtx_size; j++)
            {
                printf("%lf ", mtx[j+i*mtx_size]);
            }
            printf("\n");
        }
        printf("\n");
    chol_blocked(mtx, mtx_size, block_size);

    for(i = 0; i < mtx_size;i++)
        {
            for(j = 0; j < mtx_size; j++)
            {
                printf("%lf ", mtx[j+i*mtx_size]);
            }
            printf("\n");
        }

    free(mtx);
    return 0;
}

