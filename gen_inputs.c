#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

int main(int argc, char *argv[])
{
    srand((unsigned int)time(NULL));
    printf("%lf\n", DBL_MAX);
    if(DBL_MAX > 167772160) {
        puts("DA");
    }
    int no_files = 10;
    int first_power = 15;
    FILE* fp[no_files];
    char fname[] = "inX.txt";

    for(int i = 0; i < no_files; i++) {
        fname[2] = i + 48;
        fp[i] = fopen(fname, "w+");
        fprintf(fp[i], "%d\n", 1 << (first_power + i));
    }

    double x, a = 10.0;
    int m = 0;
    for (int i = 0; i < 1 << (first_power + no_files); i++) {
        m = i < (1 << first_power) ? 0 : log2(i) - first_power + 1;

        x = ((double)rand()/(double)(RAND_MAX)) * a;

        for(int j = m; j < no_files; j++) {
            fprintf(fp[j], "%lf\n", x);
        }
    }

    for(int i = 0; i < no_files; i++) {
        fclose(fp[i]);
    }
        
    return 0;
}