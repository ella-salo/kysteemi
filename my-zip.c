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
            printf("my-zip: cannot open file\n");
            return 1;
        }


        int count = 0;
        char current, previous;

	if ((current = fgetc(file)) == EOF) { // Puts in the first value for current or finds out 		that the file is empty and we can leave this one
	    break;
        }
	count++;
	previous = current;

	// Going through the file one character at a time and we increase the counter while it's 	    the same as the previous and go back to one if its not
        
	while ((current = fgetc(file)) != EOF) {
	    if (current == previous) {
		count++;
            } else {
		fwrite(&count, 4, 1, stdout); // Writes a 4-byte integer in binary
		fwrite(&previous, 1, 1, stdout); // Writes the ASCII character
		count = 1; // Resetting counter (next will be the first occurance)
	    }
	previous = current;
        }
	
        fclose(file);
    }

    return 0;
}
