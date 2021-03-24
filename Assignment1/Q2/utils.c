#include "utils.h"

void open_file(char *filename, MPI_File *file, int rank)
{
    int err;
    err = MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, file);
    if (err && rank == 0)
    {
        fprintf(stderr, "Couldn't open file %s\n", filename);
        MPI_Finalize();
        exit(1);
    }
}

void read_line(MPI_File *file, int row, int total_cols, double *row_arr)
{
    int offset = (MAX_DIGITS + 1) * (1 + row * total_cols);
    char num_str[(MAX_DIGITS + 1) * total_cols + 1];
    char *temp;

    MPI_File_read_at(*file, offset, num_str, (MAX_DIGITS + 1) * total_cols, MPI_CHAR, MPI_STATUS_IGNORE);
    num_str[(MAX_DIGITS + 1) * total_cols] = '\0';
    //printf("read at offset %d: %s\n", offset, num_str);

    temp = num_str;
    for (int i = 0; i < total_cols; i++)
    {
        row_arr[i] = strtod(temp, &temp);
    }

    for (int i = 0; i < row; i++)
    {
        row_arr[i] = 0;
    }
}