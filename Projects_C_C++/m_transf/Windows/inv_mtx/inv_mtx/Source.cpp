#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#define PI  3.14159265358979323846

typedef enum ret_err
{
    SUCCESS,
    WARNING,
    NO_MEM_ALLOCATED,
    ZERO_SIZE,
    DIVISION_BY_ZERO
}ret_err;


//==========================complex lib========================================///



typedef struct complex
{
    double re;
    double im;
    double r;
    double fi;
    bool reim;
    bool rfi;
}complex;

void init_reim(complex* c, double re, double im)
{
    c->re = re;
    c->im = im;
    c->reim = true;
}

void init_rfi(complex* c, double r, double fi)
{
    c->r = r;
    c->fi = fi;
    c->rfi = true;
}

void fill_rfi(complex* in)
{
    in->r = sqrt(in->re*in->re + in->im*in->im);
    in->fi = (-1)*atan2(in->re, in->im) + PI/(double)2;
    in->rfi = true;
}

void fill_reim(complex* in)
{
    in->re = in->r*cos(in->fi);
    in->im = in->r*sin(in->fi);
    in->reim = true;
}

void ccopy(complex* to_c1, complex* from_c2)
{
    memcpy(to_c1, from_c2, sizeof(complex));
}

complex cdiv(complex c1, complex c2)
{
    complex tmp; tmp.re = 0; tmp.im = 0;
    
    if (!c1.reim)
        fill_reim(&c1);
    if (!c2.reim)
        fill_reim(&c2);

    tmp.re = (c1.re * c2.re + c1.im * c2.im) / (c2.re*c2.re + c2.im*c2.im);
    tmp.im = (c1.im * c2.re - c1.re * c2.im) / (c2.re*c2.re + c2.im*c2.im);
    tmp.reim = true;
    tmp.rfi = false;
    return tmp;
}

complex cmul(complex c1, complex c2)
{
    if (!c1.reim)
        fill_reim(&c1);
    if (!c2.reim)
        fill_reim(&c2);

    complex tmp; tmp.re = 0; tmp.im = 0;
    tmp.re = c1.re * c2.re - c1.im * c2.im;
    tmp.im = c1.re * c2.im + c1.im * c2.re;
    tmp.reim = true;
    tmp.rfi = false;
    return tmp;
}

complex csum(complex c1, complex c2)
{
    if (!c1.reim)
        fill_reim(&c1);
    if (!c2.reim)
        fill_reim(&c2);

    complex tmp; tmp.re = 0; tmp.im = 0;
    tmp.re = c1.re + c2.re;
    tmp.im = c1.im + c2.im;
    tmp.reim = true;
    tmp.rfi = false;
    return tmp;
}

complex csub(complex c1, complex c2)
{
    if (!c1.reim)
        fill_reim(&c1);
    if (!c2.reim)
        fill_reim(&c2);

    complex tmp; tmp.re = 0; tmp.im = 0;
    tmp.re = c1.re - c2.re;
    tmp.im = c1.im - c2.im;
    tmp.reim = true;
    tmp.rfi = false;
    return tmp;
}

complex conj(complex c)
{
    complex ret;
    if (!c.reim)
        fill_reim(&c);
    ccopy(&ret, &c);
    ret.im *= double(-1);
    ret.reim = true;
    ret.rfi = false;
    return ret;
}

double cabs(complex* in)
{
    if (!in->rfi)
        fill_rfi(in);
    return in->r;
}

complex csqrt(complex in, int deg, int num)
{
    complex ret;
    double tmp = pow(cabs(&in), 1 / double(deg));
    double arg = double(in.fi + 2*PI*num) / double(deg);
    ret.re = tmp*cos(arg);
    ret.im = tmp*sin(arg);
    return ret;
}

void cswap(complex* c1,complex* c2)
{
    complex tmp;
    ccopy(&tmp, c1);
    ccopy(c1, c2);
    ccopy(c2, &tmp);
}

void mtx_transp(complex* mtx, complex* out, int size)
{
    int i = 0, j = 0;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
            ccopy(&(out[i*size + j]), &(mtx[j*size + i]));
    }    
}

void cmul_mtx_mtx(complex* mtx1, complex* mtx2, complex* out, int size, void* buffer)//, int threads)
{
    complex* mtx2T = (complex*)buffer;
    mtx_transp(mtx2, mtx2T, size);

    int i = 0, j = 0, k = 0;
    complex s;
//#pragma omp parallel for num_threads(threads)
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {   
            out[i * size + j].reim = true;
            out[i * size + j].re = 0;
            out[i * size + j].im = 0;
            out[i * size + j].rfi = false;
            s.re = 0; s.im = 0; s.reim = true; s.rfi = false;
            for (k = 0; k < size; k++)
                s  = csum(cmul(mtx1[i*size + k], mtx2T[j*size + k]),s);
            ccopy(&out[i*size + j],&s);
        }
    }
}

complex* cmtx_alloc(int size)
{
    complex* out = (complex*)malloc(size*size*sizeof(complex));
    int i = 0, j = 0;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            init_reim(&out[i + j*size], 0, 0);
            out[i + j*size].rfi = false;
        }
    }
    return out;

}

//==============================================================================//

ret_err cholesky(complex* in_mtx, complex* out_mtx, int size)
{
    ret_err ret = SUCCESS;
    if (in_mtx  == NULL)  return NO_MEM_ALLOCATED;
    if (out_mtx == NULL)  return NO_MEM_ALLOCATED;
    if (size == 0)        return ZERO_SIZE;

    int i = 0, j = 0, k = 0;
    complex tmp; init_reim(&tmp, 0, 0);

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < i; j++)
        {
            init_reim(&tmp, 0, 0);
            for (k = 0; k < j; k++)
            {
                tmp = csum(cmul(out_mtx[k + i*size], conj(out_mtx[k + j*size])), tmp);
            }
            if ((out_mtx[j + j*size].re == 0) && (out_mtx[j + j*size].im == 0)) return DIVISION_BY_ZERO;
            out_mtx[j + i*size] = cdiv(csub(in_mtx[i + j*size], tmp), out_mtx[j + j*size]);
        }
        tmp = in_mtx[i + i*size];
        for (k = 0; k < i; k++)
        {
            tmp = csub(tmp, cmul(out_mtx[k + i*size], (out_mtx[k + i*size]))) ;
        }
        out_mtx[i + i*size] = csqrt(tmp, 2, 0);
    }
    return ret;
}

ret_err bot_tr_cmtx_invers(complex* in_mtx, complex* out_mtx, int size)
{
    ret_err ret = SUCCESS;
    if (in_mtx  == NULL)  return NO_MEM_ALLOCATED;
    if (out_mtx == NULL)  return NO_MEM_ALLOCATED;
    if (size    == 0)     return ZERO_SIZE;

    int i = 0, j = 0, k = 0;
    complex s; init_reim(&s, 0, 0);
    complex one; init_reim(&one, 1, 0);
    complex mone; init_reim(&mone, -1, 0);
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

ret_err mtx_invers(complex* in_mtx, complex* out_mtx, int size, void* buffer)
{
    ret_err ret = SUCCESS;
    complex* tmp = (complex*)buffer;
    if (tmp     == NULL)  return NO_MEM_ALLOCATED;
    if (in_mtx  == NULL)  return NO_MEM_ALLOCATED;
    if (out_mtx == NULL)  return NO_MEM_ALLOCATED;
    if (size == 0)        return ZERO_SIZE;

    complex* tmp1 = tmp + size*size;
    complex* tmp2 = tmp1 + size*size;
    complex* sub_b = tmp2 + size*size;
    
    ret = cholesky(in_mtx, tmp, size);
    if (ret != SUCCESS) return ret;

    ret = bot_tr_cmtx_invers(tmp, tmp1, size);
    if (ret != SUCCESS) return ret;

    mtx_transp(tmp1, tmp2, size);
    cmul_mtx_mtx(tmp2, tmp1, out_mtx, size, sub_b);
    return ret;
}

complex* mtx_invers_alloc_buffer(int size)
{
    complex*  ptr = (complex*)malloc(4 * size * size * sizeof(complex));
    int i = 0;
    for (i = 0; i < size*size*4; i++)
    {
        init_reim(&ptr[i], 0,0);
    }
    return ptr;
}


int main()
{
    complex* mtx1 = cmtx_alloc(3);
    complex* mtx2 = cmtx_alloc(3);
    complex* tmp = cmtx_alloc(3);
    complex* tmp2 = cmtx_alloc(3);
    complex* buff = mtx_invers_alloc_buffer(3);
    init_reim(&mtx1[0], 5, 0);
    init_reim(&mtx1[1], 20, 0);
    init_reim(&mtx1[2], 1, 0);
    init_reim(&mtx1[3], 20, 0);
    init_reim(&mtx1[4], 3, 0);
    init_reim(&mtx1[5], -1, 0);
    init_reim(&mtx1[6], 1, 0);
    init_reim(&mtx1[7], -1, 0);
    init_reim(&mtx1[8], 0, 0);
    for (int i = 0; i < 9; i++)
        mtx1[i].rfi = false;

    mtx_invers(mtx1, mtx2, 3, buff);
    for (int i = 0; i < 3 * 3; i++) {
        printf("%lf ", mtx2[i].re);
        printf("%lf \n", mtx2[i].im);
    }
    free(mtx1);
    free(mtx2);
    free(tmp);
    free(buff);
    return 0;

}