#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Node {
    char *line;
    struct Node *next;
} Node;

// Makes a new node and checks that mallocs are fine and no errors occur when allocating memory
Node* createNode(char *line) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    newNode->line = strdup(line);
    if (newNode->line == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    newNode->next = NULL;
    return newNode;
}

// Adds the new node to the beginning of the linked list
void addNodeToBeginning(Node **head, char *line) {
    Node *newNode = createNode(line);
    newNode->next = *head;
    *head = newNode;
}

// Frees memory
void freeMemory(Node *head) {
    Node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main(int argc, char *argv[]) {
    // If too many arguments
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    } else if (argc == 3) { // If the input and output files are the same
	if (strcmp(argv[1], argv[2]) == 0) {
 	    fprintf(stderr, "Input and output file must differ\n");
	}
    } 

    // Initialize files to be NULL
    FILE *inputFile = NULL;
    FILE *outputFile = NULL;

    
    if (argc > 1) { // If arguments are given, the first one is the input file
        inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    } else { // If no arguments, reads from stdin
        inputFile = stdin;
    }


    if (argc == 3) { // If 3 arguments, the third one is the output file
        outputFile = fopen(argv[2], "w");
        if (outputFile == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            fclose(inputFile);
            exit(1);
        }
    } else { // Otherwise print to stdout
        outputFile = stdout;
    }

    // For dynamic reading of the inputfile using getline s.t. the line can be any length
    Node *head = NULL;
    char *line = NULL;
    size_t len = 0; 
    ssize_t read;

    while ((read = getline(&line, &len, inputFile)) != -1) {
        addNodeToBeginning(&head, line);
    }

    free(line);
    fclose(inputFile); 

    // Outputting
    Node *current = head;
    while (current != NULL) {
        fprintf(outputFile, "%s", current->line);
        current = current->next;
    }

    freeMemory(head);

    return 0;
}

