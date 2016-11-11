#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <complexm.h>
#include "gtest/gtest.h"

typedef enum ret_err
{
    SUCCESS,
    WARNING,
    NO_MEM_ALLOCATED,
    ZERO_SIZE,
    DIVISION_BY_ZERO
}ret_err;


void mtx_transp(complexm* mtx, complexm* out, int size)
{
    int i = 0, j = 0;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
            ccopy(&(out[i*size + j]), &(mtx[j*size + i]));
    }
}

void cmul_mtx_mtx(complexm* mtx1, complexm* mtx2, complexm* out, int size, void* buffer)//, int threads)
{
    complexm* mtx2T = (complexm*)buffer;
    mtx_transp(mtx2, mtx2T, size);

    int i = 0, j = 0, k = 0;
    complexm s;
//#pragma omp parallel for num_threads(threads)
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
        	init_reim(&out[i * size + j], 0,0);
            init_reim(&s, 0, 0);
            for (k = 0; k < size; k++)
                s  = csum(cmul(mtx1[i*size + k], mtx2T[j*size + k]),s);
            ccopy(&out[i*size + j],&s);
        }
    }
}

complexm* cmtx_alloc(int size)
{
    complexm* out = (complexm*)malloc(size*size*sizeof(complexm));
    int i = 0, j = 0;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            init_reim(&out[i + j*size], 0, 0);
        }
    }
    return out;

}

ret_err cholesky(complexm* in_mtx, complexm* out_mtx, int size)
{
	ret_err ret = SUCCESS;
    if (in_mtx  == NULL)  return NO_MEM_ALLOCATED;
    if (out_mtx == NULL)  return NO_MEM_ALLOCATED;
    if (size == 0)        return ZERO_SIZE;

    int i = 0, j = 0, k = 0;

    complexm tmp; init_reim(&tmp, 0, 0);

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < i; j++)
        {
            init_reim(&tmp, 0, 0);
            for (k = 0; k < j; k++)
            {
                tmp = csum(cmul(out_mtx[k + i*size], cmonj(out_mtx[k + j*size])), tmp);
            }
            if ((out_mtx[j + j*size].re == 0) && (out_mtx[j + j*size].im == 0)) return DIVISION_BY_ZERO;
            out_mtx[j + i*size] = cdiv(csub(in_mtx[i + j*size], tmp), out_mtx[j + j*size]);
        }
        tmp = in_mtx[i + i*size];
        for (k = 0; k < i; k++)
        {
            tmp = csub(tmp, cmul(out_mtx[k + i*size], (out_mtx[k + i*size])));
        }
        out_mtx[i + i*size] = cmsqrt(tmp, 2, 0);
    }
    return ret;
}

ret_err bot_tr_cmtx_invers(complexm* in_mtx, complexm* out_mtx, int size)
{
    ret_err ret = SUCCESS;
    if (in_mtx  == NULL)  return NO_MEM_ALLOCATED;
    if (out_mtx == NULL)  return NO_MEM_ALLOCATED;
    if (size    == 0)     return ZERO_SIZE;

    int i = 0, j = 0, k = 0;
    complexm s; init_reim(&s, 0, 0);
    complexm one; init_reim(&one, 1, 0);
    complexm mone; init_reim(&mone, -1, 0);
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            if (i == j)
            {
                if ((in_mtx[i + i*size].re == 0) && (in_mtx[i + i*size].im == 0)) return DIVISION_BY_ZERO;
                out_mtx[i + i*size] = cdiv(one, in_mtx[i + i*size]);
            }
            if (i < j)
                init_reim(&out_mtx[j + i*size], 0, 0);
        }
    }
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
            if ((i > j) && (i > 0) && (j < (size - 1)))
            {
                init_reim(&s, 0, 0);
                for (k = j; k <= i - 1; k++)
                    s = csum(cmul(in_mtx[k + i*size], out_mtx[j + k*size]), s);
                if ((in_mtx[i + i*size].re == 0) && (in_mtx[i + i*size].im == 0)) return DIVISION_BY_ZERO;
                out_mtx[j + i*size] = cmul(cdiv(mone, in_mtx[i + i*size]), s);
            }
    }
    return ret;
}

ret_err mtx_invers(complexm* in_mtx, complexm* out_mtx, int size, void* buffer)
{
    ret_err ret = SUCCESS;
    complexm* tmp = (complexm*)buffer;
    if (tmp     == NULL)  return NO_MEM_ALLOCATED;
    if (in_mtx  == NULL)  return NO_MEM_ALLOCATED;
    if (out_mtx == NULL)  return NO_MEM_ALLOCATED;
    if (size == 0)        return ZERO_SIZE;

    complexm* tmp1 = tmp + size*size;
    complexm* tmp2 = tmp1 + size*size;
    complexm* sub_b = tmp2 + size*size;

    ret = cholesky(in_mtx, tmp, size);
    if (ret != SUCCESS) return ret;

    ret = bot_tr_cmtx_invers(tmp, tmp1, size);
    if (ret != SUCCESS) return ret;

    mtx_transp(tmp1, tmp2, size);
    cmul_mtx_mtx(tmp2, tmp1, out_mtx, size, sub_b);
    return ret;
}

complexm* mtx_invers_alloc_buffer(int size)
{
    complexm*  ptr = (complexm*)malloc(4 * size * size * sizeof(complexm));
    int i = 0;
    for (i = 0; i < size*size*4; i++)
    {
        init_reim(&ptr[i], 0,0);
    }
    return ptr;
}

ret_err double_mtx_to_complexm(double* mtx1, complexm* mtx2, int size)
{
	ret_err ret = SUCCESS;
	int i = 0, j=0;
	for (i =0; i< size; i++)
	{
		for(j = 0; j< size; j++)
			init_reim(&mtx2[j+i*size], mtx1[j+i*size], 0);
	}
	return ret;
}


ret_err complexm_mtx_to_double(complexm* mtx1, double* mtx2, int size)
{
	ret_err ret = SUCCESS;
	int i = 0 , j = 0;
	for(i = 0; i<size; i++)
	{
		for(j = 0; j<size;j++)
			mtx2[j+i*size] = cmabs(&mtx1[j+i*size]);
	}
	return ret;
}

TEST (Filtering_functio_test, PositiveNos) {
	int size = 1000;
	int i = 0, j =0;
	complexm* mtx1 = cmtx_alloc(size);
	complexm* mtx2 = cmtx_alloc(size);
	complexm* mtx3 = cmtx_alloc(size);
	complexm* out = cmtx_alloc(size);
	complexm* buff = mtx_invers_alloc_buffer(size);

	for(i = 0; i < size;i++)
	{
		for(j = 0; j < size; j++)
		{
			if (i == j ){
				init_reim(&mtx2[i+j*size], 1, 0);
				init_reim(&mtx1[i + size*j], i+j+1, i);
			}
			else{
				init_reim(&mtx2[i+j*size], 0, 0);
				init_reim(&mtx1[i + size*j], 0, 0);
			}

		}
	}
	mtx_invers(mtx1, mtx3, size, buff);

	cmul_mtx_mtx(mtx1, mtx3, out, size, buff);

	for(i = 0; i < size;i++)
	{
		for(j = 0; j < size; j++)
		{
			EXPECT_NEAR(out[i+j*size].re, mtx2[i+j*size].re, 0.00000001);
			EXPECT_NEAR(out[i+j*size].im, mtx2[i+j*size].im, 0.00000001);
		}
	}

    free(mtx1);
    free(mtx2);
    free(mtx3);
    free(out);
    free(buff);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}



