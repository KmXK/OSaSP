#include <stdio.h>
#include "sys/stat.h"

int main(int argc, char const *argv[])
{
    if(argc != 3){
        printf("Invalid count of arguments!\n");
        printf("Command format: %s file1 file2\n", argv[0]);
        printf("file1: file to copy\n");
        printf("file2: copied file name\n");
        return 1;
    }

    FILE *file1 = fopen(argv[1], "r");
    if(!file1){
        printf("Invalid file to copy!\n");
        return 1;
    }
    FILE *file2 = fopen(argv[2], "w+");
    if(!file2){
        printf("Invalid copied file name!\n");
        fclose(file1);
        return 1;
    }

    int return_code = 0;

    // Rights copy
    struct stat file_stat;
    if(stat(argv[1], &file_stat)){
        printf("Error while getting file's stats!\n");
        return_code = 1;
        goto closing_files;
    }

    if(chmod(argv[2], file_stat.st_mode)){
        printf("Error while changing file's rights!\n");
        return_code = 1;
        goto closing_files;
    }

    char c;
    while((c = fgetc(file1)) != EOF){
        if(fputc(c, file2) == EOF){
            printf("Error while writing to file!\n");
            return_code = 1;
            goto closing_files;
        }
    }
    
closing_files:
    if(fclose(file1)){
        printf("Error while closing file!\n");
        fclose(file2);
        return 1;
    }

    if(fclose(file2)){
        printf("Error while closing file!");
        return 1;
    }

    return return_code;
}
