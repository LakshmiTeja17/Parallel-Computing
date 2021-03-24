all: run
	@mpirun -np ${np} ./run ${in_file_path} ${block_rows} ${out_file_path}
run: matrix_cholesky.c utils.c
	@mpicc matrix_cholesky.c utils.c -lm -o run
clean:
	rm -rf run
	rm -rf out.txt