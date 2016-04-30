#include "matrix.h"
#include <pthread.h>
#include <stdlib.h>
#define THREADS 10
#define UINT unsigned long
UINT (*thisA)[2048],(*thisB)[2048],(*thisC)[2048];
int thisN;
typedef struct {
  int from, to;
} position;
 
void *pthreadMultiply(void *msg){
    position *param = (position *)msg;
    for(int i=param->from;i< param->to ;++i){
        int x = i/thisN;
        int y = i%thisN;
        unsigned long sum = 0;
        for (int k = 0; k < thisN; k++)
            sum += thisA[x][k] * thisB[y][k];
        thisC[x][y] = sum;
    }
    free(param);
    pthread_exit(NULL);
 
}
 
void multiply(int N, unsigned long A[][2048], unsigned long B[][2048], unsigned long (*C)[2048]) {
    thisA = A;
    thisB = B;
    thisC = C;
    thisN = N;
    UINT tmp;
    for(int i = 0; i < N; i++){
        for(int j = i+1; j < N; j++){
            tmp = thisB[i][j];
            thisB[i][j] = thisB[j][i];
            thisB[j][i] = tmp;
        }
    }
    pthread_t threads[THREADS];
    int per = N*N/THREADS;
    if(N*N%THREADS!=0) per+=1;
    int usedThread = THREADS;
    for (int i=0;i<THREADS;++i){
        position *param = malloc(sizeof(position));
        param->from=i*per;
        if (param->from + per >= N*N){
            param->to= N*N;
            usedThread = i+1;
            pthread_create(&threads[i], NULL, pthreadMultiply, (void *)param);
            break;
        }else param->to = param->from+per;
        pthread_create(&threads[i], NULL, pthreadMultiply, (void *)param);
    }
    for(int i=0;i<usedThread;++i){
        pthread_join(threads[i], NULL);
    }
}
