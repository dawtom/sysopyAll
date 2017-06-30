#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/wait.h>

#define DEFINE_VARIABLE 1
#define DELETE_VARIABLE 2
#define EXECUTE_PROGRAM 3
#define MODE_DEFAULT 4


int main(int argc, char ** argv){


    char buffer[1000];
    char * arg[10];

    FILE * file_handler = fopen(argv[1], "r");

    if (file_handler == NULL){
        printf("Cannot open file");
        exit(1);
    }

    char * word_buffer = malloc(100 * sizeof(char));
    char * tmp_variable_name = malloc(100 * sizeof(char));
    char * tmp_variable_value = malloc(100 * sizeof(char));
    char * tmp_program = malloc(100 * sizeof(char));
    int mode = MODE_DEFAULT;
    int child_PID;
    pid_t parent_PID = getpid();

    while ( fgets (buffer, 1000, file_handler) != NULL ) {

        if (buffer[0] == '#'){
            word_buffer = strtok(buffer, " ");    // variable name

            strcpy(tmp_variable_name,word_buffer);

            word_buffer = strtok(NULL, " ");

            if (word_buffer == NULL){
                mode = DELETE_VARIABLE;
                tmp_variable_name[strlen(tmp_variable_name) - 1] = '\0';
            }
            else{
                strcpy(tmp_variable_value,word_buffer);
                word_buffer = strtok(NULL, " ");
                if (word_buffer != NULL){
                    printf("Improper file format");
                    exit(1);
                }
                tmp_variable_value[strlen(tmp_variable_value) - 1] = '\0';
                mode = DEFINE_VARIABLE;
            }
            tmp_variable_name++;


            //printf("name: %s value: %s\n",tmp_variable_name,tmp_variable_value);
        } else{
            mode = EXECUTE_PROGRAM;
            tmp_program = malloc(100 * sizeof(char));
            strcpy(tmp_program,buffer);
            strtok(tmp_program," ");
            //printf("%s\n",strtok(tmp_program," "));
            //printf("%s",tmp_program);
            for (int i = 0; i < 10; ++i) {
                arg[i] = malloc(20 * sizeof(char));
            }
            strcpy(arg[0],tmp_program);
            int iter = 1;
            while (tmp_program != NULL){
                tmp_program = strtok(NULL, " ");
                if (tmp_program != NULL){
                    strcpy(arg[iter],tmp_program);
                }
                //printf(tmp_program);
                iter++;
                //arg[iter] = NULL;
            }
            arg[iter - 1] = NULL;

            arg[iter - 2][strlen(arg[iter - 2]) - 1] = '\0';

            /*for (int j = 0; j < 10; ++j) {
                printf("?%s?",arg[j]);
            }*/


            //child_PID = fork();
            //printf("\nChild PID: %d\n",child_PID);

            /*char* arg[] = {"ls", "-l", NULL};
            execvp(arg[0],arg);*/
        }
        if (mode == EXECUTE_PROGRAM)
        {
            if (parent_PID == getpid()){
                child_PID = fork();
                if (child_PID == 0) {
                    printf("Program: %s", buffer);
                    if (execvp(arg[0], arg) < 0){
                        printf("Cannot execute program: %sExit.\n",buffer);
                        exit(1);
                    }
                } else{
                    int x = 0;
                    wait(&x);
                    //printf("\n%d\n",x);
                    if (x != 0){
                        exit(1);
                    }
                }

            }
        } else{
            if (mode == DELETE_VARIABLE){
                if (putenv(tmp_variable_name) != 0){
                    printf("Error while deleting variable %s. Exit.",tmp_variable_name);
                    exit(1);
                } else{
                    printf("Variable %s has been successfully deleted.\n",tmp_variable_name);
                }
            } else{
                if (mode == DEFINE_VARIABLE){
                    if (setenv(tmp_variable_name,tmp_variable_value,1) != 0){
                        printf("Error while trying to set environment variable %s to %s. Exit.",
                               tmp_variable_name,tmp_variable_value);
                    } else{
                        printf("Variable %s has been successfully saved with value %s.\n",
                               tmp_variable_name,tmp_variable_value);
                    }
                } else{
                    printf("Something went wrong while executing program or improper lines.txt file");
                    exit(1);
                }
            }
        }

    }
}

//*******************************************************************************
//MY PRACTICE

//child_PID =fork();

/*fclose(file_handler);
char * arg2[] = {"../a.out","name",NULL};
if (execvp(arg2[0],arg2) < 0){
    printf("Cannot launch tester program.\n");
}*/


/*char buffer[10000];

strcpy(buffer, getenv("HOME"));

printf("HOME: %s\n",buffer);

setenv("MYVAR","/home/administrator/Pulpit/Materiały/SysOpy",0);
system("export MYVAR");
strcpy(buffer, getenv("MYVAR"));

printf("MYVAR: %s\n",buffer);

setenv("MYVAR","/home/administrator/Pulpit",1);

strcpy(buffer, getenv("MYVAR"));

printf("MYVAR: %s\n",buffer);

strcpy(buffer,"Example of delimiters using strtok");
char str[80] = "Example of delimiters using strtok";
const char s[2] = " ";
char *token;

*//* get the first token *//*
    token = strtok(str, s);

    *//* walk through other tokens *//*
    while( token != NULL )
    {
        printf( " %s\n", token );

        token = strtok(NULL, s);
    }*/