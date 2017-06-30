//
// Created by Dawid Tomasiewicz on 18.03.17.
//


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int close(int file_handler);

void print_info(){
    printf("\tType the number of option\n"
                   "\t1) set read lock\n"
                   "\t2) set write lock\n"
                   "\t3) print all locks\n"
                   "\t4) unlock a character\n"
                   "\t5) read a character from file\n"
                   "\t6) write a character to file\n"
                   "\t0) exit\n\n");
}

int main(int argc, char ** argv){
    char file_name[50];

    strcpy(file_name,argv[1]);

    //printf("%s\n",file_name);

    int file_handler;
    if( (file_handler = open(file_name, O_RDWR)) < 0){
        printf("Cannot read file");
        exit(1);
    }

    int tmp_byte_number = 0;
    int flag;
    struct flock tmp_flock;
    struct flock check_flock;
    long int index;
    long j;
    long size_of_file;
    do{
        char insert_character;
        print_info();
        switch(flag) {
            case 0:
                break;
            case 1:
                printf("option 1 > Type index of character to lock.");
                //long int index;
                scanf("%ld", &index);
                if(index < 0) {
                    printf("Index must be a positive number.\n");
                }
                else{
                    tmp_flock.l_type = F_RDLCK;
                    tmp_flock.l_whence = SEEK_SET;
                    tmp_flock.l_start = index;
                    tmp_flock.l_len = 1;

                    check_flock.l_type = F_WRLCK;
                    check_flock.l_whence = SEEK_SET;
                    check_flock.l_start = index;
                    check_flock.l_len = 1;

                    if(fcntl(file_handler, F_GETLK, &check_flock) == -1) {
                        printf("Error while checking checking locks on this character.\n");
                    }
                    //1printf(" %d ",check_flock.l_type);
                    if(check_flock.l_type == F_RDLCK || check_flock.l_type == F_WRLCK) {
                        printf("Lock on position %ld with PID: %d\nCannot relock this character.\n", j, tmp_flock.l_pid);
                    }
                    else{
                        if(fcntl(file_handler, F_SETLK, &tmp_flock) == -1) {
                            printf("Error while setting a lock.\n");
                        } else {
                            printf("Read lock has been properly set.\n");
                        }
                    }
                }
                break;
            case 2:
                printf("option 2 > Type index of character to lock.");
                //long int index;
                scanf("%ld", &index);
                if(index < 0) {
                    printf("Index must be a positive number.\n");
                }
                else{
                    tmp_flock.l_type = F_WRLCK;
                    tmp_flock.l_whence = SEEK_SET;
                    tmp_flock.l_start = index;
                    tmp_flock.l_len = 1;

                    check_flock.l_type = F_WRLCK;
                    check_flock.l_whence = SEEK_SET;
                    check_flock.l_start = index;
                    check_flock.l_len = 1;
                    if(fcntl(file_handler, F_GETLK, &check_flock) == -1) {
                        printf("Error while checking checking locks on this character.\n");
                    }
                    //1printf(" %d ",check_flock.l_type);
                    if(check_flock.l_type == F_RDLCK || check_flock.l_type == F_WRLCK) {
                        printf("Lock on position %ld with PID: %d\nCannot relock this character.\n", j, tmp_flock.l_pid);
                    }
                    else{
                        if(fcntl(file_handler, F_SETLK, &tmp_flock) == -1) {
                            printf("Error while setting a lock.\n");
                        } else {
                            printf("Write lock has been properly set.\n");
                        }
                    }
                }
                break;
            case 3:

                size_of_file = lseek(file_handler, 0, SEEK_END);
                struct flock fl;
                int number_of_locks = 0;

                for(j = 0; j < size_of_file; j++) {
                    tmp_flock.l_type = F_WRLCK;
                    tmp_flock.l_whence = SEEK_SET;
                    tmp_flock.l_start = j;
                    tmp_flock.l_len = 1;


                    if(fcntl(file_handler, F_GETLK, &tmp_flock) == -1) {
                        perror("Cannot search locks in this file.\n");
                    }
                    if(tmp_flock.l_type == F_RDLCK) {
                        printf("Read lock on position %ld with PID: %d\n", j, tmp_flock.l_pid);
                        number_of_locks++;
                    } else if(tmp_flock.l_type == F_WRLCK) {
                        printf("Write lock on position %ld with PID: %d\n", j, tmp_flock.l_pid);
                        number_of_locks++;
                    }
                }

                printf("%d locks found.\n", number_of_locks);
                break;
            case 4:
                printf("option 4 > Type index of character to unlock.");
                //long int index;
                scanf("%ld", &index);
                if(index < 0) {
                    printf("Index must be a positive number.\n");
                }
                else{
                    tmp_flock.l_type = F_UNLCK;
                    tmp_flock.l_whence = SEEK_SET;
                    tmp_flock.l_start = index;
                    tmp_flock.l_len = 1;

                    if(fcntl(file_handler, F_SETLK, &tmp_flock) == -1) {
                        printf("Error while setting a lock.\n");
                    } else {
                        printf("Read lock has been properly unset.\n");
                    }
                }
                break;
            case 5:
                printf("option 5 > Type index of character to read.");
                //long int index;
                scanf("%ld", &index);
                if(index < 0) {
                    printf("Index must be a positive number.\n");
                }
                else{
                    lseek(file_handler, index, SEEK_SET);
                    tmp_flock.l_type = F_WRLCK;
                    tmp_flock.l_whence = SEEK_SET;
                    tmp_flock.l_start = index;
                    tmp_flock.l_len = 1;
                    if(fcntl(file_handler, F_GETLK, &tmp_flock) == -1) {
                        printf("Cannot get lock from this file.");
                    } else {
                        if(tmp_flock.l_type != F_RDLCK) {
                            if(read(file_handler, &insert_character, 1) == -1) {
                                printf("Cannot read character..\n");
                            } else {
                                printf("Reading character %c\n", insert_character);
                            }
                        } else {
                            printf("Cannot read character because of a lock.\n");
                        }
                    }
                }

                break;
            case 6:

                printf("option 6 > Type index of character to write.");
                //long int index;
                scanf("%ld", &index);
                if(index < 0) {
                    printf("Index must be a positive number.\n");
                }
                else{
                    lseek(file_handler, index, SEEK_SET);
                    tmp_flock.l_type = F_WRLCK;
                    tmp_flock.l_whence = SEEK_SET;
                    tmp_flock.l_start = index;
                    tmp_flock.l_len = 1;

                    if(fcntl(file_handler, F_GETLK, &tmp_flock) == -1) {
                        printf("Cannot get information about the lock\n");
                    } else {
                        getchar();
                        printf("Type a character to insert: ");
                        insert_character = getchar();
                        if(tmp_flock.l_type != F_WRLCK) {
                            if(write(file_handler, &insert_character, 1) == -1) {
                                printf("Cannot write character.\n");
                            } else {
                                printf("Character saved.\n");
                            }
                        } else {
                            printf("Cannot save a character because of a lock.\n");
                        }
                    }
                }

                break;
        }

    }
    while (scanf("%d",&flag) && flag != 0);




    close(file_handler);
    return 0;
}