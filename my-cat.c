#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc == 1) { // If no files are given, we just return
	return 0;
    }

    for (int i = 1; i < argc; i++) { // Looping through all files given as arguments

        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
	    printf("my-cat: cannot open file\n"); // Error in opening a file
	    exit(1);
        }

        char buffer[1024]; // Buffer for the line
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
	    printf("%s", buffer);
        }

        fclose(fp);
    }

    return 0;
}
