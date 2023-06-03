#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "helps.h"


void countNumberPrimesInMatrizWithSerialMethod(int** matriz) {
	int primeNumbersInMatriz = countPrimesInMatriz(matriz);
	printf("Number of primes in matriz: %d\n", primeNumbersInMatriz);
}

int main() {
	int **matriz = genMatriz(GenMatrizParams{1000, 1000, 1, 31999});
	countNumberPrimesInMatrizWithSerialMethod(matriz);
}
