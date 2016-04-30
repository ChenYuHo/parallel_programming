#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#define SUBDIMENSION 3
#define MIN_NUM 1
#define MAX_NUM 9
#define TOTAL_NUMS 9
#define ARRAY_SIZE (MIN_NUM + TOTAL_NUMS)
typedef int candidates[ARRAY_SIZE];
void init_candidates(candidates *c){
  int i;
  for (i = MIN_NUM; i <= MAX_NUM; ++i)
    (*c)[i] = 0;
}
 
void use_candidate(candidates *cp, int num){
  (*cp)[num] = 1;
}
 
void restore_candidate(candidates *cp, int num){
  (*cp)[num] = 0;
}
 
typedef struct {
  int has_value;
  int value;
  candidates *row_candidates;
  candidates *col_candidates;
  candidates *square_candidates;
} cell;
 
typedef struct {
  int unset_cells;
  cell cells[ARRAY_SIZE][ARRAY_SIZE];
  candidates rows[ARRAY_SIZE];
  candidates columns[ARRAY_SIZE];
  candidates squares[ARRAY_SIZE];
} board;
 
int square(int row, int col){
  return (((row - 1) / SUBDIMENSION) * SUBDIMENSION) +
    ((col - 1) / SUBDIMENSION) + 1;
}
 
void init_board(board *b){
  int i, j;
  b->unset_cells = TOTAL_NUMS * TOTAL_NUMS;
  for (i = MIN_NUM; i <= MAX_NUM; ++i) {
    init_candidates(b->rows + i);
    init_candidates(b->columns + i);
    init_candidates(b->squares + i);
    for (j = MIN_NUM; j <= MAX_NUM; ++j) {
      b->cells[i][j].has_value = 0;
      b->cells[i][j].value = 0;
      b->cells[i][j].row_candidates = b->rows + i;
      b->cells[i][j].col_candidates = b->columns + j;
      b->cells[i][j].square_candidates = b->squares + square(i, j);
    }
  }
}
 
void copy_board(board *a, board *b){
  a->unset_cells = b->unset_cells;
  for (int i = MIN_NUM; i <= MAX_NUM; ++i) {
    memcpy ((a->rows + i), (b->rows + i), sizeof(candidates));
    memcpy ((a->columns + i), (b->columns + i), sizeof(candidates));
    memcpy ((a->squares + i), (b->squares + i), sizeof(candidates));
    for (int j = MIN_NUM; j <= MAX_NUM; ++j) {
      a->cells[i][j].has_value = b->cells[i][j].has_value;
      a->cells[i][j].value = b->cells[i][j].value;
      a->cells[i][j].row_candidates = a->rows + i;
      a->cells[i][j].col_candidates = a->columns + j;
      a->cells[i][j].square_candidates = a->squares + square(i, j);
    }
  }
}
 
int find_common_free(candidates *r, candidates *c, candidates *s, int atleast)
{
  int i;
  for (i = atleast; i <= MAX_NUM; ++i)
    if ((! (*r)[i]) && (! (*c)[i]) && (! (*s)[i]))
      return i;
  return (-1);
}
 
void set_cell(board *b, int r, int c, int val){
  b->unset_cells -= 1;
  b->cells[r][c].has_value = 1;
  b->cells[r][c].value = val;
  use_candidate(b->cells[r][c].row_candidates, val);
  use_candidate(b->cells[r][c].col_candidates, val);
  use_candidate(b->cells[r][c].square_candidates, val);
}
 
void unset_cell(board *b, int r, int c, int val){
  b->unset_cells += 1;
  b->cells[r][c].has_value = 0;
  b->cells[r][c].value = 0;
  restore_candidate(b->cells[r][c].row_candidates, val);
  restore_candidate(b->cells[r][c].col_candidates, val);
  restore_candidate(b->cells[r][c].square_candidates, val);
}
 
int is_set(board *b, int r, int c){
  return (b->cells[r][c].has_value);
}
 
int following(int num){ //number following a given one circularly
  return ((num - MIN_NUM + 1) % TOTAL_NUMS + MIN_NUM);
}
 
int next_cell(int *r, int *c){
  if ((*r) == MAX_NUM && (*c) == MAX_NUM)
    return 0;
  *c = following(*c);
  if ((*c) == MIN_NUM)
    (*r) = following(*r);
  return 1;
}
 
int solve_board(board *b, int r, int c, int recursive){
  int prev;
  int val;
  int sol = 0;
  //board solved
  if (b->unset_cells == 0) return 1;
  //find next unset cell
  while (is_set(b, r, c) && next_cell(&r, &c)) ;
  //try every possible cell value
  prev = MIN_NUM;
  if(recursive){
  #pragma omp parallel for reduction(+ : sol)
    for(int i=1; i<=9;++i){
      board lb;
      copy_board(&lb, b);
      int val = find_common_free(lb.cells[r][c].row_candidates,
                 lb.cells[r][c].col_candidates,
                 lb.cells[r][c].square_candidates,
                 i);
      if(val == i){
        set_cell(&lb, r, c, i);
        sol = solve_board(&lb, MIN_NUM, MIN_NUM, 0);
      }
    }
  }else{
    while (1) {
      int val = find_common_free(b->cells[r][c].row_candidates,
                 b->cells[r][c].col_candidates,
                 b->cells[r][c].square_candidates,
                 prev);
      if (val == -1)
        break;
      set_cell(b, r, c, val);
      sol += solve_board(b,r,c, 0);
      unset_cell(b, r, c, val);
      prev = val+1;
    }
  }
  return sol;
}
 
int read_board(board* b){
  int num;
  int firstZero=-1;
  for(int row = MIN_NUM; row<=MAX_NUM;++row){
    for(int col = MIN_NUM; col<=MAX_NUM;++col){
      scanf("%d", &num);
      if(num != 0) set_cell(b, col, row, num);
      else if (firstZero == -1) firstZero = col*9+row;
    }
  }
  return firstZero;
}
 
int main(){
  omp_set_nested(1);
  board b;
  init_board(&b);
  int firstZero = read_board(&b);
  int row = firstZero/9;
  int col = firstZero%9;
  omp_set_num_threads(9);
  int ret = 0;
#pragma omp parallel for reduction(+ : ret)
  for(int i=1; i<=9;++i){
    board lb;
    copy_board(&lb, &b);
    int val = find_common_free(lb.cells[row][col].row_candidates,
               lb.cells[row][col].col_candidates,
               lb.cells[row][col].square_candidates,
               i);
    if(val == i){
      set_cell(&lb, row, col, i);
      ret = solve_board(&lb, MIN_NUM, MIN_NUM, 1);
    }
  }
  printf("%d\n", ret);
  return 0;
}
