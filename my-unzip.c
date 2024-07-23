#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) { // Check that at least one file is given
        printf("my-zip: file1 [file2 ...]\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) { // Go through all given files
        FILE *file = fopen(argv[i], "r");
        if (file == NULL) {
            printf("my-unzip: cannot open file\n");
            return 1;
        }
        
	// Going through the file one character at a time
        int count = 0;
        char ch;
	while (fread(&count, sizeof(int), 1, file) == 1) { // Read the number
	    // Read the char and make sure that the file is in correct format
	    if (fread(&ch, sizeof(char), 1, file) != 1) {
                printf("my-unzip: invalid file format\n");
                return 1;
            } else {
		for (int i = 0; i < count; i++) { // print as many of the chars as indicated
		    printf("%c", ch);
		}
	    }
        }
	
        fclose(file);
    }
    printf("\n");

    return 0;
}
