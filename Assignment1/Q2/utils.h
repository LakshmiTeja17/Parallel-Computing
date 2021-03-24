#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIGITS 9

void open_file(char *filename, MPI_File *file, int rank);
void read_line(MPI_File *file, int row, int total_cols, double *row_arr);
