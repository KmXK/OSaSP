#include <stdio.h>

const int stopChar = 6;

int main(int argc, char const *argv[])
{
	if(argc != 2){
		fprintf(stderr, "Invalid arguments.\nCommand format: %s filename\n", argv[0]);
		fprintf(stderr, "filename: name of file to write.\n");
		return 1;
	}

	FILE *file = fopen(argv[1], "w+");
	if(!file){
		perror("fopen");
		return 1;
	}

	while(1){
		char c = getc(stdin);
		if(c == stopChar)
			break;
		if(fputc(c, file) == EOF){
			perror("fputc");
			return 1;
		}
	}

	if(fclose(file)){
		perror("fclose");
		return 1;
	}

	return 0;
}