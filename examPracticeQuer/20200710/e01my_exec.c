#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define N 200
#define M 30

void my_exec(char *);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of parameters: -program -file");
        exit(1);
    }
    my_exec(argv[1]);
    return 0;
}

void my_exec(char *name) {
    FILE *fp;
    char cmd_buf[N];
    char **cmd;
    char *cmd_tok;
    char cmd_path[M+5];
    int i, j;
    fp = fopen(name, "r");

    while (fgets(cmd_buf, N, fp) != NULL) {
        // remove the newline in the buffer
        if (cmd_buf[strlen(cmd_buf) - 1] == '\n') {
            cmd_buf[strlen(cmd_buf) - 1] = '\0';
        }

        cmd = (char **) malloc(M * sizeof(char *));
        for (i=0; i<M; i++) {
            cmd[i] = (char *) malloc(M * sizeof(char));
        }

        printf("entered loop with: %s\n", cmd_buf);
        // need to tokenize cmd
        i = 0;
        cmd_tok = strtok(cmd_buf, " "); // splits the string
        while (cmd_tok != NULL) {
            strcpy(cmd[i], cmd_tok);
            i++;
            cmd_tok = strtok(NULL, " ");
            // Subsequent calls must specify NULL as the first argument, which 
            // tells the function to continue tokenizing the string you passed in first
        }
        cmd[i] = (char *) 0; // Once setting this, you dereference the allocated memory, this is a MEMORY LEAK

        sprintf(cmd_path, "/bin/%s", cmd[0]); // define /bin/[command]
        if (!fork()) {
            execv(cmd_path, cmd);
        }
        for (i=0; i<M; i++) {
            free(cmd[i]);
        }
        free(cmd);
    }
    fclose(fp);
     
}