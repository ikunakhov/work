fine_grain_alg: fine_grain_alg.o libcomplexm.a
	mpicc -o fine_grain_alg fine_grain_alg.o -L. -lcomplexm -lm

fine_grain_alg.o: fine_grain_alg.c
	mpicc -c fine_grain_alg.c

libcomplexm.a: complexm.o
	ar cr libcomplexm.a complexm.o

complexm.o: complexm.c
	gcc -c complexm.c

clean:
	rm -f *.o *.a fine_grain_alg
