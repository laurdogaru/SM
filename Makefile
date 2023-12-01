build:
	gcc scan_serial.c -o scan_serial
	gcc gen_inputs.c -lm -o gen_inputs
	gcc -o scan_omp scan_omp.c -fopenmp
	mpicc scan_mpi.c -o scan_mpi -g
	mpicc scan_mpi_omp.c -o scan_mpi_omp -fopenmp
clean:
	rm scan_serial gen_inputs scan_mpi scan_omp scan_mpi_omp && rm *.txt