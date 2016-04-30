#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <stdint.h>
 
// generate matrix, row-major
 
uint32_t* rand_gen(uint32_t seed, int R, int C) {
    uint32_t *m = (uint32_t *) malloc(sizeof(uint32_t) * R*C);
    uint32_t x = 2, n = R*C;
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            x = (x * x + seed + i + j)%n;
            m[i*C + j] = x;
        }
    }
    return m;
}
 
uint32_t hash(uint32_t x) {
    return (x * 2654435761LU);
}
// output
uint32_t signature(uint32_t *A, int r, int c) {
    uint32_t h = 0;
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++)
            h = hash(h + A[i*c + j]);
    }
    return h;
}
 
// void p(int *s, int i, int j, int n){
//     if (i==j) printf("do A%d*A%d\n", i, j);
//     else{
//         // printf("(");
//         p(s,i,s[i*n+j]-1, n);
//         p(s,s[i*n+j],j, n);
//         // printf(")");
//     }
// }
 
 
uint32_t* multiply(uint32_t *A, uint32_t *B, int a, int b, int c){
    uint32_t *C = (uint32_t *) malloc(sizeof(uint32_t) * a*c);
#pragma omp parallel for
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < c; j++) {
            uint32_t sum = 0;    // overflow, let it go.
            for (int k = 0; k < b; k++)
                sum += A[i*b+k] * B[k*c+j];
            C[i*c+j] = sum;
        }
    }
    return C;
}
uint32_t* process(uint32_t** matrix, int* size, int *s, int i/*0*/, int j/*5*/, int n){
    int b = s[i*n+j];//3
    if(i==j) return matrix[i];
    else if(i+1==j) return multiply(matrix[i], matrix[j], size[i], size[j], size[j+1]);
    uint32_t *L = process(matrix, size, s, i, b-1, n);//0, 2
    uint32_t *R = process(matrix, size, s, b, j, n);//3, 5
    uint32_t *ans = multiply(L, R, size[i], size[b], size[j+1]);
    free(L);
    free(R);
    return ans;
}
int main() {
    int n;
    while (scanf("%d", &n) == 1) {
        uint32_t* matrix[n];
        int size[n+1];
        for(int i=0;i<=n;++i)
            scanf("%d", &(size[i]));
        int seed;
        for(int i=0;i<n;++i){
            scanf("%d", &seed);
            matrix[i] = rand_gen(seed, size[i], size[i+1]);
        }
        uint64_t dp[n][n];
 
        int *s;
        s = malloc(n*n*sizeof(int));
        memset(dp, 0, sizeof(uint64_t) * n * n);
        int L, i, j, k;
        uint64_t q;
        for (L=2; L<=n; L++) {
            for (i=1; i<=n-L+1; i++) {
                j = i+L-1;
                dp[i-1][j-1] = UINT64_MAX;
                for (k=i; k<=j-1; k++) {
                    q = dp[i-1][k-1] + dp[k][j-1] + size[i-1]*size[k]*size[j];
                    if (q < dp[i-1][j-1]) {
                        dp[i-1][j-1] = q;
                        s[(i-1)*n+j-1] = k;
                    }
                }
            }
        }
        // p(s, 0, n-1, n);
        uint32_t *c = process(matrix, size, s, 0, n-1, n);
        printf("%u\n", signature(c, size[0], size[n]));
        free(s);
    }
    return 0;
}
