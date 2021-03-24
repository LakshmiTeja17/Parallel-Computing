#include "utils.h"

int block_rows;

int *gen_proc_row_nums(int rank, int nproc, int total_rows, int *proc_rows)
{
    int ptr = 0;
    int *global_rows = (int *)malloc(*proc_rows * sizeof(int));
    for (int i = rank; i * block_rows < total_rows; i += nproc)
    {
        for (int j = 0; j < block_rows; j++)
        {
            if ((i * block_rows + j) < total_rows)
            {
                global_rows[ptr] = i * block_rows + j;
                ptr++;
            }
        }
    }
    *proc_rows = ptr;
    return global_rows;
}

int main(int argc, char **argv)
{
    int rank, nproc;
    double t1, t2, time, final_time;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    MPI_File in, out;
    open_file(argv[1], &in, rank);
    block_rows = atoi(argv[2]);

    char len_str[MAX_DIGITS + 2];
    MPI_File_read(in, len_str, MAX_DIGITS + 1, MPI_CHAR, MPI_STATUS_IGNORE);
    len_str[MAX_DIGITS + 1] = '\0';
    int total_rows = atoi(len_str);
    int total_cols = total_rows;

    int proc_rows = 2 * ceil((double)total_rows / nproc);
    if (proc_rows < block_rows)
        block_rows = proc_rows;

    int *proc_row_nums;
    proc_row_nums = gen_proc_row_nums(rank, nproc, total_rows, &proc_rows);

    double proc_data[proc_rows][total_cols];
    int proc_blocks = ceil((double)proc_rows / block_rows);
    for (int i = 0; i < proc_rows; i++)
    {
        read_line(&in, proc_row_nums[i], total_cols, proc_data[i]);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t1 = MPI_Wtime();

    int active_ptr = 0;
    for (int k = 0; k < total_rows; k++)
    {

        if (k == proc_row_nums[active_ptr])
        {
            double div = sqrt(proc_data[active_ptr][k]);
            for (int j = k; j < total_cols; j++)
            {
                proc_data[active_ptr][j] /= div;
            }

            if ((rank + 1) % nproc != (k / block_rows) % nproc)
                MPI_Send(proc_data[active_ptr], total_cols, MPI_DOUBLE, (rank + 1) % nproc, 0, MPI_COMM_WORLD);

            for (int i = active_ptr + 1; i < proc_rows; i++)
            {
                for (int j = proc_row_nums[i]; j < total_cols; j++)
                {
                    proc_data[i][j] -= (proc_data[active_ptr][proc_row_nums[i]] * proc_data[active_ptr][j]);
                }
            }
            active_ptr++;
        }
        else
        {
            double recv_data[total_cols];
            MPI_Recv(recv_data, total_cols, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if ((rank + 1) % nproc != (k / block_rows) % nproc)
                MPI_Send(recv_data, total_cols, MPI_DOUBLE, (rank + 1) % nproc, 0, MPI_COMM_WORLD);

            for (int i = active_ptr; i < proc_rows; i++)
            {
                for (int j = proc_row_nums[i]; j < total_cols; j++)
                {
                    proc_data[i][j] -= (recv_data[proc_row_nums[i]] * recv_data[j]);
                }
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();
    time = t2 - t1;

    MPI_Reduce(&time, &final_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Time in seconds: %lf\n", final_time);
    }

    MPI_File_open(MPI_COMM_WORLD, argv[3], MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &out);

    if (rank == 0)
        MPI_File_write(out, len_str, MAX_DIGITS + 1, MPI_CHAR, MPI_STATUS_IGNORE);

    char write_block[block_rows * total_cols * (MAX_DIGITS + 1) + 1];
    for (int i = 0; i < proc_blocks; i++)
    {
        int row_ind = i * block_rows;
        write_block[0] = '\0';
        int offset = (MAX_DIGITS + 1) * (1 + total_cols * proc_row_nums[row_ind]);
        char num_str[MAX_DIGITS + 2];

        do
        {
            int row = proc_row_nums[row_ind];
            for (int j = 0; j < total_cols; j++)
            {
                double num = proc_data[row_ind][j];
                sprintf(num_str, "%*.4lf", MAX_DIGITS, num);
                if (j == total_cols - 1)
                    num_str[MAX_DIGITS] = '\n';
                else
                    num_str[MAX_DIGITS] = ' ';
                num_str[MAX_DIGITS + 1] = '\0';
                strcat(write_block, num_str);
            }
            row_ind++;
        } while (row_ind < proc_rows && proc_row_nums[row_ind] == (proc_row_nums[row_ind - 1] + 1));

        MPI_File_write_at(out, offset, write_block, strlen(write_block), MPI_CHAR, MPI_STATUS_IGNORE);
    }

    MPI_File_close(&in);
    MPI_File_close(&out);
    MPI_Finalize();
    return 0;
}
