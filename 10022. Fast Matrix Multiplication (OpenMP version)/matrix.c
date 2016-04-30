#include "matrix.h"
#include <stdio.h>
#include <omp.h>
 
void multiply(int N, unsigned long A[][2048], unsigned long B[][2048], unsigned long C[][2048]) {
    unsigned long tmp;
    for(int i = 0; i < N; i++){ //transpose
        for(int j = i+1; j < N; j++){
            tmp = B[i][j];
            B[i][j] = B[j][i];
            B[j][i] = tmp;
        }
    }
#pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            unsigned long sum = 0;    // overflow, let it go.
            for (int k = 0; k < N; k++)
                sum += A[i][k] * B[j][k];
            C[i][j] = sum;
        }
    }
}
