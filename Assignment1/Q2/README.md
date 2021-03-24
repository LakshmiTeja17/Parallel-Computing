Commands to use: 

make np=4 in_file_path="sample1.txt" block_rows="2" out_file_path="out.txt"

1-D row block cyclic decomposition used. The no.of rows in each block is represented by the argument block_rows in the above command.

The matrix to be decomposed is given as in_file_path, and the decomposed matrix is given out in out_file_path.

Sample positive definite matrices of the required format can be generated using the command:

python3 gen.py <matrix_size> <sample_file_path>
Ex. python3 gen.py 1000 sample3.txt will generate a 1000 x 1000 matrix in sample3.txt

2 already generated files are given as: sample1.txt (3x3 matrix) and sample2.txt (500 x 500 matrix)