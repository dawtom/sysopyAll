#include "stdio.h"
#include "sys/types.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "fcntl.h"
#include "bubble.h"
#include "bubbleSort.h"
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>



int sys_or_lib;

FILE * lib_file_handler = NULL;
int system_file_handler = -1;

int bytes_per_record;
int number_of_records;

//*********************************************************************************************

void write_records_lib(FILE *file_handler, int rec_size, int num_rec) {

    if(file_handler == NULL) {
        printf("Null file handler received. Ending program.");
        exit(1);
    }

    FILE * random_file_handler;
    char *  c;
    random_file_handler = fopen("/dev/urandom", "r");

    for(int i = 0; i < num_rec; ++i) {
        for(int j = 0; j < rec_size; ++j) {
            fread(&c,1, 1,random_file_handler);
            fwrite(&c, 1, 1, file_handler);
            //printf("%c\n",c);
        }
    }
    fclose(random_file_handler);
}
//*********************************************************************************************


int generate_lib(int argc, char * argv[]){
    /*if( argc !=  6 ) {
        printf("Wrong arguments");
        return 1;
    }*/

    char * filename = argv[3];
    int size_of_record = atoi(argv[4]);
    int number_of_records = atoi(argv[5]);

    FILE *file_handler;

    if((file_handler = fopen(filename, "wb")) == NULL) {
        printf("Error while opening file.");
        exit(1);
    }

    write_records_lib(file_handler, size_of_record, number_of_records);

    fclose(file_handler);
    return 0;
}

//*********************************************************************************************

void write_records_sys(int file_handler, int rec_size, int num_rec) {

    if(file_handler == 0) {
        printf("Null file handler received. Ending program.");
        exit(1);
    }
    char * rand = malloc(1);

    int random_file_handler;
    random_file_handler = open("/dev/urandom", O_RDONLY);

    for(int i = 0; i < num_rec; ++i) {
        for(int j = 0; j < rec_size; ++j) {
            int x = read(random_file_handler,rand, 1);
            //printf(" %c ",*rand);
            //x = write(&rand, sizeof(rand), file_handler);
            int y = write(file_handler, rand, 1);
            //printf("%d",x);
            //printf("%c",(*c));
        }
    }
    close(random_file_handler);
}

//*********************************************************************************************

int generate_sys(int argc, char * argv[]){
    /*if( argc !=  4 ) {
        printf("Wrong arguments");
        return 1;
    }*/

    char * filename = argv[3];
    int size_of_record = atoi(argv[4]);
    int number_of_records = atoi(argv[5]);

    int file_handler = 0;


    if((file_handler = open(filename, O_CREAT | O_RDWR)) && file_handler==-1) {
        printf("Error while opening file.");
        exit(1);
    }
    //printf("%d", file_handler);
    write_records_sys(file_handler, size_of_record, number_of_records);

    close(file_handler);
    return 0;
}

//*********************************************************************************************

void swap_records_sys(int first, int second) {

    long tmp_position = lseek(system_file_handler, 0, SEEK_CUR);
    if(tmp_position == -1) {
        printf("Wrong position in file. Exit.\n");     //wrong position in file
        exit(1);
    }

    long first_record_start_index = first * bytes_per_record;
    long second_record_start_index = second * bytes_per_record;

    void * first_record_buffer = malloc(bytes_per_record);
    void * temp_buffer = malloc(bytes_per_record);

    if( !first_record_buffer || !temp_buffer) {
        printf("Wrong memory allocation.\n");
        exit(1);
    }

    // read records to their buffers
    if(lseek(system_file_handler, first_record_start_index, SEEK_SET) == -1) {  //set pointer to first_record_start_index
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
    if(read(system_file_handler, first_record_buffer, bytes_per_record) == -1) {
        printf("Cannot read file.\n");
        exit(1);
    }

    if(lseek(system_file_handler, second_record_start_index, SEEK_SET) == -1) { //set pointer to second_record_start_index
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
    if(read(system_file_handler, temp_buffer, bytes_per_record) == -1) {
        printf("Cannot read file\n");
        exit(1);
    }

    // write contents of records to each other
    if(lseek(system_file_handler, first_record_start_index, SEEK_SET) == -1) {  //set pointer
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
    if(write(system_file_handler, temp_buffer, bytes_per_record) == -1) {
        printf("Cannot write to file.\n");
        exit(1);
    }
    if(lseek(system_file_handler, second_record_start_index, SEEK_SET) == -1) {
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
    if(write(system_file_handler, first_record_buffer, bytes_per_record) == -1) {
        printf("Cannot write to file.\n");
        exit(1);
    }

    free(first_record_buffer);
    free(temp_buffer);


    //go back to the first pointer position
    if(lseek(system_file_handler, tmp_position, SEEK_SET) != tmp_position) {
        printf("Cannot set the pointer position in file.\n");
        exit(1);
    }

}

//*********************************************************************************************

void swap_records_lib(int first, int second) {

    long tmp_position = ftell(lib_file_handler);    //set pointer of position to come back later
    if(tmp_position == -1) {
        printf("Cannot set pointer to file.\n");
        exit(1);
    }

    long first_record_start_index = first * bytes_per_record;
    long second_record_start_index = second * bytes_per_record;

    void * first_record_buffer = malloc(bytes_per_record);
    void * temp_buffer = malloc(bytes_per_record);

    if( !first_record_buffer || !temp_buffer) {
        printf("Wrong memory allocation.\n");
        exit(1);
    }

    // read records into their buffers
    if(fseek(lib_file_handler, first_record_start_index, SEEK_SET) != 0) { //set pointer
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
    if(fread(first_record_buffer, bytes_per_record, 1, lib_file_handler) != 1) {    //read one record to frb
        printf("Cannot read file.\n");
        exit(1);
    }
    if(fseek(lib_file_handler, second_record_start_index, SEEK_SET) == -1) {    //set pointer
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
    if(fread(temp_buffer, bytes_per_record, 1, lib_file_handler) != 1) {
        printf("Cannot read file.\n");
        exit(1);
    }

    // actual swap
    if(fseek(lib_file_handler, first_record_start_index, SEEK_SET) == -1) {
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
    if(fwrite(temp_buffer, bytes_per_record, 1, lib_file_handler) == -1) {
        printf("Cannot write to file.\n");
        exit(1);
    }
    if(fseek(lib_file_handler, second_record_start_index, SEEK_SET) == -1) {
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
    if(fwrite(first_record_buffer, bytes_per_record, 1, lib_file_handler) == -1) {
        printf("Cannot write to file.\n");
        exit(1);
    }

    free(first_record_buffer);
    free(temp_buffer);

    //go back to the first pointer position
    if(fseek(lib_file_handler, tmp_position, SEEK_SET) != 0) {
        printf("Cannot set the pointer position in file.\n");
        exit(1);
    }
}

//*********************************************************************************************

void set_pointer_to_beginning_lib() {
    if(fseek(lib_file_handler, 0, SEEK_SET) != 0) { //set pointer to the beginning of the file
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
}

//*********************************************************************************************

void set_pointer_to_beginning_sys() {

    if(lseek(system_file_handler, 0, SEEK_SET) == -1)
    {
        printf("Cannot set pointer to file.\n");
        exit(1);
    }
    return;
}

//*********************************************************************************************

void * sys_next_record()
{

    void * next_record = malloc(bytes_per_record);
    if(next_record == NULL)
    {
        printf("Wrong memory allocation.\n");
        exit(1);
    }

    int bytes_read = read(system_file_handler, next_record, bytes_per_record);  //save the whole record
    if(bytes_read == 0) {
        printf("No records left\n");
        if(lseek(system_file_handler, 0, SEEK_SET) == -1)
        {
            printf("Wrong access to file.\n");
            exit(1);
        }
        printf("Restart search.\n");
        return sys_next_record();
    } else if(bytes_read != bytes_per_record)
    {
        printf("Cannot read bytes.\n");
        exit(1);
    }

    return next_record;
}

//*********************************************************************************************

void * lib_next_record()
{
    void * next_record = malloc(bytes_per_record);
    if(next_record == NULL)
    {
        printf("Wrong memory allocation.\n");
        exit(1);
    }

    int bytes_read = fread(next_record, bytes_per_record, 1, lib_file_handler); //read byte from lib_file_handler

    if(bytes_read != 1 || ferror(lib_file_handler))     //check whether all properly allocated
    {
        printf("Cannot read file.\n");
        exit(1);
    }

    if(feof(lib_file_handler))      //check whether we are at the end
    {
        if(fseek(lib_file_handler, 0, SEEK_SET) != 0) {
            printf("Wrong access to file.\n");
            exit(1);
        }
    }

    return next_record;
}

//*********************************************************************************************

int main(int argc, char * argv[])
{
    srand(time(NULL));
    int k;

    for (k=1;k<argc;k++){
        printf("%s ",argv[k]);
    }

    struct tms *buf = malloc(sizeof(struct tms));
    if(buf == NULL) {
        printf("Wrong memory allocation\n");
        exit(1);
    }

    if(argc != 6) {         //should be 5 arguments
        printf("Invalid arguments. Should be filename, SYS/LIB, name of operation, size of record, number of records and type of library");
        exit(1);
    }
    /*
     *arguments:
     * 1) SYS or LIB
     * 2) which operation (generate/shuffle/sort)
     * 3) name of file
     * 4) number of records
     * 5) record size
     */
    if (strcmp(argv[2],"generate") == 0){
        if (strcmp(argv[1],"SYS") == 0){
            generate_sys(argc,argv);
        } else{
            if (strcmp(argv[1],"LIB") == 0){
                generate_lib(argc,argv);
            } else{
                printf("You can only generate files using system functions (SYS) or library functions (LIB)");
                exit(1);
            }
        }
    }
    else{
        if( access( argv[3], F_OK ) == -1 ) {               //check if file exists
            printf("The file does not exist\n");
        }

        bytes_per_record = atoi(argv[5]);

        if (strcmp(argv[1], "SYS") == 0) {
            //printf("SYS");
            sys_or_lib = 1;

            if((system_file_handler = open(argv[3], O_RDWR)) == -1) {
                printf("The file could not be opened\n");
                exit(1);
            }

            /*struct stat file_stat;
            fstat(system_file_handler, &file_stat);
            if(file_stat.st_size % bytes_per_record != 0) {
                printf("Cannot manage this file. It does not contain integer number of records.\n");
                exit(1);
            }*/
            //number_of_records = file_stat.st_size / bytes_per_record;
            number_of_records = atoi(argv[4]);

        } else if (strcmp(argv[1], "LIB") == 0) {
            //printf("LIB");
            sys_or_lib = 2;

            if((lib_file_handler = fopen(argv[3], "r+b")) == NULL) {        //to read binary file
                printf("The file could not be opened\n");
                exit(1);
            }

            /*struct stat file_stat;
            stat(argv[1], &file_stat);
            if(file_stat.st_size % bytes_per_record) {
                printf("Cannot manage this file. It does not contain integer number of records.\n");
                exit(1);
            }
            number_of_records = file_stat.st_size / bytes_per_record;*/

            number_of_records = atoi(argv[4]);

        } else {
            printf("You can only use system library (SYS) or stdio library (LIB)");
            exit(1);
        }

        long clock = sysconf(_SC_CLK_TCK);

        if (strcmp(argv[2],"sort") == 0){
            if(sys_or_lib == 2) {
                printf(" %d %d ",bytes_per_record,number_of_records);
                sort(&lib_next_record, &swap_records_lib, &set_pointer_to_beginning_lib, bytes_per_record, number_of_records);
            } else {
                sort(&sys_next_record, &swap_records_sys, &set_pointer_to_beginning_sys, bytes_per_record, number_of_records);
            }
        } else {
            if (strcmp(argv[2],"shuffle") == 0){
                if(sys_or_lib == 2) {
                    shuffle(&swap_records_lib, &set_pointer_to_beginning_lib, bytes_per_record, number_of_records);
                } else {
                    shuffle(&swap_records_sys, &set_pointer_to_beginning_sys, bytes_per_record, number_of_records);
                }
            }
        }
        times(buf);

        printf("\nResults:\n\tUser time: %5.3lf s, Sys time: %5.3lf s\n", ((double) buf -> tms_utime) / clock, ((double) buf -> tms_stime) / clock);

    }

    return 0;
}