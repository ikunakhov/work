// 2_methods.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>

#define EPS 0.0005


struct m_size
{
    int width;
    int height;
};

typedef struct m_size m_size;

void thr_diagonal_sweep(double** src_matrix, m_size size, double** dst, double* src_rhs)
{
    double* alpha = new double[size.width + 1]; if (alpha == NULL) throw std::bad_alloc();
    double* betta = new double[size.width + 1]; if (betta == NULL) throw std::bad_alloc();

    alpha[1] = (src_matrix[0][1]) / (src_matrix[0][0])*(-1);
    betta[1] = (src_rhs[0]) / (src_matrix[0][0]);
    for (int i = 1; i < size.width - 1; i++)
    {
       alpha[i + 1] = (src_matrix[i][i + 1]) / ((src_matrix[i][i]) + alpha[i]* (src_matrix[i][i - 1]))*(-1);
       betta[i + 1] = ((src_rhs[i]) - betta[i]*(src_matrix[i][i - 1])) / ((src_matrix[i][i]) + alpha[i] * (src_matrix[i][i - 1]));
    }

    dst[0][size.width - 1] = (src_rhs[size.width - 1] - (src_matrix[size.width - 1][size.width - 2])*betta[size.width - 1]) / ((src_matrix[size.width - 1][size.width - 1]) + (src_matrix[size.width - 1][size.width - 2])*alpha[size.width - 1]);

    for (int i = size.width - 2; i >= 0; i--)
    {
        dst[0][i] = alpha[i + 1] * (dst[0][i + 1]) + betta[i + 1];
    }
    delete[] alpha;
    delete[] betta;
}

template <typename Type>
Type** create_matrix(m_size size, Type ini_val)
{
    Type **matrix = NULL;
    matrix = new Type*[size.width]; if (matrix == NULL) throw std::bad_alloc();
    for (int i = 0; i < size.width; i++)
    {
        matrix[i] = new Type[size.height]; if (matrix[i] == NULL) throw std::bad_alloc();
        for (int j = 0; j < size.height; j++)
            matrix[i][j] = ini_val;
    }
    return matrix;
}
template <typename Type>
Type* create_bar(int height, Type ini_val)
{
    Type *bar;
    bar = new Type[height]; if (bar == NULL) throw std::bad_alloc();
    for (int i = 0; i < height; i++)
        bar[i] = ini_val;
    return bar;
   
}


template  <typename Type>
void destroy_bar(Type *bar)
{
    delete[] bar;
}

template <typename Type>
void destroy_matrix(Type **matrix_ptr, m_size size)
{
    for (int i = 0; i < size.width; i++)
    {
        delete[] matrix_ptr[i];
    }
    delete[] matrix_ptr;
}

void usage()
{
    std::cout << " \n"\
              << " 2_methods.cpp builds the numerical solution of differential equations using 2 methods \n"\
              << " y''(x) - 2y(x)/(x+1)^2 = 9/[2(x+1)^(3/2)] - differential equation \n"\
              << " y(0) = 0, y(1) - 4y'(1) = 0 \n";
}



int main(int argc, char* argv[])
{
    int h_w = 1000;
    if (argc != 1)
    {
        usage();
        return 0;
    }


    m_size size;
    size.width = h_w;
    size.height = h_w;

    double** matrix = create_matrix(size, 0.0);
    double* x       = create_bar(size.height, 0.0);
    double* rhs     = create_bar(size.height, 0.0);

    double h = 1 / double(h_w);
    
    matrix[0][0] = h*h * 2 - 2;
    matrix[0][1] = 1.0;
    matrix[h_w - 1][h_w - 1] = h*h * 2 / (((h_w - 1)*h + 1)*((h_w - 1)*h + 1)) - 2 + 4 / (4 - h_w - 1);
    matrix[h_w - 1][h_w - 2] = 1.0;
    for (int i = 1; i < h_w - 1; i++)
    {
        matrix[i][i] = h*h * 2.0 / ((i*h + 1.0)*(i*h + 1.0)) - 2.0;
        matrix[i][i + 1] = 1.0;
        matrix[i][i - 1] = 1.0;
    }

    for (int i = 0; i < h_w; i++)
        rhs[i] = 9.0 / (2.0 * powf(i*h + 1, 3 / 2));

    thr_diagonal_sweep(matrix, size, &x, rhs);
    for (int i = 0; i < h_w; i++)
    {
        std::cout << x[i] <<" ";
    }

    double* y = create_bar(size.height + 1, 0.0);
    double* p = create_bar(size.height + 1, 0.0);

    bool contin = true;
    double a = -15.0;
    y[0] = 0;

    while (contin == true)
    {
        p[0] = a;
        for (int i = 1; i <= h_w; i++)
        {
            y[i] = y[i - 1] + h*p[i - 1];
            p[i] = p[i - 1] + h*(2.0*y[i - 1] / ((((i - 1)*h) + 1)*(((i - 1)*h) + 1)) + rhs[i - 1]);
        }
        if (fabs((p[h_w] - y[h_w] / 4.0)) < EPS)
            contin = false;
        else
            a += 0.0001;
    }
    std::cout << "/n";
    for (int i = 0; i < h_w; i++)
    {
        std::cout << y[i] << " ";
    }

    return 0;
}

