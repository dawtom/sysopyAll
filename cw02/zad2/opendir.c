#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

void listdir(const char *name, int depth, double max_size, int is_absolute)
{
    DIR *dir;
    struct dirent *dir_reader;

    if (!(dir = opendir(name)))
        return;
    if (!(dir_reader = readdir(dir)))
        return;

    char absolute_path[1024];
    getcwd(absolute_path, 1024);
    do {
        char path[1024];


        int size;
        int len = snprintf(path, sizeof(path), "%s/%s", name, dir_reader->d_name);

        if (dir_reader->d_type == DT_DIR) {

            //path[len] = 0;
            if (len < 0 || len>= sizeof(path)){
                printf("Len wrong");
                exit(1);
            }
            if (strcmp(dir_reader->d_name, ".") == 0 || strcmp(dir_reader->d_name, "..") == 0)
                continue;
            printf("%*s[%s] %s\n", depth*2, "", dir_reader->d_name,path);
            listdir(path, depth + 1,max_size,is_absolute);
        }
        else{
            struct stat st;
            char* tmp_path = path;
            //char last_modified[1024];
            time_t last_modified;
            //strcat(tmp_path2, dir_reader->d_name);
            //strcat(absolute_path,"/");
            if (is_absolute){
                strcpy(absolute_path,tmp_path);
            } else {
                tmp_path = tmp_path + 1;
                strcat(absolute_path,tmp_path);
            }
            //printf("%s\n",tmp_path);
            //printf("%s\n\n",absolute_path);
            if (stat(absolute_path, &st) == 0){
                size = st.st_size;
                last_modified = st.st_mtime;
                //strcpy(last_modified,st.st_mtime);
            }
            //printf("%s\n", absolute_path);
            if (size <= max_size){
                printf("%*s- %s  %s  %d %lld ", depth*2, "", dir_reader->d_name, absolute_path, size, (long long) last_modified);

                printf( (st.st_mode & S_IRUSR) ? "r" : "-");
                printf( (st.st_mode & S_IWUSR) ? "w" : "-");
                printf( (st.st_mode & S_IXUSR) ? "x" : "-");
                printf( (st.st_mode & S_IRGRP) ? "r" : "-");
                printf( (st.st_mode & S_IWGRP) ? "w" : "-");
                printf( (st.st_mode & S_IXGRP) ? "x" : "-");
                printf( (st.st_mode & S_IROTH) ? "r" : "-");
                printf( (st.st_mode & S_IWOTH) ? "w" : "-");
                printf( (st.st_mode & S_IXOTH) ? "x" : "-");
                printf("\n");
            }
        }
    } while (dir_reader = readdir(dir));
    closedir(dir);
}

int main(int argc, char ** argv) {

    int is_absolute;
    if (argc != 3){ //expected two arguments
        printf("Expected two arguments: path and maximum size.\n");
        exit(1);
    }

    int max_size = atoi(argv[2]);
    printf("I am here");
    if (argv[1][0] == '/'){
        printf("absolute path\n");
        is_absolute = 1;
    } else{
        if (argv[1][0] == '.'){
            printf("current path\n");
            is_absolute = 0;
        }
        else{
            printf("Wrong path argument\n");
            exit(1);
        }
    }

    listdir(argv[1], 0, max_size, is_absolute);
    return 0;
}