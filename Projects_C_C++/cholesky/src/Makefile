m_parallel: m_parallel.o libcomplexm.a
	mpicc -o m_parallel m_parallel.o -L. -lcomplexm -lm

m_parallel.o: m_parallel.c
	mpicc -c m_parallel.c

libcomplexm.a: complexm.o
	ar cr libcomplexm.a complexm.o

complexm.o: complexm.c
	gcc -c complexm.c

clean:
	rm -f *.o *.a blocked_cholesky
