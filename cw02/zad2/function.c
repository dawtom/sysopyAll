#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <ftw.h>
#include <time.h>

int maximum_number_of_bytes;

int fn(char * fpath, struct stat *stat_buf, int typeflag);

int main(int argc, char * argv[]) {

    if(argc != 3) {
        printf("Wrong number of arguments. It should be: filename, maximum_number_of_bytes\n");
        exit(1);
    }

    if(access(argv[1], F_OK) != 0) {
        printf("Cannot open specified path.\n");
        exit(1);
    }

    maximum_number_of_bytes = atoi(argv[2]);

    __ftw_func_t fun = (__ftw_func_t) &fn;
    if(ftw(argv[1], fun, 5) != 0) {
        printf("Error while traversing file tree using nftw.\n");
        exit(1);
    }

    printf("\n");
    return 0;
}

int fn(char * file_path, struct stat *stat_buf, int typeflag) {


    char tmp;
    int index_iterator = 0;
    int nesting_level = 0;
    int last_slash_occurence_index = -1;

    while((tmp = file_path[index_iterator++]) != 0) {
        if(tmp == '/') {
            ++nesting_level;
            last_slash_occurence_index = index_iterator-1;
        }
    }
    int i = nesting_level;
    int file_path_lenght = strlen(file_path);

    if(last_slash_occurence_index != -1) {
        file_path_lenght = file_path_lenght - last_slash_occurence_index - 1;
    }

    char * actual_name_of_file;

    if((actual_name_of_file = malloc(file_path_lenght + 1)) == NULL) {
        printf("Wrong memory allocation. Exit.\n");
        return -1;
    }

    strcpy(actual_name_of_file, &file_path[last_slash_occurence_index+1]);

    if(S_ISDIR(stat_buf -> st_mode)) {
        i = nesting_level;
        while(i--) printf("..");
        printf("%s/\n", actual_name_of_file);

    } else if (S_ISREG(stat_buf -> st_mode) && stat_buf -> st_size < maximum_number_of_bytes) {
        i = nesting_level;
        while(i--) printf("..");
        printf("%s, ", actual_name_of_file);
        printf("%d B, ", (int) stat_buf -> st_size);
        time_t seconds = time(NULL);
        struct tm * time_str = gmtime(&seconds);
        printf("%d.%d.%d, %d:%d\n", time_str -> tm_mday, time_str -> tm_mon, time_str -> tm_year + 1900,
               time_str -> tm_hour, time_str -> tm_min);
    }

    return 0;
}


