#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	if(argc != 3){
		printf("Invalid arguments.\nCommand format: %s filename lines_count\n", argv[0]);
        printf("filename: name of file to read.\n");
        printf("lines_count: count of lines in block to read.\nEnter 0 to read file in one moment.\n");
		return 1;
	}

    int lines_count = strtol(argv[2], NULL, 10);
    if(lines_count < 0){
        printf("Invalid count of lines.\n");
        return 1;
    }

	FILE *file = fopen(argv[1], "r");
	if(!file){
		printf("Error while opening file.\n");
		return 1;
	}

    printf("------------------------------\n");

    int i = 0;
    while((i < lines_count || lines_count == 0)){
        // Reading
        char c;
        do
        {
            c = fgetc(file);
            if(c != EOF)
                printf("%c", c);
        } while (c != EOF && c != '\n');
        
        if(c == EOF)
            break;
        
        i++;
        if(i == lines_count){
            printf("Enter ENTER to continue...");
            getc(stdin);
            i = 0;
        }
    }

    printf("------------------------------\nEnd of reading.\n");

	if(fclose(file)){
		printf("Error while closing file!\n");
		return 1;
	}

	return 0;
}