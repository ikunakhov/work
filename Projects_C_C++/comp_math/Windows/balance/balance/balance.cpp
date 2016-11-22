// balance.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

double pre = 0.0;
int num_t = 0;

struct info {
    double x1;
    double x2;
    double step;
    double out;
};
typedef struct info info;


struct info2 {
    double* ptr;
    int length;
    double out;
};
typedef struct info2 info2;

double f(double x)

{
    return 1 / x;
}

double df(double x)
{
    return (-1)/(x*x);

}

void calc_a_line(info *args)
{
    info* arg = (info*)args;
    double x1 = arg->x1;
    double x2 = arg->x2;
    double dx = arg->step;
    double sum = 0.0;

    while (fabs(x2 - x1) >= fabs(dx))
    {
        sum += dx * (f(x1) + f(x1 + dx)) / 2.0;
        x1 = x1 + dx;
    }
    arg->out = sum;
}

bool runge_rul(double i_2n, double i_n)
{
    if ((fabs(i_2n - i_n) / 3.0) <= pre)
        return true;
    else
        return false;
}

void* re_exec(void *args)
{
    info* arg = (info*)args;
    bool check = false;
    double out1, out2;
    arg->step = (arg->x2 - arg->x1);
    calc_a_line(arg);
    out1 = arg->out;

    while (check == false)
    {
        arg->step /= 2.0;
        calc_a_line(arg);
        out2 = arg->out;
        check = runge_rul(out1, out2);
        out1 = out2;
    }
    arg->out = out1;
    return 0;
}

void* find_avg(void *args)
{
    info* arg = (info*)args;
    double from = arg->x1;
    double to = arg->x2;
    double step = arg->step;
    double ret = 0.0;
    int cnt = 1;
    while (from < to)
    {
        ret += df(from);
        from += step;
        cnt++;
    }
    ret /= (double)cnt;
    arg->out = fabs(ret);
    return 0;
}

double find_min(info* ptr, int length)
{
    double ret = ptr[0].out;
    int i;
    for (i = 1; i < length; i++)
    {
        if (ptr[i].out < ret)
            ret = ptr[i].out;
    }
    return ret;
}


int main(int argc, char* argv[])
{

    double in_from = atof(argv[1]);
    double in_to = atof(argv[2]);
    pre = atof(argv[3]);
    num_t = atoi(argv[4]);
    info *str_a;
    pthread_t *threads;
    threads = (pthread_t*)malloc(num_t*sizeof(pthread_t));
    str_a = (info*)malloc(num_t*sizeof(info));

    double patch = (in_to - in_from) / num_t;
    double output = 0.0;
    int i;

    for (i = 0; i < num_t; i++)
    {
        str_a[i].x1 = in_from + i * patch;
        str_a[i].x2 = str_a[i].x1 + patch;
        str_a[i].step = pre;
        int sts = pthread_create(&threads[i], NULL, &find_avg, (void*)&str_a[i]);
    }
    for (i = 0; i < num_t; i++)
        pthread_join(threads[i], NULL);


    double norm = find_min(str_a, num_t);
    int* tmp_mass = (int*)malloc(num_t*sizeof(int));
    double* patch_mass = (double*)malloc(num_t*sizeof(double));
    int all = 0;

    for (i = 0; i < num_t; i++)
    {
        str_a[i].out /= norm;
        tmp_mass[i] = trunc(str_a[i].out);
        printf("%f====%d\n", patch, tmp_mass[i]);
        patch_mass[i] = patch / tmp_mass[i];
        printf(" ---%f-patch/tmp \n", patch_mass[i]);
        all += tmp_mass[i];
    }

    i = 0;
    int ii = 0;
    int cnt = 0;
    double res = patch_mass[cnt];
    info* str_b = (info*)malloc(all*sizeof(info));
    int wait = 0;
    while (cnt < num_t)
    {
        for (i = 0; i < num_t; i++)
        {
            str_b[ii].x1 = in_from;
            str_b[ii].x2 = in_from + res;
            int sts = pthread_create(&threads[i], NULL, &re_exec, (void*)&str_b[ii]);
            printf("%f %f  %f--ret\n", str_b[ii].x1, str_b[ii].x2, str_b[ii].out);
            in_from += res;
            tmp_mass[cnt]--;
            if (tmp_mass[cnt] <= 0)
                cnt++;
            ii++;
            wait = i;
            if (cnt >= num_t) { break; }
            res = patch_mass[cnt];
            if (in_from >= in_to) break;
        }
        for (i = 0; i <= wait; i++)
        {
            pthread_join(threads[i], NULL);
        }
    }
    for (int k = 0; k < ii; k++)
        printf("  ---%f----\n", str_b[k].out);
    double ret = 0.0;
    printf("%d\n", ii);
    for (i = 0; i < ii; i++)
        ret += str_b[i].out;
    printf("  %.20lf \n", ret);


    return 0;
}



