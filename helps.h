#include <stdio.h>
#include <time.h>
#include <stdlib.h>

struct GenMatrizParams
{
  int rows;
  int cols;
  int min;
  int max;
};

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

int isNumberPrime(int num)
{
  if (num < 2)
  {
    return 0;
  }

  for (int i = 2; i <= num / 2; i++)
  {
    if (num % i == 0)
    {
      return 0;
    }
  }

  return 1;
}


MacroBlock *createMacroBlocksFromMatriz(Matriz *matriz)
{
  MacroBlock *blocks = (MacroBlock *)malloc(sizeof(MacroBlock) * MACRO_BLOCK_MAX_SIZE);
  int index = 0;
  for (int row = 0; row < matriz->rows; row += BLOCK_ROWS) {
    for (int col = 0; col < matriz->cols; col += BLOCK_COLS, index++) {
      blocks[index] = {
        .rowStart = row,
        .rowEnd = row + BLOCK_ROWS,
        .colStart = col,
        .colEnd = col + BLOCK_COLS,
        .processed = false
      };
    }
  }
  return blocks;
}

int countPrimesNumbersBetweenStartAndEndBlockInMatriz(MacroBlock block, Matriz *matriz) {
  int primeNumbersInBlock = 0;
  for (int i = block.rowStart; i < block.rowEnd; i++)
  {
    for (int j = block.colStart; j < block.colEnd; j++)
    {
      if (isNumberPrime(matriz->matriz[i][j]))
      {
        primeNumbersInBlock++;
      }
    }
  }
  return primeNumbersInBlock;
}

int PRIMES_NUMBER_COUT_IN_PARALLEL_METHOD = 0;
/**
 * Esta função conta a qauntidade de números primos em um determinado bloco usando multithreading.
 *
 * @param param - um ponteiro para uma estrutura MacroBlock contendo o bloco para contar números primos em
 * @return NULL
 */
void *countPrimesInBlockWithThread(void *param)
{
  int canCount = false;
  for (int blockindex = 0; blockindex < MACRO_BLOCK_MAX_SIZE; blockindex++)
  {
    // Bloqueia a main mutex para garantir a segurança do thread
    pthread_mutex_lock(&MAIN_MUTEX);
    // Verifica se esse ainda não já foi precessado por outra thread
    if (!blocks[blockindex].processed)
    {
      // marca o bloco como processado
      blocks[blockindex].processed = true;
      // contar os numeros primos
      canCount = true;
    }
    // Desbloqueia a main mutex
    pthread_mutex_unlock(&MAIN_MUTEX);

    int primeNumbersInBlock = canCount ? countPrimesNumbersBetweenStartAndEndBlockInMatriz(blocks[blockindex], matriz) : 0;
    
    pthread_mutex_lock(&COUNT_MUTEX);
    PRIMES_NUMBER_COUT_IN_PARALLEL_METHOD += primeNumbersInBlock;
    pthread_mutex_unlock(&COUNT_MUTEX);

    canCount = false;
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

void printMacroBlocks(MacroBlock* blocks) {
  for (int i = 0; i < MACRO_BLOCK_MAX_SIZE; i++) {
    printf("%d ", blocks[i].rowStart);
    printf("%d ", blocks[i].rowEnd);
    printf("%d ", blocks[i].colStart);
    printf("%d ", blocks[i].colEnd);
    printf("\n");
  }
}

int countPrimesInMatriz(Matriz *matriz)
{
  int primeNumbersInMatriz = 0;
  for (int i = 0; i < matriz->rows; i++)
  {
    for (int j = 0; j < matriz->cols; j++)
    {
      if (isNumberPrime(matriz->matriz[i][j]))
      {
        primeNumbersInMatriz++;
      }
    }
  }
  return primeNumbersInMatriz;
}