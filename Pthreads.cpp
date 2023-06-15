#define HAVE_STRUCT_TIMESPEC
#pragma comment(lib, "pthreadVC2.lib")
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tgmath.h>
#include <sys/time.h>

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

typedef struct
{
	int rows;
	int cols;
	int min;
	int max;
} GenMatrizParams;

int SEED = 111;

#define MATRIZ_ROWS_SIZE 15000
#define MATRIZ_COLS_SIZE 15000

#define BLOCK_ROWS 100
#define BLOCK_COLS 100
#define MACRO_BLOCK_MAX_SIZE (MATRIZ_ROWS_SIZE * MATRIZ_COLS_SIZE) / (BLOCK_ROWS * BLOCK_COLS)

#define NUMBER_THREAD 4

pthread_mutex_t VERIFICATION_MUTEX;
pthread_mutex_t COUNT_MUTEX;

// Cria um ponteiro para uma matriz gerada aleatoriamente com dimensões MATRIZ_ROWS_SIZE x MATRIZ_COLS_SIZE
// A matriz é preenchida com números de 1 a 31999
Matriz *matriz;

MacroBlock *blocks;

void printMemoryAllocationError(const char *file, int line)
{
	printf("Falha na alocação de memória.\n arquivo: %s\n linha: %d\n", file, line);
	exit(0);
}

Matriz *createRadomMatriz(GenMatrizParams params)
{
	srand(SEED);
	Matriz *matriz = (Matriz *)malloc(sizeof(Matriz));
	matriz->rows = params.rows;
	matriz->cols = params.cols;
	matriz->matriz = (int **)malloc(matriz->rows * sizeof(int *));
	for (int i = 0; i < matriz->rows; i++)
	{
		matriz->matriz[i] = (int *)malloc(matriz->cols * sizeof(int));
		for (int j = 0; j < matriz->cols; j++)
		{
			matriz->matriz[i][j] = rand() % (params.max - params.min + 1) + params.min;
		}
	}
	return matriz;
}

int ehPrimo(int num)
{
	if (num <= 1)
		return false;

	if (num <= 3)
		return true;

	if (num % 2 == 0 || num % 3 == 0)
		return false;

	int sqrtN = sqrt(num);
	for (int i = 5; i <= sqrtN; i += 6)
	{
		if (num % i == 0 || num % (i + 2) == 0)
			return false;
	}

	return true;
}

MacroBlock *createMacroBlocksFromMatriz()
{
	MacroBlock *blocks = (MacroBlock *)malloc(sizeof(MacroBlock) * MACRO_BLOCK_MAX_SIZE);
	if (blocks == NULL)
		printMemoryAllocationError(__FILE__, __LINE__);

	int index = 0;
	for (int row = 0; row < matriz->rows; row += BLOCK_ROWS)
	{
		for (int col = 0; col < matriz->cols; col += BLOCK_COLS, index++)
		{
			blocks[index].rowStart = row;
			blocks[index].rowEnd = row + BLOCK_ROWS;
			blocks[index].colStart = col;
			blocks[index].colEnd = col + BLOCK_COLS;
			blocks[index].processed = false;
		}
	}
	return blocks;
}

int countPrimesNumbersBetweenStartAndEndBlockInMatriz(MacroBlock block)
{
	int primeNumbersInBlock = 0;
	for (int i = block.rowStart; i < block.rowEnd; i++)
	{
		for (int j = block.colStart; j < block.colEnd; j++)
		{
			if (ehPrimo(matriz->matriz[i][j]))
			{
				primeNumbersInBlock++;
			}
		}
	}
	return primeNumbersInBlock;
}

int PRIMES_NUMBER_COUNT_IN_PARALLEL_METHOD = 0;

/**
 * Esta função conta a qauntidade de números primos em um determinado bloco usando multithreading.
 *
 * @param param - null
 * @return NULL
 */
int CURRENT_BLOCK_INDEX_COUNT = 0;
void *countPrimesInBlockWithThread(void *param)
{
	while (1)
	{
		// Verifica se há mais blocos a serem processados
		if (CURRENT_BLOCK_INDEX_COUNT >= MACRO_BLOCK_MAX_SIZE)
			pthread_exit(NULL);

		// Bloqueia a main mutex apenas para modificar a variável visitedBlocks
		pthread_mutex_lock(&VERIFICATION_MUTEX);
		MacroBlock block = blocks[CURRENT_BLOCK_INDEX_COUNT++];
		// Desbloqueia a main mutex
		pthread_mutex_unlock(&VERIFICATION_MUTEX);

		// Conta os números primos no macrobloco
		int primeNumbersInBlock = countPrimesNumbersBetweenStartAndEndBlockInMatriz(block);

		// Bloqueia a COUNT_MUTEX
		pthread_mutex_lock(&COUNT_MUTEX);
		// Adiciona a quantidade de números primos encontrados
		PRIMES_NUMBER_COUNT_IN_PARALLEL_METHOD += primeNumbersInBlock;
		// Desbloqueia a COUNT_MUTEX
		pthread_mutex_unlock(&COUNT_MUTEX);
	}

	// Retorna NULL para satisfazer o tipo de retorno void
	return NULL;
}

void printMatriz(Matriz *matriz)
{
	for (int i = 0; i < matriz->rows; i++)
	{
		for (int j = 0; j < matriz->cols; j++)
		{
			printf("%d ", matriz->matriz[i][j]);
		}
		printf("\n");
	}
}

void printMacroBlocks(MacroBlock *blocks)
{
	for (int i = 0; i < MACRO_BLOCK_MAX_SIZE; i++)
	{
		printf("%d ", blocks[i].rowStart);
		printf("%d ", blocks[i].rowEnd);
		printf("%d ", blocks[i].colStart);
		printf("%d ", blocks[i].colEnd);
		printf("\n");
	}
}

int countPrimesInMatriz()
{
	int primeNumbersInMatriz = 0;
	for (int i = 0; i < matriz->rows; i++)
	{
		for (int j = 0; j < matriz->cols; j++)
		{
			if (ehPrimo(matriz->matriz[i][j]))
			{
				primeNumbersInMatriz++;
			}
		}
	}
	return primeNumbersInMatriz;
}

/**
 * Retorna o tempo decorrido desde uma hora de início fornecida até a hora atual.
 *
 * @param timeStart a hora de início para medir o tempo decorrido.
 * @return o tempo decorrido em segundos.
 */
double getTimeEnd(struct timeval startTime)
{
	struct timeval endTime;
	double elapsed;
	
	// Obter o tempo de término
	gettimeofday(&endTime, NULL);

	// Calcular o tempo decorrido em segundos
	elapsed = (endTime.tv_sec - startTime.tv_sec) +
								 (endTime.tv_usec - startTime.tv_usec) / 1000000.0;

	return elapsed;
}

/**
 * Conta a quantidades de números primos na matriz fornecida usando o método serial.
 */
void countNumberPrimesInMatrizWithSerialMethod()
{
	struct timeval startTime;
	gettimeofday(&startTime, NULL);
	// Conta os números primos na matriz.
	int primeNumbersInMatriz = countPrimesInMatriz();
	// Imprime o tempo de execução.
	printf("Tempo de execução no modo serial: %.4f segundos\n", getTimeEnd(startTime));
	// Imprime a quantidade de números primos encontrados na matriz.
	printf("Números primos encontrados na matriz: %d\n", primeNumbersInMatriz);
}

/**
 * conta a quantidade de números primos dentro de uma determinada matriz usando phread e mutex.
 */
void countNumberPrimesInMatrizWithParallelMethod()
{
	// Criado os macrosblocos
	blocks = createMacroBlocksFromMatriz();

	// Inicializa o mutex para sincronização de thread.
	pthread_mutex_init(&VERIFICATION_MUTEX, NULL);
	pthread_mutex_init(&COUNT_MUTEX, NULL);

	// Criar threads para contar números primos em cada "macrobloco".
	pthread_t threads[NUMBER_THREAD];
	for (int thread = 0; thread < NUMBER_THREAD; thread++)
	{
		pthread_create(&threads[thread], NULL, countPrimesInBlockWithThread, NULL);
	}

	struct timeval startTime;
	gettimeofday(&startTime, NULL);

	// Aguarde a conclusão de todos os threads antes de continuar.
	for (int thread = 0; thread < NUMBER_THREAD; thread++)
	{
		pthread_join(threads[thread], NULL);
	}

	printf("Tempo de execução no modo paralelo: %.4f segundos\n", getTimeEnd(startTime));

	// Destruir o mutex depois que a sincronização de thread não for mais necessária.
	pthread_mutex_destroy(&VERIFICATION_MUTEX);
	pthread_mutex_destroy(&COUNT_MUTEX);

	printf("Números primos encontrados no metodo paralelo: %d\n", PRIMES_NUMBER_COUNT_IN_PARALLEL_METHOD);
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
	countNumberPrimesInMatrizWithSerialMethod();

	// Conta a quantiade de números primos na matriz usando um método pararelo
	countNumberPrimesInMatrizWithParallelMethod();
}