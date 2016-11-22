#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define PI 3.14159265358979323846

typedef struct complex
{
    double re;
    double im;
}complex;

complex cdiv(complex c1, complex c2)
{
    complex tmp; tmp.re = 0; tmp.im = 0;
    tmp.re = (c1.re * c2.re + c1.im * c2.im) / (c2.re*c2.re + c2.im*c2.im);
    tmp.im = (c1.im * c2.re - c1.re * c2.im) / (c2.re*c2.re + c2.im*c2.im);
    return tmp;
}

complex cmul(complex c1, complex c2)
{
    complex tmp; tmp.re = 0; tmp.im = 0;
    tmp.re = c1.re * c2.re - c1.im * c2.im;
    tmp.im = c1.re * c2.im + c1.im * c2.re;
    return tmp;
}

complex csum(complex c1, complex c2)
{
    complex tmp; tmp.re = 0; tmp.im = 0;
    tmp.re = c1.re + c2.re;
    tmp.im = c1.im + c2.im;
    return tmp;
}

complex csub(complex c1, complex c2)
{
    complex tmp; tmp.re = 0; tmp.im = 0;
    tmp.re = c1.re - c2.re;
    tmp.im = c1.im - c2.im;
    return tmp;
}


complex* buffer_alloc(int size)
{
    int ctr = size;
    while (size)
    {
        size = size / 2;
        ctr += size;
    }
    complex* tmp = (complex*)malloc(ctr*sizeof(complex));
    return tmp;
}

void fft(complex* io, int size, complex* buffer)
{
    if (size == 1) return;
    complex* a1 = buffer;
    complex* a2 = buffer + (size / 2);
    int i = 0, j = 0;
    for (i = 0, j = 0; i < size; i += 2, ++j)
    {
        a1[j] = io[i];
        a2[j] = io[i + 1];
    }
    buffer += size;

    fft(a1, size / 2, buffer);
    fft(a2, size / 2, (buffer + size / 2));

    double fi = -2 * PI / (double)size;
    complex w; w.re = 1; w.im = 0;
    complex wn; wn.re = cosl(fi); wn.im = sinl(fi);
    for (i = 0; i < size / 2; i++)
    {
        io[i] = csum(a1[i],cmul(w, a2[i]));
        io[i + size / 2] = csub(a1[i], cmul(w, a2[i]));
        w = cmul(w, wn);
    }
}






int main(int argc, char* argv[])
{
    const int length = 8;
    complex test[length];
    int i = 0;
    for (i = 0; i < length; i++)
    {
        test[i].re = (i + 1);
        test[i].im = 0;
    }
    complex* buf = buffer_alloc(length);
    fft(test, length, buf);
    return 0;
}



