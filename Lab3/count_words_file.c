#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){

    int indexes[2];
    int indexesCount = 0;
    char extendedMode = 0;
    char isError = 0;

    for (int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-'){
            for (int j = 1; j < strlen(argv[i]); j++)
            {
                switch (argv[i][j])
                {
                case 'h':{
                    printf("Command format: «%s filepath word»\n", argv[0]);
                    printf("filepath: file's path to count words.\n");
                    printf("word: word to count.\n");
                    return 0;
                }
                case 'e':{
                    extendedMode = 1;
                    break;
                }
                default:
                    fprintf(stderr, "Invalid flag — «%c»\n", argv[i][j]);
                    isError = 1;
                }
            }
            
        }
        else{
            indexes[indexesCount++] = i;
        }
    }

    if(isError){
        return 1;
    }
    
    if(indexesCount != 2){
        fprintf(stderr, "Invalid arguments.\n");
        fprintf(stderr, "Command format: %s filepath word.\n", argv[0]);
        fprintf(stderr, "See «%s -h» to see help page.\n", argv[0]);
        return 1;
    }

    char *filename = argv[indexes[0]];
    char *word = argv[indexes[1]];

    int file = open(filename, O_RDONLY);
    if(file == -1){
        perror("open");
        return 1;
    }

    int wordLength = strlen(word);
    char c;

    int count = 0;
    int current = 0;
    char canBeWord = 1;
    long countBytes = 0;
    char isCorrentWord = 0;

    // Здесь слово = совокупность букв, ограниченная символами табуляции, перехода строки или пробелами
    while(read(file, &c, 1) == 1){
        if(c == ' ' || c == '\t' || c == '\n' || c == '\r'){
            if(isCorrentWord){
                count++;
            }
            current = 0;
            canBeWord = 1;
            isCorrentWord = 0;
        }
        else if(canBeWord) {
            if(isCorrentWord){
                canBeWord = 0;
                isCorrentWord = 0;
            }
            else {
                if(word[current] == c){
                    current++;

                    if(current == wordLength){
                        isCorrentWord = 1;
                    }
                }
                else{
                    current = 0;
                    canBeWord = 0;
                }
            }
        }

        countBytes++;
    }

    if(close(file) == -1){
        perror("close");
        return 1;
    }

    if(extendedMode)
        printf("Bytes\tCount\tPath\n");
    printf("%li\t%i\t%s\n", countBytes, count, filename);

    return 0;
}