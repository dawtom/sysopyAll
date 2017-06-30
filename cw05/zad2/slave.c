#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <complex.h>
#include <stdbool.h>
#include <zconf.h>

#define D_RE_BOTTOM -2
#define D_RE_TOP 1
#define D_IM_BOTTOM -1
#define D_IM_TOP 1

char * fifo_name;
int N;
int K;

long double random_real;
long double random_imaginary;

int iters(long double complex c);

int main(int argc, char ** argv){
    srand(time(NULL));
    char line_buffer[100];
    
    if (argc != 4){
        printf("Wrong arguments number. Exit\n");
        exit(1);
    }

    fifo_name = malloc(20 * sizeof(char));
    strcpy(fifo_name, argv[1]);

    N = atoi(argv[2]);
    K = atoi(argv[3]);


    // ARGUMENTS PARSED

    int  fifo_file_handler;

    mkfifo(fifo_name,0666);


    fifo_file_handler = open("myFifo", O_WRONLY);

    long double complex z;

    for (int i = 0; i < N; ++i) {
        random_real = ((long double)rand()/RAND_MAX*(D_RE_TOP - D_RE_BOTTOM))+D_RE_BOTTOM;
        random_imaginary = ((long double)rand()/RAND_MAX)*(D_IM_TOP - D_IM_BOTTOM)+D_IM_BOTTOM;
        //printf("(%Lf,%Lf)\n",random_real,random_imaginary);
        z = random_real + random_imaginary * I;
        // printf("(%Lf,%Lf) -> %d\n",creall(z),cimagl(z),iters(z));
        sprintf(line_buffer,"%Lf %Lf %d",creall(z), cimagl(z), iters(z));
        write(fifo_file_handler,line_buffer,strlen(line_buffer) + 1);
        //usleep(2);
    }
    // printf("(%Lf,%Lf)\n",creall(z),cimagl(z));
    // printf("\n\n\n%d",iters(z));


    



    return 0;
}

int iters(long double complex c){
    int result = 0;
    long double tmp_complex_abs;
    bool modulus_is_greater_than_two = false;

    long double complex z = 0.0 + 0.0 * I;

    for (int i = 0; i < K && !modulus_is_greater_than_two; ++i) {
        z = cpowl(z,2.0 + 0 * I);
        z = z + c;
        tmp_complex_abs = cabsl(z);
        //printf("modulus(%Lf,%Lf) = %Lf\n",creall(z),cimagl(z),tmp_complex_abs);
        if (tmp_complex_abs > 2.0){
            modulus_is_greater_than_two = true;
        }
        result++;
    }

    return result;
}