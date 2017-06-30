#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char ** argv){



    char * fifo_name = argv[1];
    int R = atoi(argv[2]);
    char in[100];
    int read_file_descriptor;
    char * tmp_line_buffer = malloc(100 * sizeof(char));
    char * tmp_num_buffer1;
    char * tmp_num_buffer2 = malloc(100 * sizeof(char));
    char * tmp_num_buffer3 = malloc(100 * sizeof(char));
    long int tmp_re_index;
    long int tmp_im_index;
    int tmp_iters;

    FILE *data_file_handler;
    FILE *gnuplot_handler;
    data_file_handler = fopen("data", "w");

    int ** T = calloc(R,sizeof(int *));
    for (int j = 0; j < R; ++j) {
        T[j] = calloc(R,sizeof(int));
    }


    //mkfifo(fifo_name,S_IRUSR | S_IWUSR);

    read_file_descriptor = open(fifo_name, O_RDONLY);

    //sleep(1);
    // int i = 0;
    int chars_read = 1;
    while(chars_read != 0){
        tmp_re_index = 0;
        tmp_im_index = 0;
        tmp_iters = 0;
        chars_read = read(read_file_descriptor,in, sizeof(in));
        if (chars_read < 0) break;
        in[chars_read] = '\0';
        strcpy(tmp_line_buffer,in);
        tmp_num_buffer1 = strtok(tmp_line_buffer," ");
        //printf("(%lf, ",atof(tmp_num_buffer1));
        if (tmp_num_buffer1 != NULL) tmp_re_index = ((atof(tmp_num_buffer1) + 2) * (R/3));

        //strcpy(tmp_num_buffer2,tmp_num_buffer1);
        tmp_num_buffer1 = strtok(NULL," ");

        //printf("%lf)=>",atof(tmp_num_buffer1));
        if (tmp_num_buffer1 != NULL) tmp_im_index = ((atof(tmp_num_buffer1) + 1) * (R/2));
        //strcpy(tmp_num_buffer3,tmp_num_buffer1);
        tmp_num_buffer1 = strtok(NULL," ");

        //printf(" %d\n",atoi(tmp_num_buffer1));
        if (tmp_num_buffer1 != NULL) tmp_iters = atoi(tmp_num_buffer1);

        T[tmp_re_index%R][tmp_im_index%R] = tmp_iters;


        //printf("Receiver has received %s which is (%d, %d) => %d\n",in,tmp_re_index,tmp_im_index,tmp_iters);
        //i++;
    }

    printf("\nEverything read from slave. Press any key to continue.\n");

    char * data_line_buffer = malloc(30 * sizeof(char));
    char * gnuplot_commands_buffer = malloc(100 * sizeof(char));// = {"set view map","","", "plot 'data' with image"};


    for (int j = 0; j < R; ++j) {
        for (int k = 0; k < R; ++k) {
            // printf("%d %d %d\n",j,k,T[j][k]);
            sprintf(data_line_buffer,"%d %d %d\n",j,k,T[j][k]);
            fputs(data_line_buffer,data_file_handler);
        }
    }



    sprintf(gnuplot_commands_buffer,"set view map\n"
            "set xrange [0:%d]\n"
            "set yrange [0:%d]\n"
            "plot 'data' with image\n",R,R);

    // printf("%s\n",gnuplot_commands_buffer);

    gnuplot_handler = popen("gnuplot -persistant","w");
    fputs(gnuplot_commands_buffer,gnuplot_handler);
    for (int i = 0; i < 4; ++i) {
        fprintf(gnuplot_handler,"%s \n", gnuplot_commands_buffer);
    }

    getc(stdin);
    fflush(gnuplot_handler);
    //pclose(gnuplot_handler);
    fclose(data_file_handler);

    close(read_file_descriptor);

    return 0;
}

//char * x_range = malloc(10 * sizeof(char));
//char * y_range = malloc(10 * sizeof(char));
//sprintf(x_range,"%d",R);
//sprintf(y_range,"%d",R);
/*    char * tmp = malloc(20 * sizeof(char));
    sprintf(tmp,"set xrange [0:%d]",R);
    strcat(gnuplot_commands_buffer[1], tmp);
    sprintf(tmp,"set xrange [0:%d]",R);
    strcat(gnuplot_commands_buffer[2], tmp);*/
