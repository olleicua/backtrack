#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _square{
  char* grid;
  int* row_used; // an array of bit arrays to keep track of symbol availability
  int* col_used; // in each row/column
  // the nth bit of the mth element of this array is a 1 iff n is already taken
  // for the mth row
  int size;
} square;

void set_avail(int* array, int a_index, char b_index, bool val) {
  if (val) {
	int mask = 1 << b_index;
	array[a_index] |= mask; // set the value to true
  } else {
	int mask = ~(1 << b_index);
	array[a_index] &= mask; // set the value to false
  }
}

void set_cell(square square, int row, int col, char symbol) { // TODO: make apis consistent
  int index = (square.size * row) + col;
  // old value is available
  set_avail(square.row_used, row, square.grid[index], false);
  set_avail(square.col_used, col, square.grid[index], false);
  // new value is not
  set_avail(square.row_used, row, symbol, true);
  set_avail(square.col_used, col, symbol, true);
  square.grid[index] = symbol;
}

bool any_possibilities(square square, int row, int col) {
  int mask = (1 << square.size) - 1;
  int used_union = (mask & square.row_used[row]) |
	(mask & square.col_used[col]);
  return used_union != mask;
}

bool is_avail(square square, char symbol, int row, int col) {
  int mask = 1 << symbol;
  if ((square.row_used[row] & mask) || (square.col_used[col] & mask)) {
	return false;
  }
  return true;
}

square new_square(int size) {
  int i, j;
  square square;
  square.size = size;
  square.grid = malloc(size * size * sizeof(char));
  square.row_used = malloc(size * sizeof(int));
  square.col_used = malloc(size * sizeof(int));
  
  // initialize to zero the usage arrays
  // and each cell to a symbol outside the normal range
  for (i = 0; i < size; i++) {
	square.row_used[i] = 0;
	square.col_used[i] = 0;
	for (j = 0; j < size; j++) {
	  set_cell(square, i, j, size);	  
	}
  }
  
  // initilize first row and first column
  for (i = 0; i < size; i++) {
	set_cell(square, i, 0, i);
	set_cell(square, 0, i, i);
  }
  return square;
}

void print_square(square square) {
  int i;
  for (i = 0; i < square.size * square.size; i++) {
	if (i % square.size == 0) {
	  printf("\n");
	}
	printf("%d", square.grid[i]);
  }
  printf("\n\n");
}

int backtrack(square square, int row, int col, bool verbose) {

  // check whether the square has been filled
  if (row >= square.size) {
	if (verbose) {
	  print_square(square);
	}
	return 1;
  }
  
  // check whether any symbol is possible for this cell
  if (! any_possibilities(square, row, col)) {
	return 0;
  }

  // determine next cell
  int next_row = row;
  int next_col = col + 1;
  if (next_col >= square.size) {
	next_row = row + 1;
	next_col = 1;
  }
  
  // try each possible symbol for this cell
  int total = 0;
  int i;
  for (i = 0; i < square.size; i++) {
	if (is_avail(square, i, row, col)) {
	  set_cell(square, row, col, i);
	  total += backtrack(square, next_row, next_col, verbose);
	  set_cell(square, row, col, square.size);
	}
  }
  
  return total;
}

int search(int size, bool verbose) {
  square square = new_square(size);
  return backtrack(square, 1, 1, verbose);
}

int main() {
  int i, solutions;
  for (i = 1; i <= 6; i++) {
	solutions = search(i, false);
	printf("found %d squares of size %d\n", solutions, i);
  }
  return 0;
}
