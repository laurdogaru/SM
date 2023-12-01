// Usage: "mpirun -np X scan_mpi Y", where X is the number of processes and Y
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

        double **partial_sums = (double**)malloc(np * sizeof(double*));
        for(int i = 0; i < np; i++) {
            partial_sums[i] = (double*)malloc(elements_per_process * sizeof(double));
        }

		// master process add its own sub array
		double sum = 0;
		for (i = 0; i < elements_per_process; i++) {
			sum += a[i];
            partial_sums[0][i] = sum;
        }


		// collects partial sums from other processes. Every array of partial
        // sums is stored in the corresponding line from the matrix
		double tmp;
		for (i = 1; i < np; i++) {
            if(i != np - 1) {
                MPI_Recv(partial_sums[i], elements_per_process, MPI_DOUBLE, i,
                0, MPI_COMM_WORLD, &status);
            } else {
                MPI_Recv(partial_sums[i], elements_left, MPI_DOUBLE, i,
                0, MPI_COMM_WORLD, &status);
            }
		}

        for(i = 1; i < np - 1; i++) {
            double x = partial_sums[i-1][elements_per_process - 1];
            for(int j = 0; j < elements_per_process; j++) {
                partial_sums[i][j] += x;
            }
        }
        double x = partial_sums[np - 2][elements_per_process - 1];
        for(int j = 0; j < elements_left; j++) {
            partial_sums[np - 1][j] += x;
        }

		// prints the final sum of array
		printf("Sum of array is : %lf\n", partial_sums[np - 1][elements_left - 1]);
        // printf("Partial sums: ");
        // for(int i = 0; i < np - 1; i++) {
        //     for(int j = 0; j < elements_per_process; j++) {
        //         printf("%lf ", partial_sums[i][j]);
        //     }
        // }
        puts("");
	}
	// slave processes
	else {
		MPI_Recv(&n_elements_recieved,
				1, MPI_DOUBLE, 0, 0,
				MPI_COMM_WORLD,
				&status);

        a2 = (double*)malloc(n_elements_recieved * sizeof(double));

        // a2 stores the received array segment
		MPI_Recv(a2, n_elements_recieved,
				MPI_DOUBLE, 0, 0,
				MPI_COMM_WORLD,
				&status);

		// calculates its partial sum
		for (int i = 0; i < n_elements_recieved; i++) {
            sleep(0.001);
            if(i > 0) {
                a2[i] += a2[i-1];
            }
        }

		// sends the partial sum to the root process
		MPI_Send(a2, n_elements_recieved, MPI_DOUBLE,
				0, 0, MPI_COMM_WORLD);
        // free(a2);
	}

	// cleans up all MPI state before exit of process
	MPI_Finalize();

	return 0;
}
