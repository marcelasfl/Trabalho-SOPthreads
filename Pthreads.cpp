#define HAVE_STRUCT_TIMESPEC
#pragma comment(lib,"pthreadVC2.lib")
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
	int **matriz;
	int rows;
	int cols;
} Matriz;

typedef struct
{
	int rowStart;
	int rowEnd;
	int colStart;
	int colEnd;
	bool processed;
} MacroBlock;

int SEED = 111;

#define MATRIZ_ROWS_SIZE 1000
#define MATRIZ_COLS_SIZE 1000

#define BLOCK_ROWS MATRIZ_ROWS_SIZE / 100
#define BLOCK_COLS MATRIZ_COLS_SIZE / 100
#define MACRO_BLOCK_MAX_SIZE (MATRIZ_ROWS_SIZE * MATRIZ_COLS_SIZE) / (BLOCK_ROWS * BLOCK_COLS)

#define NUMBER_THREAD 4

pthread_mutex_t MAIN_MUTEX;
pthread_mutex_t COUNT_MUTEX;

// Cria um ponteiro para uma matriz gerada aleatoriamente com dimensões MATRIZ_ROWS_SIZE x MATRIZ_COLS_SIZE
// A matriz é preenchida com números de 1 a 31999
Matriz *matriz;

MacroBlock *blocks;

#include "helps.h"

/**
 * Conta a quantidades de números primos na matriz fornecida usando o método serial.
 *
 * @param matriz A matriz para procurar números primos.
 */
void countNumberPrimesInMatrizWithSerialMethod(Matriz *matriz)
{
	// Conta os números primos na matriz.
	int primeNumbersInMatriz = countPrimesInMatriz(matriz);
	// Imprime a quantidade de números primos encontrados na matriz.
	printf("Números primos encontrados na matriz: %d\n", primeNumbersInMatriz);
}

/**
 * conta a quantidade de números primos dentro de uma determinada matriz usando phread e mutex.
 *
 * @param matriz A matriz para procurar números primos.
 */
void countNumberPrimesInMatrizWithParallelMethod(Matriz *matriz)
{
	// Criado os macrosblocos
	blocks = createMacroBlocksFromMatriz(matriz);

	// Inicializa o mutex para sincronização de thread.
	pthread_mutex_init(&MAIN_MUTEX, NULL);
	pthread_mutex_init(&COUNT_MUTEX, NULL);

	// Criar threads para contar números primos em cada "macrobloco".
	pthread_t threads[NUMBER_THREAD];
	for (int thread = 0; thread < NUMBER_THREAD; thread++)
	{
		pthread_create(&threads[thread], NULL, countPrimesInBlockWithThread, NULL);
	}

	// Aguarde a conclusão de todos os threads antes de continuar.
	for (int thread = 0; thread < NUMBER_THREAD; thread++) {
		pthread_join(threads[thread], NULL);
	}

	// Destruir o mutex depois que a sincronização de thread não for mais necessária.
	pthread_mutex_destroy(&MAIN_MUTEX);
	pthread_mutex_destroy(&COUNT_MUTEX);

	printf("Números primos encontrados no metodo paralelo: %d\n", PRIMES_NUMBER_COUT_IN_PARALLEL_METHOD);
}

int main()
{
	GenMatrizParams params;
	params.rows = MATRIZ_ROWS_SIZE;
	params.cols = MATRIZ_COLS_SIZE;
	params.min = 1;
	params.max = 31999;

	matriz = createRadomMatriz(params);
	// Conta a quantiade de números primos na matriz usando um método serial
	countNumberPrimesInMatrizWithSerialMethod(matriz);
	// Conta a quantiade de números primos na matriz usando um método pararelo
	countNumberPrimesInMatrizWithParallelMethod(matriz);
}
