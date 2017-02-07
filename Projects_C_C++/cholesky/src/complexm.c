#include "complexm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


complexm double2complexm(double in)
{
    complexm ret;
    init_reim(&ret,in, 0);
    return ret;
}

void double_mtx2complexm_mtx(double* src_mtx, complexm* dst_mtx, int c, int r)
{
    int i = 0, j = 0;
    for (i = 0; i < r; i++)
    {
        for(j = 0; j < c; j++)
        {
            dst_mtx[j+c*i] = double2complexm(src_mtx[j+c*i]);
        }
    }
}

double complexm2double(complexm in)
{
    return in.r;
}

void complexm_mtx2double_mtx(complexm* src_mtx, double* dst_mtx, int c, int r)
{
    int i = 0, j = 0;
    for (i = 0; i < r; i++)
    {
        for(j = 0; j < c; j++)
        {
            dst_mtx[j+c*i] = complexm2double(src_mtx[j+c*i]);
        }
    }
}

void fill_rfi(complexm* in)
{
    in->r = sqrt(in->re*in->re + in->im*in->im);
    in->fi = (-1)*atan2(in->re, in->im) + PI/(double)2;
}

void fill_reim(complexm* in)
{
    in->re = in->r*cos(in->fi);
    in->im = in->r*sin(in->fi);
}

void init_reim(complexm* c, double re, double im)
{
    c->re = re;
    c->im = im;
    fill_rfi(c);
}



void init_rfi(complexm* c, double r, double fi)
{
    c->r = r;
    c->fi = fi;
    fill_reim(c);
}

void ccopy(complexm* to_c1, complexm* from_c2) //c1 - destination c2 - source
{
    memcpy(to_c1, from_c2, sizeof(complexm));
}

complexm cdiv(complexm c1, complexm c2)
{
    complexm tmp;
    double re = (c1.re * c2.re + c1.im * c2.im) / (c2.re*c2.re + c2.im*c2.im);
    double im = (c1.im * c2.re - c1.re * c2.im) / (c2.re*c2.re + c2.im*c2.im);
    init_reim(&tmp,re,im);
    return tmp;
}

complexm cmul(complexm c1, complexm c2)
{
    complexm tmp;
    double re = c1.re * c2.re - c1.im * c2.im;
    double im = c1.re * c2.im + c1.im * c2.re;
    init_reim(&tmp, re,im);
    return tmp;
}

complexm csum(complexm c1, complexm c2)
{
    complexm tmp;
    double re = c1.re + c2.re;
    double im = c1.im + c2.im;
    init_reim(&tmp, re, im);
    return tmp;
}

complexm csub(complexm c1, complexm c2)
{
    complexm tmp;
    double re = c1.re - c2.re;
    double im = c1.im - c2.im;
    init_reim(&tmp, re, im);
    return tmp;
}

complexm cmonj(complexm c)
{
    complexm ret;
    ccopy(&ret, &c);
    ret.im *= (double)(-1);
    return ret;
}

double cmabs(complexm* in)
{
    return in->r;
}

complexm cmsqrt(complexm in, int deg, int num)
{
    complexm ret;
    double tmp = pow(cmabs(&in), 1 / (double)(deg));
    double arg = (double)(in.fi + 2*PI*num) / (double)(deg);
    double re = tmp*cos(arg);
    double im = tmp*sin(arg);
    init_reim(&ret, re, im);
    return ret;
}

void cmswap(complexm* c1,complexm* c2)
{
    complexm tmp;
    ccopy(&tmp, c1);
    ccopy(c1, c2);
    ccopy(c2, &tmp);
}

void printfcm_reim(complexm c)
{
    if (c.im != 0.0)
        printf("%.2lf %.2lfi ",c.re,c.im);
    else
        printf("%.2lf       ",c.re);
}

void printfcm_rfi(complexm c)
{

    if(c.fi != 0.0)
        printf("%.2lfexp(i*%.2lf) ", c.r, c.fi);
    else
        printf("%.2lf       ", c.r);
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

void cmtx_free(complexm* src)
{
    free(src);
}
