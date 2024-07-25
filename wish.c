#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> // This gives us the wait function
#include <fcntl.h>   // This gives us open function
#include <ctype.h>  // For the isspace function

// Global MAX values
#define MAX_PATHS 100

// Global variable to store the paths
char *path[MAX_PATHS];
int path_count = 1;

void handle_error(void) { // An error handler for basically everything
 	char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}


void initialize_path() {
    path[0] = strdup("/bin");
}

void set_path(char *new_paths) {
    // Free previously allocated path memory
    for (int i = 0; i < path_count; i++) {
        free(path[i]);
    }
    path_count = 0;

    // If there are no new paths, set it to default
    if (new_paths == NULL || strlen(new_paths) == 0) {
        path[0] = strdup("/bin");
        path_count = 1;
        return;
    }

    // Add new paths
    char path_str[MAX_PATHS * 100]; // Make sure the buffer is large enough
    path_str[0] = '\0'; // Initialize the string

    char *token = strtok(new_paths, " ");
    while (token != NULL) { // All the defined paths are added
        if (path_count < MAX_PATHS) {
            path[path_count] = strdup(token);
            strcat(path_str, path[path_count]);
            strcat(path_str, ":"); // Delimiter if many paths
            path_count++;
        } else {
            handle_error();
            break;
        }
        token = strtok(NULL, " ");
    }
    // Take off the colon from the end
    if (strlen(path_str) > 0) {
        path_str[strlen(path_str) - 1] = '\0';
    }
    // Set the PATH environment variable
    setenv("PATH", path_str, 1);
}


void handle_redirection(char *line, char **args, int *redirect_out, char **tothisfile) {
    
    char *redirect_pos = strrchr(line, '>');  // Finds > placement
    if (redirect_pos != NULL) { // If '>' is found
        
        
        if (strrchr(redirect_pos + 1, '>') != NULL) { // Check that only one
            handle_error();
            return;
        }

        *redirect_pos = '\0'; // replace with empty
        *redirect_out = 1; // boolean that yes, we are redirecting

        char *filename_start = redirect_pos + 1; // finding the filename and taking whitespace off around it
        while (isspace((unsigned char)*filename_start)) {
            filename_start++;
        }

        *tothisfile = filename_start;

        
        // Find the end of the filename and put empty to it
        char *filename_end = filename_start;
        while (*filename_end && !isspace((unsigned char)*filename_end)) {
            filename_end++;
        }

        if (filename_end != filename_start) {
            *filename_end = '\0';
        }

        // Remove redirection args from args[]
        char *arg;
        int i = 0;
        while ((arg = args[i]) != NULL) {
            if (strcmp(arg, ">") == 0) {
                args[i] = NULL; // End the arguments here
                break;
            }
            i++;
        }
    } else {
        *redirect_out = 0;
        *tothisfile = NULL;
    }
}

int main(int argc, char *argv[]) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
	FILE *input_source = stdin;  // Default to stdin for interactive mode

	// Check if there is a batch file
    if (argc > 1) {
        // Open the batch file
        input_source = fopen(argv[1], "r");
        if (input_source == NULL) {
            handle_error();
            exit(1);
        }
    }

	initialize_path(); // Initialize the path array with default values

    while(1) {

		if (input_source == stdin) {
            // Print the prompt only in interactive mode
            printf("wish> ");
        }
       
        // Read a line from the input source
        nread = getline(&line, &len, input_source);
        if (nread == -1) { // End of file or read error
            if (input_source != stdin) {
                fclose(input_source);
            }
            break;
        }

        // Remove the newline character
        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0'; // Replace with empty
        }

        // Check for exit command
        if (strcmp(line, "exit") == 0) {
            break;
        }

       
		// Check for path command
        if (strncmp(line, "path", 4) == 0) {
            char *path_cmd = line + 5; // Skip "path " part
            set_path(path_cmd);
            continue;
        }

		// Check for cd command
        if (strncmp(line, "cd", 2) == 0) {
            char *dir = line + 3; // Skip "cd " part
            if (chdir(dir) != 0) { // chdir changes the directory
                handle_error(); // The directory is not correct
            }
            continue;
        }

		// Create a copy of the original line for redirection handling
        char *line_copy = strdup(line);
        if (line_copy == NULL) {
            handle_error();
            exit(1);
        }
 
        char *args[100]; // Here tokens will be saved
        int i = 0; // Counter for the tokens
        char *token = strtok(line_copy, " "); // First token

        while (token != NULL) {
            args[i] = token; // Saves token
            i++;
            token = strtok(NULL, " "); // Next token
		    }
	    args[i] = NULL; // Last one is NULL


		// Check for redirection

		int redirect_out;
		char *tothisfile;
		handle_redirection(line, args, &redirect_out, &tothisfile);


        pid_t pid = fork();
        if (pid == 0) { // Child process
     
            if (redirect_out && tothisfile) {
                int fd = open(tothisfile, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open file in write mode, source https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/ 
                if (fd < 0) {
                    perror("Error opening file");
                    handle_error();
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                dup2(fd, STDERR_FILENO);
                close(fd);
            }

            execvp(args[0], args);
            perror("Error executing command");
            handle_error();
            exit(1);
        } else if (pid > 0) { // Parent process
            wait(NULL);
        } else {
            perror("Error in fork");
            handle_error();
            exit(1);
        }
    }



    free(line);
    return 0;
}
