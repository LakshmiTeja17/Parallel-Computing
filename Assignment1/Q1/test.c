#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void parprocess(MPI_File *in, MPI_File *out, int rank, int nproc, const int padsize)
{
    MPI_Offset globalstart;
    MPI_Offset globalend;
    MPI_Offset filesize;

    int chunksize;
    char *chunk;

    MPI_File_get_size(*in, &filesize);
    chunksize = filesize / nproc;
    globalstart = rank * chunksize - 1;
    if (rank == 0)
        globalstart = 0;
    globalend = (rank + 1) * chunksize - 1 + padsize;
    if (rank == nproc - 1)
        globalend = filesize - 1;

    chunksize = globalend - globalstart + 1;
    chunk = malloc((chunksize + 1) * sizeof(char));

    MPI_File_read_at_all(*in, globalstart, chunk, chunksize, MPI_CHAR, MPI_STATUS_IGNORE);
    chunk[chunksize] = '\0';

    int locstart = 0, locend = chunksize - 1;
    if (rank != 0)
    {
        while (chunk[locstart] != '\n')
            locstart++;
    }
    if (rank != nproc - 1)
    {
        locend -= padsize;
        while (chunk[locend] != '\n')
            locend++;
        locend--;
    }
    chunksize = locend - locstart + 1;

    int lineno = 1;
    for (int i = locstart; i <= locend; i++)
    {
        char c = chunk[i];
        chunk[i] = (isspace(c) ? c : '0' + (char)rank);
        if (c == '\n')
            lineno++;
        chunk[i] = c;
    }
    printf("%d ", lineno);

    MPI_File_write_at_all(*out, (MPI_Offset)(globalstart + (MPI_Offset)locstart), &(chunk[locstart]), chunksize, MPI_CHAR, MPI_STATUS_IGNORE);

    return;
}

int main(int argc, char **argv)
{

    MPI_File in, out;
    int rank, size;
    int ierr;
    const int padsize = 100;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3)
    {
        if (rank == 0)
            fprintf(stderr, "Usage: %s filepath outfilename\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }

    ierr = MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &in);
    if (ierr)
    {
        if (rank == 0)
            fprintf(stderr, "%s: Couldn't open file %s\n", argv[0], argv[1]);
        MPI_Finalize();
        exit(2);
    }

    ierr = MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &out);
    if (ierr)
    {
        if (rank == 0)
            fprintf(stderr, "%s: Couldn't open output file %s\n", argv[0], argv[2]);
        MPI_Finalize();
        exit(3);
    }

    parprocess(&in, &out, rank, size, padsize);

    MPI_File_close(&in);
    MPI_File_close(&out);

    MPI_Finalize();
    return 0;
}
abc