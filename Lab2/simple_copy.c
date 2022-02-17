#include <stdio.h>

const int stopChar = 6;

int main(int argc, char const *argv[])
{
	if(argc != 2){
		printf("Invalid arguments.\nCommand format: %s filename\n", argv[0]);
		printf("filename: name of file to write.");
		return 1;
	}

	FILE *file = fopen(argv[1], "w+");
	if(!file){
		printf("Error while creating file.\n");
		return 1;
	}

	while(1){
		char c = getc(stdin);
		if(c == stopChar)
			break;
		if(fputc(c, file) == EOF){
			printf("Error while writing to file!\n");
			return 1;
		}
	}

	if(fclose(file)){
		printf("Error while closing file!\n");
		return 1;
	}

	return 0;
}