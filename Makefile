build:
	gcc serial.c -o serial
	gcc gen_inputs.c -lm -o gen_inputs
	mpicc mpi_scan.c -o mpi_scan
	mpicc mpi.c -o mpi
clean:
	rm serial gen_inputs && rm *.txt