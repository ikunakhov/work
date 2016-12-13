// reloc_functions.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdlib.h>


void LU_decomposition(float* in, float* out, int n)
{
    float sum = 0.0;
    int i = 0, j = 0, k = 0;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            sum = 0;
            if (i <= j)
            {
                for (k = 0; k < i; k++)
                    sum += out[k + i*n] * out[j + k*n];
                    out[j + i*n] = in[j + i*n] - sum;
                
            }
            else
            {
                for (k = 0; k < j; k++)
                    sum += out[k + i*n] * out[j + k*n];
                if (out[j + j*n] == 0)
                    return;
                out[j + i*n] = (in[j + i*n] - sum) / out[j + j*n];
            }
        }
    }
}







int main()
{
    float in[] = { 1, 2, 1, 1 };
    float out[] = { 0, 0, 0, 0 };
    LU_decomposition(in, out, 2);
    return 0;
}

