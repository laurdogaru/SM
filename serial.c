#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>


int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));

    FILE* fp;
    char fname[] = "inX.txt";
    int file_no = atoi(argv[1]);
    fname[2] = file_no + 48;
    fp = fopen(fname, "r");
    int n;
    fscanf(fp, "%d", &n);

    double *a;
    a = (double*)malloc(n * sizeof(double));
    for(int i = 0; i < n; i++) {
        fscanf(fp, "%lf", &a[i]);
        a[i] = 1.0f;
    }

    for(int i = 1; i < n; i++) {
        a[i] += a[i-1];
    }

    for(int i = 0; i < 10; i++) {
        printf("%lf ", a[n-1]);
    }
    puts("");
}