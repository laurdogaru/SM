// Usage: "./scan_omp X", where X should be a number from 0 to 9 and represents
// which input file will be used
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <unistd.h>
#include "omp.h"

void computeparallelprefix(double *iplist, double *_pprefixsum, unsigned long size) {
    int nthr;
    double *z, *x = _pprefixsum;
    #pragma omp parallel
    {
        int i;
        #pragma omp single
        {
            nthr = omp_get_num_threads();
            z = malloc(sizeof(int)*nthr+1);
            z[0] = 0.0f;
        }

        int tid = omp_get_thread_num();
        double sum = 0.0f;

        #pragma omp for schedule(static)
        for(i = 0; i < size; i++) {
            sum += iplist[i];
            x[i] = sum;
            sleep(0.001);
        }

        z[tid+1] = sum;

        #pragma omp barrier

        double offset = 0.0f;
        for(i = 0; i < (tid+1); i++) {
            offset += z[i];
        }

        #pragma omp for schedule(static)
        for(i = 0; i < size; i++) {
            x[i] += offset;
        }
    }
    free(z);
}

int main(int argc, char *argv[]) {
    omp_set_num_threads(2);
    FILE* fp;
    char fname[] = "inX.txt";
    int file_no = atoi(argv[1]);
    fname[2] = file_no + 48;
    fp = fopen(fname, "r");

    int n;
    fscanf(fp, "%d", &n);


    double *a, *pprefixsum ;

    a = (double*)malloc(n * sizeof(double));
    pprefixsum = (double*)malloc(n * sizeof(double));

    for(int i = 0; i < n; i++) {
        fscanf(fp, "%lf", &a[i]);
    }

    // for(int i=0; i<n; i++) printf("%d ", iplist[i]); printf("\n");

    computeparallelprefix(a, pprefixsum, n);

    // for(int i=0; i<n; i++) printf("%lf ", pprefixsum[i]); printf("\n");

    // for(int i=0; i<n; i++) printf("%d ", (i+1)*(i+2)/2); printf("\n");

    printf("%lf ", pprefixsum[n-1]);
    puts("");

    return 0;
}