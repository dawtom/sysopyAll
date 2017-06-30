#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "pipes.h"

int main(int argc, char const *argv[]) {
    char * arguments[ARGUMENTS_LIMIT];
    char * programs[PROGRAMS_LIMIT];
    pid_t child_pids[PROGRAMS_LIMIT];
    int read_pipe[2];
    int write_pipe[2];
    char* tmp_line_buffer=NULL;
    size_t line_buffer_size=0;
    int number_of_programs;
    int wpid_status;
    int has_args = 1;
    printf("Enter line> ");
    fflush(stdout);
    while((getline(&tmp_line_buffer,&line_buffer_size,stdin))!=-1) {
        number_of_programs = line_to_programs(tmp_line_buffer, programs);
        for(int i = 0; i < number_of_programs; i++) {
            if(i > 0 && has_args) {
                swap_write_read_pipes((int **) &write_pipe, (int **) &read_pipe);
            }
            if(i==0) {
                pipe(write_pipe);
            }
            if(program_to_args(programs[i], arguments)) {

                has_args = 1;
                if((child_pids[i] = fork())==0) {

                    if(i < number_of_programs - 1) {
                        close(write_pipe[0]);
                        if(dup2(write_pipe[1], STDOUT_FILENO) == -1){
                            printf("Dup2 error");
                            exit(1);
                        }
                        close(write_pipe[1]);

                        // close(write_pipe[0]);
                    }
                    if(i > 0) {
                        close(read_pipe[1]);    // close write

                        if(dup2(read_pipe[0],STDIN_FILENO) == -1){
                            printf("Dup2 error");
                            exit(1);
                        }

                        close(read_pipe[0]);    //close read

                    }
                    if(execvp(*arguments, arguments) == -1){
                        printf("Cannot execute program\n");
                        exit(1);
                    }
                }
            }
            else {
                has_args = 0;
                continue;
            }
            if(i>0) {
                close(read_pipe[0]);
                close(read_pipe[1]);
            }
            if(i<number_of_programs - 1){
                pipe(read_pipe);
            }
        }
        close(write_pipe[1]);
        close(write_pipe[0]);

        for(int i=0 ; i<number_of_programs-1; i++) {
            waitpid(child_pids[i], &wpid_status, 0);
        }
        fflush(stdout);
        printf("Enter line> \n");
    }
    free(tmp_line_buffer);
    return 0;
}

//***********************************************************************************************************
//***********************************************************************************************************
//***********************************************************************************************************

int line_to_programs(char *line, char **programs) {
    int i=0;
    if((programs[0]=strtok(line, "|\n"))!=NULL)
        while (i<PROGRAMS_LIMIT-2 && (programs[++i]=strtok(NULL, "|\n"))!=NULL);
    programs[i]=NULL;
    return i;
}

//***********************************************************************************************************

int program_to_args(char *program, char **args) {
    int i=0;
    if((args[0]=strtok(program, " \t\r\n"))!=NULL)
        while (i<ARGUMENTS_LIMIT-2 && (args[++i]=strtok(NULL, " \t\r\n"))!=NULL);
    args[i]=NULL;
    return i;
}

//***********************************************************************************************************

void swap_write_read_pipes(int **pipe1, int **pipe2) {
    int* tmp = *pipe1;
    *pipe1 = *pipe2;
    *pipe2 = tmp;
}

//***********************************************************************************************************