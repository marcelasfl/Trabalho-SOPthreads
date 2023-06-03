#include <stdio.h>
#include <time.h>
#include <stdlib.h>


struct GenMatrizParams {
  int rows;
  int cols;
  int min;
  int max;
};

int** genMatriz (GenMatrizParams params) {
  int rows = params.rows;
  int cols = params.cols;
  int min = params.min;
  int max = params.max;
  srand(time(NULL));
  int** matriz = new int*[rows];
  for (int i = 0; i < rows; i++) {
    matriz[i] = new int[cols];
    for (int j = 0; j < cols; j++) {
      matriz[i][j] = min + rand() % max;
    }
  }
  return matriz;
}

int isNumberPrime(int num) {
    if (num < 2) {
        return 0;
    }

    for (int i = 2; i <= num / 2; i++) {
        if (num % i == 0) {
            return 0;
        }
    }

    return 1;
}


void printMatriz(int** matriz) {
  for(int i = 0; i < 1000; i++){
        for(int j = 0; j < 1000; j++)
           printf("%5d ", matriz[i][j]);
        printf("\n");
        
    }
}


int countPrimesInMatriz(int** matriz) {
  int count = 0;
  for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 1000; j++) {
      if (isNumberPrime(matriz[i][j])) {
        count++;
      }
    }
  }
  return count;
}