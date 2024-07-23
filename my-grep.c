#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
   
    if (argc == 1) { // If no arguments are given
        printf("my-grep: searchterm [file ...]\n");
        exit(1);
    }

    char *searchstring = argv[1];

    if (strlen(searchstring) == 0) { // If given string is empty, will not print anything
	exit(0);
    }
    
    // Needed initializations for the getline()
    char *line = NULL; 
    size_t len = 0;


    if (argc == 2) { // If no files are given, read from stdin
        while (getline(&line, &len, stdin) != -1) {
            if (strstr(line, searchstring) != NULL) {
                printf("%s", line);
            }
        }
        free(line);
        return 0; // Natural exit
    }


    // Loop through each file specified in the command line arguments
    for (int i = 2; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");
        if (file == NULL) {
            printf("my-grep: cannot open file\n");
            exit(1);
        }

        while (getline(&line, &len, file) != -1) {
	    // strstr return a pointer to the first index of a substring (searchstring) so 		       if that is not null, then that string is on the line an we print it.
            if (strstr(line, searchstring) != NULL) { 
                printf("%s", line);
            }
        }

        fclose(file);
    }

    free(line);
    return 0;
}
