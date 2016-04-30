#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define MAX(a,b) (((a)>(b))?(a):(b))
int main(){
  int i,j,n,w;
  scanf("%d %d", &n, &w);
  int value[n];
  int weight[n];
  for(i=0;i<n;++i)
    scanf("%d %d", &weight[i], &value[i]);
  int dp[w+1][2];
  for(i=0;i<=w;++i)
    for(j=0;j<2;++j)
      dp[i][j]=0;
  int flag = 0;
  for(i=0;i<n;++i){
#pragma omp parallel for
    for(j=0;j<=w;++j){
      dp[j][!flag] = (j - weight[i] < 0) ? dp[j][flag] : MAX(dp[j][flag], dp[j - weight[i]][flag] + value[i]);
    }
    flag = !flag;
  }
  printf("%d\n",dp[w][flag]);
  return 0;
}
