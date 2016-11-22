#include <cstdlib>
#include <iostream>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <fstream>
#include <QVector>
#include <iomanip> 
#include <cmath> 


using namespace std;
double func(double x);
int main(int argc, char *argv[])
{
	QString a1 = argv[1]; QString b1 = argv[2];
	if (argc == 4)
	{
		double a = a1.toDouble(); double b = b1.toDouble();
		QString num = argv[3];
		int num_iter = num.toInt();
		int i = 0, n = 0;
		double max = 0;
		double step = 0, sum = 0;

		for (i = 0; i < num_iter; i++)
		{
			step = (rand() % 10)*0.1 + rand() % (int)(b - a) + (rand() % 10)*0.01;
			if (b > a) max = a + step; else max = b + step;
			sum += func(max);
			n++;
		}
		sum = sum * (b - a) / n;
		cout << sum;
	}
	if (argc == 5)
	{
		int n = a1.toInt(); double r = b1.toDouble();
		QVector<double> ran;
		ran.resize(n);
		QString num = argv[3];
		int num_iter = num.toInt();
		int i =0, j = 0 ;
		int p_in = 0;
		double v = 1;
		double sum = 0, rrt;
		for (j = 0; j < n; j++)
		{
			v *= (2 * r);
		}


		for (i = 0; i < num_iter; i++)
		{
			for (j = 0; j < n; j++)
			{
				ran[j] = rand() % (2 * int(r)) + (rand() % 10)*0.01 + (rand() % 10)*0.1;
			}

			for (j = 0; j < n; j++)
			{
				sum += (ran[j] - r)*(ran[j] - r);
			}
			if (sum <= r*r)
				p_in++;
			sum = 0;
		}

		rrt = (p_in / (double)num_iter) * v;
		cout << fixed << setprecision(8)<< rrt;
		

	}

	return 0;
}

double func(double x)
{
	return log(x);
}