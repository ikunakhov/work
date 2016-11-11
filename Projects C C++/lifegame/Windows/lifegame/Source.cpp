#include <iostream>
#include <stdlib.h>

typedef enum Border_type
{
    up,
    down,
    left,
    right,
    up_right,
    up_left,
    down_right,
    down_left,
    none
}Border_type;


short live_or_die(short* ptr, int size, Border_type type)
{
    short ret = 0;
    short sum = 0;
    if (type == none)
    {
        sum = *(ptr + 1) + *(ptr - 1) + \
              *(ptr + size) + *(ptr + size + 1) + *(ptr + size - 1) + \
              *(ptr - size) + *(ptr - size + 1) + *(ptr - size - 1);
    }
    if (type == up)
    {
        sum = *(ptr + 1) + *(ptr - 1) + \
              *(ptr + size) + *(ptr + size + 1) + *(ptr + size - 1);
    }
    if (type == down)
    {
        sum = *(ptr + 1) + *(ptr - 1) + \
              *(ptr - size) + *(ptr - size + 1) + *(ptr - size - 1);
    }
    if (type == left)
    {
        sum = *(ptr + 1) + \
              *(ptr + size) + *(ptr + size + 1) +\
              *(ptr - size) + *(ptr - size + 1);
    }
    if (type == right)
    {
        sum = *(ptr - 1) + \
              *(ptr + size) + *(ptr + size - 1) + \
              *(ptr - size) + *(ptr - size - 1);
    }
    if (type == up_left)
    {
        sum = *(ptr + 1) + \
              *(ptr + size) + *(ptr + size + 1);
    }
    if (type == up_right)
    {
        sum = *(ptr - 1) + \
              *(ptr + size) + *(ptr + size - 1);
    }
    if (type == down_left)
    {
        sum = *(ptr + 1) + \
              *(ptr - size) + *(ptr - size + 1);
    }
    if (type == down_right)
    {
        sum = *(ptr - 1) + \
              *(ptr - size) + *(ptr - size - 1);
    }

    if ((sum == 2) || (sum == 3))
        ret = 1;
    return ret;
}

void exec(short* map, int size, short* buffer)
{
    short* tmp_map = buffer;
    Border_type type = none;
#pragma omp parallel for num_threads(4)
    for (int i = 0; i < size*size; i++)
    {
        type = none;
        if (i == 0)
            type = up_left;
        if (i == size - 1)
            type = up_right;
        if ((i > 0) && (i < size - 1))
            type = up;
        if (i == size*(size - 1))
            type = down_left;
        if (i == size*size - 1)
            type = down_right;
        if ((i > (size*(size - 1))) && (i < (size*size - 1)))
            type = down;
        if (((i % size) == 0) && (type == none))
            type = left;
        if (((i % size) == (size - 1)) && (type == none))
            type = right;
        tmp_map[i] = live_or_die((map + i), size, type);
    }
    for (int i = 0; i < size*size; i++)
        map[i] = tmp_map[i];
}



int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cout << "wrong args!";
        return 0;
    }

    int size = atoi(argv[1]);
    int iter = atoi(argv[2]);

    short* test = new short[size*size];

    for (int i = 0; i < size*size; i++)
        test[i] = rand() % 2;

    for (int i = 0; i < size*size; i++)
    {
        if (test[i] == 0)
            std::cout << "-";
        if(test[i] == 1)
            std::cout << "*";
        if ((i % size) == (size - 1))
            std::cout << std::endl;
    }
    short* buf = (short*)malloc(size * size * sizeof(short));
    for (int j = 0; j < iter; j++)
    {
        exec(test, size, buf);
    }
    std::cout << std::endl;
    for (int i = 0; i < size*size; i++)
    {
        if (test[i] == 0)
            std::cout << "-";
        if (test[i] == 1)
            std::cout << "*";
        if (i % size == size - 1)
            std::cout << std::endl;
    }
}