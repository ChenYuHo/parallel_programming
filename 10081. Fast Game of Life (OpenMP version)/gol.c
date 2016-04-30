#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXN 2048
#define aliveNeighbor(g,i,j) g[i-1][j-1]+g[i-1][j]+g[i-1][j+1]+g[i][j-1]+g[i][j+1]+g[i+1][j-1]+g[i+1][j]+g[i+1][j+1]
char game[2][MAXN][MAXN]; //double buffering, initialized as 0
int main() {
    int m, n;
    while (scanf("%d %d\n", &n, &m) == 2) {
        for (int i = 1; i <= n; ++i) { //read input
            fgets(game[0][i]+1, MAXN, stdin);
            for (int j = 1; j <= n; ++j)
                game[0][i][j] -= '0';
            game[0][i][n+1] = 0;
        }
        int flag = 0;
        for(int g=0;g<m;++g){ //game of life
#pragma omp parallel for
            for(int i=1; i<=n; ++i){
                for(int j=1; j<=n; ++j){
                    int nln = aliveNeighbor(game[flag], i, j);
                    game[!flag][i][j] = game[flag][i][j] ? (nln == 2 || nln == 3) : nln == 3;
                }
            }
            flag = !flag; //current flag
        }
        for (int i = 1; i <= n; i++) { //print result
            for (int j = 1; j <= n; j++)
                game[flag][i][j] += '0';
            game[flag][i][n+1] = '\0';
            puts(game[flag][i]+1);
        }
    }
    return 0;
}
