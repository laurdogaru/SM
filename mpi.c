// Usage: "mpirun -np X mpi Y", where X is the number of processes and Y
// should be a number from 0 to 9 and represents which input file will be used
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    double *a, *a2;
	int pid, np, elements_per_process, n_elements_recieved, n;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	// master process
	if (pid == 0) {
        FILE* fp;
        char fname[] = "in0.txt";
        int file_no = atoi(argv[1]);

        fname[2] = file_no + 48;
        fp = fopen(fname, "r");

        fscanf(fp, "%d", &n);

        a = (double*)malloc(n * sizeof(double));

        for(int i = 0; i < n; i++) {
            fscanf(fp, "%lf", &a[i]);
        }

        fclose(fp);

        int index, i;
		elements_per_process = n / np;

        // distributes the portion of array
        // to child processes to calculate
        // their partial sums
        for (i = 1; i < np - 1; i++) {
            index = i * elements_per_process;

            MPI_Send(&elements_per_process,
                    1, MPI_DOUBLE, i, 0,
                    MPI_COMM_WORLD);
            MPI_Send(&a[index],
                    elements_per_process,
                    MPI_DOUBLE, i, 0,
                    MPI_COMM_WORLD);
        }

        // last process adds remaining elements
        index = i * elements_per_process;
        int elements_left = n - index;

        MPI_Send(&elements_left,
                1, MPI_DOUBLE,
                i, 0,
                MPI_COMM_WORLD);
        MPI_Send(&a[index],
                elements_left,
                MPI_DOUBLE, i, 0,
                MPI_COMM_WORLD);

		// master process add its own sub array
		double sum = 0;
		for (i = 0; i < elements_per_process; i++)
			sum += a[i];

		// collects partial sums from other processes
		double tmp;
		for (i = 1; i < np; i++) {
			MPI_Recv(&tmp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
            &status);
			sum += tmp;
		}

		// prints the final sum of array
		printf("Sum of array is : %lf\n", sum);
	}
	// slave processes
	else {
		MPI_Recv(&n_elements_recieved,
				1, MPI_DOUBLE, 0, 0,
				MPI_COMM_WORLD,
				&status);

        a2 = (double*)malloc(n_elements_recieved * 2 * sizeof(double));

        // a2 stores the received array segment
		MPI_Recv(a2, n_elements_recieved,
				MPI_DOUBLE, 0, 0,
				MPI_COMM_WORLD,
				&status);

		// calculates its partial sum
		double partial_sum = 0;
		for (int i = 0; i < n_elements_recieved; i++) {
            sleep(0.001);
			partial_sum += a2[i];
        }

		// sends the partial sum to the root process
		MPI_Send(&partial_sum, 1, MPI_DOUBLE,
				0, 0, MPI_COMM_WORLD);
        free(a2);
	}

	// cleans up all MPI state before exit of process
	MPI_Finalize();

	return 0;
}
