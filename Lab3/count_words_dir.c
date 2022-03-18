#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "sys/stat.h"
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_PROCESSES_COUNT 1000

void output_files(int n);
int check_directory(char const *dir_name);
void add_path(const char *file_path);

typedef char Path[PATH_MAX];
Path *file_paths;
int file_paths_count = 0;
int file_paths_max_count = 10;
const int file_path_delta = 10;

const char* word;

int main(int argc, char const *argv[])
{
    if(argc != 4){
        fprintf(stderr, "Invalid arguments.\n");
        fprintf(stderr, "Command format: %s directory word n.\n", argv[0]);
        fprintf(stderr, "directory: start directory to check.\n");
        fprintf(stderr, "word: word to check.\n");
        fprintf(stderr, "n: max count of processes in one moment.\n");
        return 1;
    }

    int n = strtoi(argv[2], NULL, 10);
    // int n = atoi(argv[3]);
    // strtoi я всё-таки на нашёл
    if(n <= 0 && n > MAX_PROCESSES_COUNT){
        fprintf(stderr, "Max processes count cannot be less or equal 0 and more than %i!\n", MAX_PROCESSES_COUNT);
        return 1;
    }

    word = argv[2];

    file_paths = calloc(file_path_delta, sizeof(Path));
    if(!file_paths){
        perror("calloc");
        return 1;
    }

    check_directory(argv[1]);
    output_files(n);

    free(file_paths);

    return 0;
}

void add_path(const char *file_path){
    if(file_paths_max_count == file_paths_count){
        file_paths_max_count += file_path_delta;
        file_paths = realloc(file_paths, sizeof(Path) * file_paths_max_count);
        if(!file_paths){
            perror("realloc");
            file_paths_max_count -= file_path_delta;
            return;
        }
    }

    strcpy(file_paths[file_paths_count++], file_path);
}

void output_files(int n){
    FILE *testFile;
    if(testFile = fopen("./count_words_file.out", "r")){
        fclose(testFile);
    }
    else {
        fprintf(stderr, "File ./count_words_file.out doesn't exist\n");
        return;
    }

    int i = 0;
    int processes_count = 0;
    system("> log.log");

    while(i < file_paths_count){
        if(processes_count == n){
            while(wait(NULL) == -1);
            processes_count--;
        }

        // logging
        // это для того, чтобы проверить, что действительно процессы создаются и закрываются
        char logCmd[60];
        sprintf(logCmd, "ps -h | tail -%i >> log.log && echo >> log.log", 5 + processes_count);
        system(logCmd);

        pid_t pid_t = fork();
        switch(pid_t){
            case -1:{
                perror("fork");
                continue;
            }
            case 0:{
                if(execl("./count_words_file.out", "count_words_file.out", file_paths[i], word, (char *)0) == -1){
                    perror("execl");
                    _exit(127);
                }
            }
            default:{
                processes_count++;
                i++;
            }
        }
    }

    while(wait(NULL) != -1);
}

int check_directory(char const *dir_name){
    DIR *current_dir = opendir(dir_name);
    if(!current_dir){
        perror("opendir");
        return 1;
    }

    struct dirent *d;

    int i = 1;
    while((d = readdir(current_dir))){

        Path filepath;
        sprintf(filepath, "%s/%s", dir_name, d->d_name);
                
        if(d->d_type == DT_REG){
            add_path(filepath);
        }
        else if(d->d_type == DT_DIR && strcmp(".", d->d_name) && strcmp("..", d->d_name)){
            check_directory(filepath);
        }
    }

    if(closedir(current_dir)){
        perror("closedir");
        return 1;
    }

    return 0;
}
