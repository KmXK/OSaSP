#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "sys/stat.h"
#include <string.h>

void output_dublicates();
int check_directory(char const *dir_name, int minSize, int maxSize);
void add_path(const char *file_path);

typedef char Path[PATH_MAX];
Path *file_paths;
int file_paths_count = 0;
int file_paths_max_count = 10;
const int file_path_delta = 10;

int main(int argc, char const *argv[])
{
    if(argc != 4){
        fprintf(stderr, "Invalid arguments.\n");
        fprintf(stderr, "Command format: %s directory min_size max_size.\n", argv[0]);
        fprintf(stderr, "directory: start directory to check.\n");
        fprintf(stderr, "min_size: minimal size of file in bytes.\n");
        fprintf(stderr, "max_size: maximal size of file in bytes.\n");
        return 1;
    }

    int min_size = strtol(argv[2], NULL, 10);
    int max_size = strtol(argv[3], NULL, 10);
    if(min_size < 0 || max_size < 0){
        fprintf(stderr, "Size cannot be less than 0 bytes!\n");
        return 1;
    }

    // swap :)
    if(min_size > max_size){
        int temp = min_size;
        min_size = max_size;
        max_size = temp;
    }

    file_paths = malloc(sizeof(Path) * file_path_delta);
    if(!file_paths){
        perror("malloc");
        return 1;
    }

    check_directory(argv[1], min_size, max_size);
    output_dublicates();

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

void output_dublicates(){
    int fDublicates = 0;
    int prevHasDublicate = 0;
    for (size_t i = 0; i < file_paths_count; i++)
    {
        int hasDublicate = 0;
        FILE *file1 = fopen(file_paths[i], "r");
        if(!file1)
            continue;
        for (size_t j = i + 1; j < file_paths_count; j++)
        {
            fseek(file1, SEEK_SET, 0);

            FILE *file2 = fopen(file_paths[j], "r");
            if(!file2)
                continue;

            char c1, c2;
            do
            {
                c1 = fgetc(file1);
                c2 = fgetc(file2);
            } while (c1 != EOF && c2 != EOF && c1 == c2);

            if(c1 == c2 && c1 == EOF){
                if(!fDublicates){
                    fDublicates = 1;
                    printf("Dublicates:\n\n");
                }
                if(!hasDublicate){
                    if(prevHasDublicate)
                        printf("\n");
                    printf("%s\n", file_paths[i]);
                }
                hasDublicate++;
                printf("   EQUALS\n%s\n", file_paths[j]);

                fDublicates = 1;
            }
            
            fclose(file2);
        }

        fclose(file1);

        prevHasDublicate = hasDublicate;

        if(hasDublicate > 1)
            i += hasDublicate - 1;
    }

    if(!fDublicates){
        printf("There are no dublicates.\n");
    }
}

int check_directory(char const *dir_name, int min_size, int max_size){
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
            struct stat file_stat;
            stat(filepath, &file_stat);
            if(file_stat.st_size >= min_size && file_stat.st_size <= max_size)
                add_path(filepath);
        }
        else if(d->d_type == DT_DIR && strcmp(".", d->d_name) * strcmp("..", d->d_name)){
            check_directory(filepath, min_size, max_size);
        }
    }

    if(closedir(current_dir)){
        perror("closedir");
        return 1;
    }

    return 0;
}
