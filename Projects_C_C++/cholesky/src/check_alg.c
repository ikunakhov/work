#include <stdio.h>
#include <math.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
int main()
{
	int i = 0 , j = 0, k = 0;
	double a[] = {1, 2, 3,
				2, 4, 1,
				3, 1, 5};
	for (i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			for (k = 0; MIN(i,j) - 1; k++)
				a[j + i*3] = a[j + i*3] - a[k +i*3]*a[j + k*3];
			if (i == j)
			{
				a[i +i*3] = sqrt(a[i+i*3]);
			}
			else
			{
				if (i < j)
					a[j + i*3] = a[j + i*3]/a[i+i*3];
				else
					a[j + i*3] = a[j+i*3]/a[j+j*3];
			}
			
		}
	}
	
	for (i = 0; i< 3; i++)
	{
		for(j = 0; j<3; j++)
		{
			printf("%lf ", a[j + i*3]);
		}
		printf("\n");
	}
	return 0;
}
