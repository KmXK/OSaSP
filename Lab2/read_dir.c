#include <stdio.h>
#include <dirent.h>

int print_directory(char const *dir_name);

int main(int argc, char const *argv[])
{
    printf("Current directory:\n");
    print_directory("./");
    printf("---------------------\n");
    printf("Root directory:\n");
    print_directory("../");

    return 0;
}

// returns 0 if call is successful
int print_directory(char const *dir_name){
    DIR *current_dir = opendir(dir_name);
    if(!current_dir){
        perror("opendir");
        return 1;
    }

    struct dirent *d;

    int i = 1;
    while((d = readdir(current_dir))){
        printf("%3d.\t%s\n", i++, d->d_name);
    }

    if(closedir(current_dir)){
        perror("closedir");
        return 1;
    }

    return 0;
}
